#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "bace/linux/base.h"
#include "bace/thread_context.h"
#include "bace/files.h"

global LinuxState linux_state = {0};

void *reserve_memory(u64 size) {
  void *res = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (res == MAP_FAILED) {
    res = 0;
  }
  return res;
}

bool commit_memory(void *ptr, u64 size) {
  i32 res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
  return res == 0 ? true : false;
}

void decommit_memory(void *ptr, u64 size) {
  madvise(ptr, size, MADV_DONTNEED);
  mprotect(ptr, size, PROT_NONE);
}

void release_memory(void *ptr, u64 size) {
  munmap(ptr, size);
}

void *reserve_memory_large(u64 size) {
  void *res = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
  if (res == MAP_FAILED) {
    res = 0;
  }
  return res;
}

bool commit_memory_large(void *ptr, u64 size) {
  i32 res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
  return res == 0 ? true : false;
}

void init_linux_state(void) {
  linux_state.sys_info = (SystemInfo){
      .logical_processor_count = (u32)get_nprocs(),
      .page_size = (u64)getpagesize(),
      .large_page_size = MB(2),
      .allocation_granularity = (u64)getpagesize(),
  };
  linux_state.arena = arena_alloc();

  TCTX *tctx = tctx_alloc();
  tctx_select(tctx);

  bool got_result = 0;
  u8 *buf = 0;
  u64 size = 0;

  Temp scratch = scratch_begin(0, 0);
  for (u64 cap = 4096, r = 0; r < 4; cap *= 2, r += 1) {
    scratch_end(scratch);
    buf = push_array(scratch.arena, u8, cap);
    i32 gethostname_res = gethostname((char *)buf, cap);
    size = cstring8_length(buf);
    if (gethostname_res == 0 && size < cap) {
      got_result = 1;
      break;
    }
  }
  if (got_result && size > 0) {
    Str8 *mname = &linux_state.sys_info.machine_name;
    mname->size = size;
    mname->str = push_array_no_zero(linux_state.arena, u8, size + 1);
    memmove(mname->str, buf, size);
    mname->str[mname->size] = 0;
  }
  scratch_end(scratch);
}

SystemInfo *get_system_info(void) {
  return &linux_state.sys_info;
}

void bace_os_state_init(void) {
  init_linux_state();
}

// dy lib stuff
DyLib lib_open(Str8 path) {
  void *so = dlopen((char *)path.str, RTLD_LAZY | RTLD_LOCAL);
  DyLib result = {(u64)so};
  return result;
}

void lib_close(DyLib lib) {
  void *so = (void *)lib.handle;
  dlclose(so);
}

VoidProc *lib_load_proc(DyLib lib, Str8 name) {
  void *so = (void *)lib.handle;
  VoidProc *proc = (VoidProc *)dlsym(so, (char *)name.str);
  return proc;
}

DateTime lnx_date_time_from_tm(struct tm in, u32 msec) {
  DateTime dt = {0};
  dt.sec = (u16)in.tm_sec;
  dt.min = (u16)in.tm_min;
  dt.hour = (u16)in.tm_hour;
  dt.day = (u16)(in.tm_mday - 1);
  dt.mon = (u16)in.tm_mon;
  dt.year = (u16)(in.tm_year + 1900);
  dt.msec = (u16)msec;
  return dt;
}

struct tm lnx_tm_from_date_time(DateTime dt) {
  struct tm result = {0};
  result.tm_sec = (int)dt.sec;
  result.tm_min = (int)dt.min;
  result.tm_hour = (int)dt.hour;
  result.tm_mday = (int)dt.day + 1;
  result.tm_mon = (int)dt.mon;
  result.tm_year = (int)dt.year - 1900;
  return result;
}

struct timespec lnx_timespec_from_date_time(DateTime dt) {
  struct tm tm_val = lnx_tm_from_date_time(dt);
  time_t seconds = timegm(&tm_val);
  struct timespec result = {0};
  result.tv_sec = seconds;
  return result;
}

DenseTime lnx_dense_time_from_timespec(struct timespec in) {
  DenseTime result = 0;
  {
    struct tm tm_time = {0};
    gmtime_r(&in.tv_sec, &tm_time);
    DateTime date_time = lnx_date_time_from_tm(tm_time, (u32)in.tv_nsec / Million(1));
    result = dense_time_from_date_time(date_time);
  }
  return result;
}

// files stuff
FileProperties lnx_file_properties_from_stat(struct stat *s) {
  FileProperties props = {0};
  props.size = (u64)s->st_size;
  props.created = lnx_dense_time_from_timespec(s->st_ctim);
  props.modified = lnx_dense_time_from_timespec(s->st_mtim);
  if (s->st_mode & S_IFDIR) {
    props.flags |= FilePropertyFlag_IsFolder;
  }
  return props;
}

