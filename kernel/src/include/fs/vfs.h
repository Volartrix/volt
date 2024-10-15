#ifndef __VFS_H__
#define __VFS_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    TYPE_TMPFS = 0
} vfs_drive_type;

typedef enum {
    FLAG_HIDDEN = 0,
    FLAG_WP = 1,
    FLAG_OS_RES = 2 // No reading nor writing unless in the kernel and only readable as a super user
} vfs_flags;

typedef enum {
    ACCESS_RW = 0,
    ACCESS_RO = 1,
    ACCESS_WO = 2
} vfs_access;

// File Types
typedef enum {
    FTYPE_NORMAL = 0,
    FTYPE_STDOUT = 1
} vfs_ftype;

typedef enum {
    SCHEME_ROOT = 1,
    SCHEME_LETTER = 2,
    SCHEME_STRING = 3
} vfs_drive_scheme;
/* 
   Examples for every mounting scheme: 
   1. ROOT: /example/test.txt 
   2. LETTER: C://example/test.txt
   3. STRING (8 CHARS MAX withour NULL so 9): DRIVE://example/test.txt
   
   All of them can except root will be accessed by a URI
*/

typedef struct {
    uint8_t type;
    uint8_t scheme;
    void*  (*find_function     )(void *current_dir, const char *dirname);
    void*  (*find_root_function)(void *fs  );
    void*  (*open_dir_function )(void *dir );
    void*  (*open_file_function)(void *file, uint8_t mode);
    void   (*mkdir_function    )(void *current_dir, const char *dirname);
    void   (*mkfile_function   )(void *current_dir, const char *filename);
    void   (*close_function    )(void *file);
    int    (*write_function    )(void *file, char *buffer, size_t len);
    int    (*read_function     )(void *file, char *buffer, size_t max_len);
    size_t (*length_function   )(void *file);
} vfs_fs;

typedef struct {
    bool present;
    vfs_fs file_system;
} vfs_drive_t;

// Reserved Directories
#define HOME_DIR "~"
#define DEV_ROOT "%"

#endif // __VFS_H__