#include "bace/win32/base.h"
#include "bace/arena.h"
#include "bace/base_os.h"
#include "bace/thread_context.h"
#include "bace/files.h"
#include "bace/path.h"

global Win32State win32_state = {0};

void *reserve_memory(u64 size) {
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  return result;
}

bool commit_memory(void *ptr, u64 size) {
  bool result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

void decommit_memory(void *ptr, u64 size) {
  VirtualFree(ptr, size, MEM_DECOMMIT);
}

void release_memory(void *ptr, u64 size) {
  (void)size; // not needed
  VirtualFree(ptr, 0, MEM_RELEASE);
}

void *reserve_memory_large(u64 size) {
  // have to commit
  // https://learn.microsoft.com/en-gb/windows/win32/memory/large-page-support
  void *result =
      VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
  return result;
}

bool commit_memory_large(void *ptr, u64 size) {
  (void)ptr, (void)size;
  return true;
}

void init_win32_state(void) {
  SYSTEM_INFO sys_info = {0};
  GetSystemInfo(&sys_info);

  win32_state.sys_info = (SystemInfo){
      .logical_processor_count = (u32)sys_info.dwNumberOfProcessors,
      .page_size = sys_info.dwPageSize,
      .large_page_size = GetLargePageMinimum(),
      .allocation_granularity = sys_info.dwAllocationGranularity,
  };

  TCTX *tctx = tctx_alloc();
  tctx_select(tctx);

  win32_state.arena = arena_alloc();

  u8 buf[MAX_COMPUTERNAME_LENGTH + 1] = {0};
  DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
  if (GetComputerNameA((char *)buf, &size)) {
    win32_state.sys_info.machine_name = str8_copy(win32_state.arena, str8(buf, size));
  }
}

void bace_os_state_init(void) {
  init_win32_state();
}

SystemInfo *get_system_info(void) {
  return &win32_state.sys_info;
}

// dy lib stuff
DyLib lib_open(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  Str16 path16 = str16_from_8(scratch.arena, path);
  HMODULE mod = LoadLibraryW(path16.str);
  DyLib result = {(u64)mod};
  scratch_end(scratch);
  return result;
}

void lib_close(DyLib lib) {
  HMODULE mod = (HMODULE)lib.handle;
  FreeLibrary(mod);
}

VoidProc *lib_load_proc(DyLib lib, Str8 name) {
  HMODULE mod = (HMODULE)lib.handle;
  VoidProc *result = (VoidProc *)GetProcAddress(mod, (LPCSTR)name.str);
  return result;
}

// files
File file_open(AccessFlags flags, Str8 path) {
  File result = {0};
  Temp scratch = scratch_begin(0, 0);
  Str16 path16 = str16_from_8(scratch.arena, path);
  DWORD access_flags = 0;
  DWORD share_mode = 0;
  DWORD creation_disposition = OPEN_EXISTING;
  SECURITY_ATTRIBUTES security_attributes = {sizeof(security_attributes), 0, 0};
  if (flags & AccessFlag_Read) {
    access_flags |= GENERIC_READ;
  }
  if (flags & AccessFlag_Write) {
    access_flags |= GENERIC_WRITE;
  }
  if (flags & AccessFlag_Execute) {
    access_flags |= GENERIC_EXECUTE;
  }
  if (flags & AccessFlag_ShareRead) {
    share_mode |= FILE_SHARE_READ;
  }
  if (flags & AccessFlag_ShareWrite) {
    share_mode |= FILE_SHARE_WRITE | FILE_SHARE_DELETE;
  }
  if (flags & AccessFlag_Write) {
    creation_disposition = CREATE_ALWAYS;
  }
  if (flags & AccessFlag_Append) {
    creation_disposition = OPEN_ALWAYS;
    access_flags |= FILE_APPEND_DATA;
  }
  if (flags & AccessFlag_Inherited) {
    security_attributes.bInheritHandle = 1;
  }
  HANDLE file =
      CreateFileW((WCHAR *)path16.str, access_flags, share_mode, &security_attributes,
                  creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
  if (file != INVALID_HANDLE_VALUE) {
    result.u64[0] = (u64)file;
  } else {
    DWORD err = GetLastError();
    (void)err;
  }
  scratch_end(scratch);
  return result;
}

void file_close(File file) {
  if (file_match(file, file_zero())) {
    return;
  }
  HANDLE handle = (HANDLE)file.u64[0];
  BOOL result = CloseHandle(handle);
  (void)result;
}

u64 file_read(File file, u64 mn, u64 mx, void *out_data) {
  if (file_match(file, file_zero())) {
    return 0;
  }

  HANDLE handle = (HANDLE)file.u64[0];
  u8 *ptr = out_data;
  u64 off = mn;
  while (off != mx) {
    u64 amt64 = mx - off;
    u32 amt32 = (u32)min(MB(32), amt64);
    DWORD read_size = 0;
    OVERLAPPED overlapped = {.Offset = (u32)off, .OffsetHigh = (u32)(off >> 32)};
    if (!ReadFile(handle, ptr, amt32, &read_size, &overlapped)) {
      break;
    }
    ptr += read_size;
    off += read_size;
  }

  u64 total_read_size = off - mn;
  return total_read_size;
}

u64 file_write(File file, u64 mn, u64 mx, void *data) {
  if (file_match(file, file_zero())) {
    return 0;
  }
  HANDLE win_handle = (HANDLE)file.u64[0];
  u64 src_off = 0;
  u64 dst_off = mn;
  u64 total_write_size = (mx >= mn) ? mx - mn : 0;
  for (;;) {
    void *bytes_src = (u8 *)data + src_off;
    u64 bytes_left = total_write_size - src_off;
    DWORD write_size = (DWORD)min(MB(1), bytes_left);
    DWORD bytes_written = 0;
    OVERLAPPED overlapped = {0};
    overlapped.Offset = (dst_off & 0x00000000ffffffffull);
    overlapped.OffsetHigh = (dst_off & 0xffffffff00000000ull) >> 32;
    BOOL success =
        WriteFile(win_handle, bytes_src, write_size, &bytes_written, &overlapped);
    if (success == 0) {
      break;
    }
    src_off += bytes_written;
    dst_off += bytes_written;
    if (bytes_left == 0) {
      break;
    }
  }
  return src_off;
}

void w32_system_time_from_date_time(SYSTEMTIME *out, DateTime *in) {
  out->wYear = (WORD)(in->year);
  out->wMonth = (WORD)(in->mon + 1);
  out->wDay = (WORD)(in->day + 1);
  out->wHour = in->hour;
  out->wMinute = in->min;
  out->wSecond = in->sec;
  out->wMilliseconds = in->msec;
}

bool file_set_time(File file, DateTime time) {
  if (file_match(file, file_zero())) {
    return 0;
  }
  bool result = 0;
  HANDLE handle = (HANDLE)file.u64[0];
  SYSTEMTIME system_time = {0};
  w32_system_time_from_date_time(&system_time, &time);
  FILETIME file_time = {0};
  result = (SystemTimeToFileTime(&system_time, &file_time) &&
            SetFileTime(handle, &file_time, &file_time, &file_time));
  return result;
}

void w32_date_time_from_system_time(DateTime *out, SYSTEMTIME *in) {
  out->year = in->wYear;
  out->mon = in->wMonth - 1;
  out->wday = in->wDayOfWeek;
  out->day = in->wDay - 1;
  out->hour = in->wHour;
  out->min = in->wMinute;
  out->sec = in->wSecond;
  out->msec = in->wMilliseconds;
}

void w32_dense_time_from_file_time(DenseTime *out, FILETIME *in) {
  SYSTEMTIME systime = {0};
  FileTimeToSystemTime(in, &systime);
  DateTime date_time = {0};
  w32_date_time_from_system_time(&date_time, &systime);
  *out = dense_time_from_date_time(date_time);
}

FilePropertyFlags w32_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes) {
  FilePropertyFlags flags = 0;
  if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    flags |= FilePropertyFlag_IsFolder;
  }
  return flags;
}

