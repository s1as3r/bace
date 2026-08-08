// files.h
//
// provides types and functions for working with files.
//
// references: raddebugger, mr4th

#ifndef _H_FILES
#define _H_FILES

#include "bace/base.h"
#include "bace/strings.h"
#include "bace/time.h"

typedef u32 FilePropertyFlags;
enum {
  FilePropertyFlag_IsFolder = (1 << 0),
};

typedef u32 AccessFlags;
enum {
  AccessFlag_Read = (1 << 0),
  AccessFlag_Write = (1 << 1),
  AccessFlag_Execute = (1 << 2),
  AccessFlag_Append = (1 << 3),
  AccessFlag_ShareRead = (1 << 4),
  AccessFlag_ShareWrite = (1 << 5),
  AccessFlag_Inherited = (1 << 6),
};

typedef struct FileProperties {
  u64 size;
  DenseTime modified;
  DenseTime created;
  FilePropertyFlags flags;
} FileProperties;

typedef u32 FileIterFlags;
enum {
  FileIterFlag_SkipFolders = (1 << 0),
  FileIterFlag_SkipFiles = (1 << 1),
  FileIterFlag_SkipHiddenFiles = (1 << 2),
  FileIterFlag_Done = (1 << 31),
};

// directory-iteration cursor
// `.memory` is a platform-specific backing storage and should not be read or written
// directly by callers.
typedef struct FileIter {
  FileIterFlags flags;
  u8 memory[800];
} FileIter;

typedef struct FileInfo {
  Str8 name;
  FileProperties props;
} FileInfo;

typedef struct FileID {
  u64 v[3];
} FileID;

typedef struct File {
  u64 u64[1];
} File;

// returns the invalid/no file value
File file_zero(void);

// tests for equality of two file handles.
bool file_match(File a, File b);

// opens `path` read-only, reads its contents into a `Str8` and closes the file.
// does not test for file existance, call `file_path_exists` first.
Str8 data_from_file_path(Arena *arena, Str8 path);

// opens `path` for writing, writes all of `data` and closes the file.
// returns `true` if file opened successfully and every byte of `data` was written
bool write_data_to_file_path(Str8 path, Str8 data);

// like `write_data_to_file_path` but for a `Str8List`'s contents.
bool write_data_list_to_file_path(Str8 path, Str8List list);

// opens `path` for appending, writes `data` starting aththe file's curretn EOF and closes
// the file.
// returns `true` only  if the file opened successfully and every byte of `data` was
// written
bool append_data_to_file_path(Str8 path, Str8 data);

// opens `path` read-only, retrieves its `FileID` and closes the file.
FileID id_from_file_path(Str8 path);

// byte-lexographic ordering of two `FileID` values.
// return value of 0 means the same underlying file.
i64 file_id_compare(FileID a, FileID b);

// reads the byte range `[mn, mx)` of `file` into a `Str8`.
Str8 string_from_file_range(Arena *arena, File file, u64 mn, u64 mx);

// reads a null-terminated string starting at byte offset `off` in `file`.
Str8 file_read_cstring(Arena *arena, File file, u64 off);

// --------------------------
// implemented by each os

// opens the file at `path` with the given `flags`.
// returns a file handle, or a value equal to `file_zero` on failure
File file_open(AccessFlags flags, Str8 path);

// closes a `File` handle previously returned by `file_open.`
void file_close(File file);

// reads the byte range `[mn, mx)` of `file` into `out_data`, returning the number
// of bytes actually read.
u64 file_read(File file, u64 mn, u64 mx, void *out_data);

// conveneience wrapper. reads `sizeof(*ptr)` bytes starting at `off` into `ptr`.
#define file_read_struct(f, off, ptr) file_read((f), (off), (off) + sizeof(*(ptr)), (ptr))

// writes `mx - mn` bytes from `data` to `file` staring at offset `mn`, returning the
// number of bytes actually written.
u64 file_write(File file, u64 mn, u64 mx, void *data);

// sets `file`'s timestamp to `time, returns whether it suceeded.
bool file_set_times(File file, DateTime time);

// retrieves `file`'s properties
FileProperties properties_from_file(File file);

// retrieves an identifier for `file`
FileID id_from_file(File file);

// reserves `size` bytes of underlying storage for `file`
bool file_reserve_size(File file, u64 size);

// deletes the file at `path`. returns whether it succeeded.
bool delete_file_at_path(Str8 path);

// copies  the file at `src` to `dst`. returns whether it succeeded.
bool copy_file_path(Str8 dst, Str8 src);

// moves/renames the file at `src` to `dst`. returns whether it succeeded.
bool move_file_path(Str8 dst, Str8 src);

// resolves `path` to its absolute form.
Str8 full_path_from_path(Arena *arena, Str8 path);

// returns `true` if `path` exists and refers to a file the caller can access
bool file_path_exists(Str8 path);

// returns `true` if `path` exists and refers to a directory.
bool folder_path_exists(Str8 path);

// equivalent to openeing `path` read-only and calling `properties_from_file`.
FileProperties properties_from_file_path(Str8 path);

// begins iterating the entries of directory `path`, honoring `flags` as a filter.
// returns an iterator.
// must be paired with a matching `file_iter_end`.
FileIter *file_iter_begin(Arena *arena, Str8 path, FileIterFlags flags);

// advances `iter` to the next extry matching its filter flags, writing its name and
// properties into `info_out.
// returns `true` if an entry was produced, or `false` once iteration is exhausted.
bool file_iter_next(Arena *arena, FileIter *iter, FileInfo *info_out);

// releases any platform resources held by `iter`.
// must be called once for every `file_iter_begin`, once iteration is done.
void file_iter_end(FileIter *iter);

// creates a directory at `path`. returns whether it succeeded.
bool make_directory(Str8 path);

#endif // !_H_FILES