File file_open(AccessFlags flags, Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  Str8 path_copy = str8_copy(scratch.arena, path);
  int lnx_flags = 0;
  if (flags & AccessFlag_Read && flags & AccessFlag_Write) {
    lnx_flags = O_RDWR;
  } else if (flags & AccessFlag_Write) {
    lnx_flags = O_WRONLY;
  } else if (flags & AccessFlag_Read) {
    lnx_flags = O_RDONLY;
  }
  if (flags & AccessFlag_Append) {
    lnx_flags |= O_APPEND;
  }
  if (flags & (AccessFlag_Write | AccessFlag_Append)) {
    lnx_flags |= O_CREAT;
  }
  lnx_flags |= O_CLOEXEC;
  int fd = open((char *)path_copy.str, lnx_flags, 0755);
  File handle = {0};
  if (fd != -1) {
    handle.u64[0] = (u64)fd;
  }
  scratch_end(scratch);
  return handle;
}

void file_close(File file) {
  if (file_match(file, file_zero())) {
    return;
  }
  int fd = (int)file.u64[0];
  close(fd);
}

u64 file_read(File file, u64 mn, u64 mx, void *out_data) {
  if (file_match(file, file_zero())) {
    return 0;
  }
  int fd = (int)file.u64[0];
  u64 total_num_bytes_to_read = mx > mn ? mx - mn : 0;
  u64 total_num_bytes_read = 0;
  u64 total_num_bytes_left_to_read = total_num_bytes_to_read;
  for (; total_num_bytes_left_to_read > 0;) {
    ssize_t read_result =
        pread(fd, (u8 *)out_data + total_num_bytes_read, total_num_bytes_left_to_read,
              (off_t)(mn + total_num_bytes_read));
    if (read_result >= 0) {
      total_num_bytes_read += (u64)read_result;
      total_num_bytes_left_to_read -= (u64)read_result;
    } else if (errno != EINTR) {
      break;
    }
  }
  return total_num_bytes_read;
}

u64 file_write(File file, u64 mn, u64 mx, void *data) {
  if (file_match(file, file_zero())) {
    return 0;
  }
  int fd = (int)file.u64[0];
  u64 total_num_bytes_to_write = mx > mn ? mx - mn : 0;
  u64 total_num_bytes_written = 0;
  u64 total_num_bytes_left_to_write = total_num_bytes_to_write;
  for (; total_num_bytes_left_to_write > 0;) {
    ssize_t write_result =
        pwrite(fd, (u8 *)data + total_num_bytes_written, total_num_bytes_left_to_write,
               (off_t)(mn + total_num_bytes_written));
    if (write_result >= 0) {
      total_num_bytes_written += (u64)write_result;
      total_num_bytes_left_to_write -= (u64)write_result;
    } else if (errno != EINTR) {
      break;
    }
  }
  return total_num_bytes_written;
}

bool file_set_times(File file, DateTime date_time) {
  if (file_match(file, file_zero())) {
    return 0;
  }
  int fd = (int)file.u64[0];
  struct timespec time = lnx_timespec_from_date_time(date_time);
  struct timespec times[2] = {time, time};
  int futimens_result = futimens(fd, times);
  bool good = (futimens_result != -1);
  return good;
}

FileProperties properties_from_file(File file) {
  if (file_match(file, file_zero())) {
    return (FileProperties){0};
  }
  int fd = (int)file.u64[0];
  struct stat fd_stat = {0};
  int fstat_result = fstat(fd, &fd_stat);
  FileProperties props = {0};
  if (fstat_result != -1) {
    props = lnx_file_properties_from_stat(&fd_stat);
  }
  return props;
}

FileID id_from_file(File file) {
  if (file_match(file, file_zero())) {
    return (FileID){0};
  }
  int fd = (int)file.u64[0];
  struct stat fd_stat = {0};
  int fstat_result = fstat(fd, &fd_stat);
  FileID id = {0};
  if (fstat_result != -1) {
    id.v[0] = fd_stat.st_dev;
    id.v[1] = fd_stat.st_ino;
  }
  return id;
}

bool file_reserve_size(File file, u64 size) {
  int status = fallocate((int)file.u64[0], FALLOC_FL_KEEP_SIZE, 0, (ssize_t)size);
  return status == 0;
}

bool delete_file_at_path(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  bool result = 0;
  Str8 path_copy = str8_copy(scratch.arena, path);
  if (remove((char *)path_copy.str) != -1) {
    result = 1;
  }
  scratch_end(scratch);
  return result;
}

bool copy_file_path(Str8 dst, Str8 src) {
  bool result = 0;
  File src_h = file_open(AccessFlag_Read, src);
  File dst_h = file_open(AccessFlag_Write, dst);
  if (!file_match(src_h, file_zero()) && !file_match(dst_h, file_zero())) {
    int src_fd = (int)src_h.u64[0];
    int dst_fd = (int)dst_h.u64[0];
    FileProperties src_props = properties_from_file(src_h);
    u64 size = src_props.size;
    u64 total_bytes_copied = 0;
    u64 bytes_left_to_copy = size;
    for (; bytes_left_to_copy > 0;) {
      off_t sendfile_off = (off_t)total_bytes_copied;
      ssize_t send_result = sendfile(dst_fd, src_fd, &sendfile_off, bytes_left_to_copy);
      if (send_result <= 0) {
        break;
      }
      u64 bytes_copied = (u64)send_result;
      bytes_left_to_copy -= bytes_copied;
      total_bytes_copied += bytes_copied;
    }
  }
  file_close(src_h);
  file_close(dst_h);
  return result;
}