FileProperties properties_from_file(File file) {
  if (file_match(file, file_zero())) {
    FileProperties r = {0};
    return r;
  }
  FileProperties props = {0};
  HANDLE handle = (HANDLE)file.u64[0];
  BY_HANDLE_FILE_INFORMATION info;
  BOOL info_good = GetFileInformationByHandle(handle, &info);
  if (info_good) {
    u32 size_lo = info.nFileSizeLow;
    u32 size_hi = info.nFileSizeHigh;
    props.size = (u64)size_lo | (((u64)size_hi) << 32);
    w32_dense_time_from_file_time(&props.modified, &info.ftLastWriteTime);
    w32_dense_time_from_file_time(&props.created, &info.ftCreationTime);
    props.flags = w32_file_property_flags_from_dwFileAttributes(info.dwFileAttributes);
  }
  return props;
}

FileID id_from_file(File file) {
  if (file_match(file, file_zero())) {
    FileID r = {0};
    return r;
  }
  FileID result = {0};
  HANDLE handle = (HANDLE)file.u64[0];
  BY_HANDLE_FILE_INFORMATION info;
  BOOL is_ok = GetFileInformationByHandle(handle, &info);
  if (is_ok) {
    result.v[0] = info.dwVolumeSerialNumber;
    result.v[1] = info.nFileIndexLow;
    result.v[2] = info.nFileIndexHigh;
  }
  return result;
}

