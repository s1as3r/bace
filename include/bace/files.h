// files.h

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

File file_zero(void);
bool file_match(File a, File b);

Str8 data_from_file_path(Arena *arena, Str8 path);
bool write_data_to_file_path(Str8 path, Str8 data);
bool write_data_list_to_file_path(Str8 path, Str8List list);
bool append_data_to_file_path(Str8 path, Str8 data);
FileID id_from_file_path(Str8 path);
i64 file_id_compare(FileID a, FileID b);
Str8 string_from_file_range(Arena *arena, File file, u64 mn, u64 mx);
Str8 file_read_cstring(Arena *arena, File file, u64 off);

// implemented by each os
File file_open(AccessFlags flags, Str8 path);
void file_close(File file);
u64 file_read(File file, u64 mn, u64 mx, void *out_data);
#define file_read_struct(f, off, ptr) file_read((f), (off), (off) + sizeof(*(ptr)), (ptr))
u64 file_write(File file, u64 mn, u64 mx, void *data);
bool file_set_times(File file, DateTime time);
FileProperties properties_from_file(File file);
FileID id_from_file(File file);
bool file_reserve_size(File file, u64 size);
bool delete_file_at_path(Str8 path);
bool copy_file_path(Str8 dst, Str8 src);
bool move_file_path(Str8 dst, Str8 src);
Str8 full_path_from_path(Arena *arena, Str8 path);
bool file_path_exists(Str8 path);
bool folder_path_exists(Str8 path);
FileProperties properties_from_file_path(Str8 path);

FileIter *file_iter_begin(Arena *arena, Str8 path, FileIterFlags flags);
bool file_iter_next(Arena *arena, FileIter *iter, FileInfo *info_out);
void file_iter_end(FileIter *iter);

bool make_directory(Str8 path);

#endif // !_H_FILES