bool move_file_path(Str8 dst, Str8 src) {
  bool good = 0;
  Temp scratch = scratch_begin(0, 0);
  {
    char *src_cstr = (char *)str8_copy(scratch.arena, src).str;
    char *dst_cstr = (char *)str8_copy(scratch.arena, dst).str;
    int rename_result = rename(src_cstr, dst_cstr);
    good = (rename_result != -1);
  }
  scratch_end(scratch);
  return good;
}

Str8 full_path_from_path(Arena *arena, Str8 path) {
  Temp scratch = scratch_begin(&arena, 1);
  Str8 path_copy = str8_copy(scratch.arena, path);
  char buffer[PATH_MAX] = {0};
  realpath((char *)path_copy.str, buffer);
  Str8 result = str8_copy(arena, str8_cstring(buffer));
  scratch_end(scratch);
  return result;
}

bool file_path_exists(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  Str8 path_copy = str8_copy(scratch.arena, path);
  int access_result = access((char *)path_copy.str, F_OK);
  bool result = 0;
  if (access_result == 0) {
    result = 1;
  }
  scratch_end(scratch);
  return result;
}

bool folder_path_exists(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  bool exists = 0;
  Str8 path_copy = str8_copy(scratch.arena, path);
  DIR *handle = opendir((char *)path_copy.str);
  if (handle) {
    closedir(handle);
    exists = 1;
  }
  scratch_end(scratch);
  return exists;
}

FileProperties properties_from_file_path(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  Str8 path_copy = str8_copy(scratch.arena, path);
  struct stat f_stat = {0};
  int stat_result = stat((char *)path_copy.str, &f_stat);
  FileProperties props = {0};
  if (stat_result != -1) {
    props = lnx_file_properties_from_stat(&f_stat);
  }
  scratch_end(scratch);
  return props;
}

// directory iter

FileIter *file_iter_begin(Arena *arena, Str8 path, FileIterFlags flags) {
  FileIter *base_iter = push_array(arena, FileIter, 1);
  base_iter->flags = flags;
  Linux_FileIter *iter = (Linux_FileIter *)base_iter->memory;
  {
    Str8 path_copy = str8_copy(arena, path);
    iter->dir = opendir((char *)path_copy.str);
    iter->path = path_copy;
  }
  return base_iter;
}

bool file_iter_next(Arena *arena, FileIter *iter, FileInfo *info_out) {
  bool good = 0;
  Linux_FileIter *lnx_iter = (Linux_FileIter *)iter->memory;
  for (; lnx_iter->dir != 0;) {
    lnx_iter->dp = readdir(lnx_iter->dir);
    good = (lnx_iter->dp != 0);

    struct stat st = {0};
    int stat_result = 0;
    if (good) {
      Temp scratch = scratch_begin(&arena, 1);
      Str8 full_path =
          str8f(scratch.arena, "%S/%s", lnx_iter->path, lnx_iter->dp->d_name);
      stat_result = stat((char *)full_path.str, &st);
      scratch_end(scratch);
    }

    bool filtered = 0;
    if (good) {
      filtered = ((st.st_mode == S_IFDIR && iter->flags & FileIterFlag_SkipFolders) ||
                  (st.st_mode == S_IFREG && iter->flags & FileIterFlag_SkipFiles) ||
                  (lnx_iter->dp->d_name[0] == '.' && lnx_iter->dp->d_name[1] == 0) ||
                  (lnx_iter->dp->d_name[0] == '.' && lnx_iter->dp->d_name[1] == '.' &&
                   lnx_iter->dp->d_name[2] == 0));
    }

    if (good && !filtered) {
      info_out->name = str8_copy(arena, str8_cstring(lnx_iter->dp->d_name));
      if (stat_result != -1) {
        info_out->props = lnx_file_properties_from_stat(&st);
      }
      break;
    }

    if (!good) {
      break;
    }
  }
  return good;
}

void file_iter_end(FileIter *iter) {
  Linux_FileIter *lnx_iter = (Linux_FileIter *)iter->memory;
  closedir(lnx_iter->dir);
}

bool make_directory(Str8 path) {
  Temp scratch = scratch_begin(0, 0);
  bool result = 0;
  Str8 path_copy = str8_copy(scratch.arena, path);
  if (mkdir((char *)path_copy.str, 0755) != -1) {
    result = 1;
  } else {
    // match windows behavior
    result = file_path_exists(path);
  }
  scratch_end(scratch);
  return result;
}
