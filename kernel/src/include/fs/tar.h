#ifndef __TAR_H__
#define __TAR_H__

#include <string.h>

#define USTAR_MAGIC "ustar"
#define USTAR_VERSION "00"

typedef struct ustar_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
} ustar_header_t;

struct header_node {
    ustar_header_t header;
    struct header_node *next;
};

#endif // __TAR_H__