bool file_reserve_size(File file, u64 size) {
  HANDLE handle = (HANDLE)file.u64[0];

  FILE_ALLOCATION_INFO alloc_info = {0};
  alloc_info.AllocationSize.LowPart = size & UINT32_MAX;
  alloc_info.AllocationSize.HighPart = (LONG)((size >> 32) & UINT32_MAX);

  BOOL is_reserved = SetFileInformationByHandle(handle, FileAllocationInfo, &alloc_info,
                                                sizeof(alloc_info));
  return is_reserved;
}

bool delete_file_at_path(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  Str16 path16 = str16_from_8(scratch.arena, path);
  bool result = DeleteFileW((WCHAR *)path16.str);
  scratch_end(scratch);
  return result;
}

bool copy_file_path(Str8 dst, Str8 src) {
  Temp scratch = scratch_begin(0, 0);
  Str16 dst16 = str16_from_8(scratch.arena, dst);
  Str16 src16 = str16_from_8(scratch.arena, src);
  bool result = CopyFileW((WCHAR *)src16.str, (WCHAR *)dst16.str, 0);
  scratch_end(scratch);
  return result;
}

bool move_file_path(Str8 dst, Str8 src) {
  Temp scratch = scratch_begin(0, 0);
  Str16 dst16 = str16_from_8(scratch.arena, dst);
  Str16 src16 = str16_from_8(scratch.arena, src);
  bool result = MoveFileW((WCHAR *)src16.str, (WCHAR *)dst16.str);
  scratch_end(scratch);
  return result;
}

Str8 full_path_from_path(Arena *arena, Str8 path) {
  Temp scratch = scratch_begin(&arena, 1);
  DWORD buffer_size = (DWORD)max(MAX_PATH, path.size * 2) + 1;
  Str16 path16 = str16_from_8(scratch.arena, path);
  WCHAR *buffer = push_array_no_zero(scratch.arena, WCHAR, buffer_size);
  DWORD path16_size = GetFullPathNameW((WCHAR *)path16.str, buffer_size, buffer, NULL);
  if (path16_size > buffer_size) {
    arena_pop(scratch.arena, buffer_size);
    buffer_size = path16_size + 1;
    buffer = push_array_no_zero(scratch.arena, WCHAR, buffer_size);
    path16_size = GetFullPathNameW((WCHAR *)path16.str, buffer_size, buffer, NULL);
  }
  Str8 full_path = str8_from_16(arena, str16((u16 *)buffer, path16_size));
  scratch_end(scratch);
  return full_path;
}

bool file_path_exists(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  Str16 path16 = str16_from_8(scratch.arena, path);
  DWORD attributes = GetFileAttributesW((WCHAR *)path16.str);
  bool exists = (attributes != INVALID_FILE_ATTRIBUTES) &&
                !!(~attributes & FILE_ATTRIBUTE_DIRECTORY);
  scratch_end(scratch);
  return exists;
}

bool folder_path_exists(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  Str16 path16 = str16_from_8(scratch.arena, path);
  DWORD attributes = GetFileAttributesW((WCHAR *)path16.str);
  bool exists =
      (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
  scratch_end(scratch);
  return exists;
}

FileProperties properties_from_file_path(Str8 path) {
  WIN32_FIND_DATAW find_data = {0};
  Temp scratch_outer = scratch_begin(0, 0);
  Str16 path16 = str16_from_8(scratch_outer.arena, path);
  HANDLE handle = FindFirstFileW((WCHAR *)path16.str, &find_data);
  FileProperties props = {0};
  if (handle != INVALID_HANDLE_VALUE) {
    props.size = compose_64bit(find_data.nFileSizeHigh, find_data.nFileSizeLow);
    w32_dense_time_from_file_time(&props.created, &find_data.ftCreationTime);
    w32_dense_time_from_file_time(&props.modified, &find_data.ftLastWriteTime);
    props.flags =
        w32_file_property_flags_from_dwFileAttributes(find_data.dwFileAttributes);
  } else {
    Temp scratch = scratch_begin(0, 0);
    WCHAR buffer[512] = {0};
    DWORD length = GetLogicalDriveStringsW(sizeof(buffer), buffer);
    u64 last_slash_pos = 0;
    for (; last_slash_pos < path.size;
         last_slash_pos = str8_find_needle(path, last_slash_pos + 1, str8_lit("/"),
                                           StringMatchFlag_SlashInsensitive));
    Str8 path_trimmed = str8_prefix(path, last_slash_pos);
    for (u64 off = 0; off < (u64)length;) {
      Str16 next_drive_string_16 = str16_cstring((u16 *)buffer + off);
      off += next_drive_string_16.size + 1;
      Str8 next_drive_string = str8_from_16(scratch.arena, next_drive_string_16);
      next_drive_string = str8_chop_last_slash(next_drive_string);
      if (str8_match(path_trimmed, next_drive_string, StringMatchFlag_CaseInsensitive)) {
        props.flags |= FilePropertyFlag_IsFolder;
        break;
      }
    }
    scratch_end(scratch);
  }
  FindClose(handle);
  scratch_end(scratch_outer);
  return props;
}

// directory iter

FileIter *file_iter_begin(Arena *arena, Str8 path, FileIterFlags flags) {
  Temp scratch = scratch_begin(&arena, 1);
  Str8 path_with_wildcard = str8_cat(scratch.arena, path, str8_lit("\\*"));
  Str16 path16 = str16_from_8(scratch.arena, path_with_wildcard);
  FileIter *iter = push_array(arena, FileIter, 1);
  iter->flags = flags;
  W32_FileIter *w32_iter = (W32_FileIter *)iter->memory;
  if (path.size == 0) {
    w32_iter->is_volume_iter = 1;
    WCHAR buffer[512] = {0};
    DWORD length = GetLogicalDriveStringsW(sizeof(buffer), buffer);
    Str8List drive_strings = {0};
    for (u64 off = 0; off < (u64)length;) {
      Str16 next_drive_string_16 = str16_cstring((u16 *)buffer + off);
      off += next_drive_string_16.size + 1;
      Str8 next_drive_string = str8_from_16(arena, next_drive_string_16);
      next_drive_string = str8_chop_last_slash(next_drive_string);
      str8_list_push(scratch.arena, &drive_strings, next_drive_string);
    }
    w32_iter->drive_strings = str8_array_from_list(arena, &drive_strings);
    w32_iter->drive_strings_iter_idx = 0;
  } else {
    w32_iter->handle =
        FindFirstFileExW((WCHAR *)path16.str, FindExInfoBasic, &w32_iter->find_data,
                         FindExSearchNameMatch, 0, FIND_FIRST_EX_LARGE_FETCH);
  }
  scratch_end(scratch);
  return iter;
}

bool file_iter_next(Arena *arena, FileIter *iter, FileInfo *info_out) {
  bool result = 0;
  FileIterFlags flags = iter->flags;
  W32_FileIter *w32_iter = (W32_FileIter *)iter->memory;
  if (!w32_iter->is_volume_iter) {
    // file iter
    if (!(flags & (u32)FileIterFlag_Done) && w32_iter->handle != INVALID_HANDLE_VALUE) {
      do {
        // check is usable
        bool usable_file = 1;

        WCHAR *file_name = w32_iter->find_data.cFileName;
        DWORD attributes = w32_iter->find_data.dwFileAttributes;
        if (file_name[0] == '.') {
          if (flags & FileIterFlag_SkipHiddenFiles) {
            usable_file = 0;
          } else if (file_name[1] == 0) {
            usable_file = 0;
          } else if (file_name[1] == '.' && file_name[2] == 0) {
            usable_file = 0;
          }
        }
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
          if (flags & FileIterFlag_SkipFolders) {
            usable_file = 0;
          }
        } else {
          if (flags & FileIterFlag_SkipFiles) {
            usable_file = 0;
          }
        }

        // emit if usable
        if (usable_file) {
          info_out->name = str8_from_16(arena, str16_cstring((u16 *)file_name));
          info_out->props.size = (u64)w32_iter->find_data.nFileSizeLow |
                                 (((u64)w32_iter->find_data.nFileSizeHigh) << 32);
          w32_dense_time_from_file_time(&info_out->props.created,
                                        &w32_iter->find_data.ftCreationTime);
          w32_dense_time_from_file_time(&info_out->props.modified,
                                        &w32_iter->find_data.ftLastWriteTime);
          info_out->props.flags =
              w32_file_property_flags_from_dwFileAttributes(attributes);
          result = 1;
          if (!FindNextFileW(w32_iter->handle, &w32_iter->find_data)) {
            iter->flags |= (u32)FileIterFlag_Done;
          }
          break;
        }
      } while (FindNextFileW(w32_iter->handle, &w32_iter->find_data));
    }
  } else {
    // volume iteration
    result = w32_iter->drive_strings_iter_idx < w32_iter->drive_strings.count;
    if (result != 0) {
      memset(info_out, 0, sizeof(*info_out));
      info_out->name = w32_iter->drive_strings.v[w32_iter->drive_strings_iter_idx];
      info_out->props.flags |= FilePropertyFlag_IsFolder;
      w32_iter->drive_strings_iter_idx += 1;
    }
  }
  if (!result) {
    iter->flags |= (u32)FileIterFlag_Done;
  }
  return result;
}

void file_iter_end(FileIter *iter) {
  W32_FileIter *w32_iter = (W32_FileIter *)iter->memory;
  HANDLE zero_handle;
  memset(&zero_handle, 0, sizeof(zero_handle));
  if (!(memcmp(&zero_handle, &w32_iter->handle, sizeof(zero_handle)) == 0)) {
    FindClose(w32_iter->handle);
  }
}

bool make_directory(Str8 path) {
  bool result = 0;
  Temp scratch = scratch_begin(0, 0);
  Str16 name16 = str16_from_8(scratch.arena, path);
  WIN32_FILE_ATTRIBUTE_DATA attributes = {0};
  GetFileAttributesExW((WCHAR *)name16.str, GetFileExInfoStandard, &attributes);
  if (attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    result = 1;
  } else if (CreateDirectoryW((WCHAR *)name16.str, 0)) {
    result = 1;
  }
  scratch_end(scratch);
  return (result);
}
