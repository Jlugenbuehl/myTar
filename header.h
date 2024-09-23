#ifndef header_h
#define header_h
#define nameLen 100
#define modeLen 8
#define uidLen 8
#define gidLen 8
#define sizeLen 12
#define mtimeLen 12
#define chksumLen 8
#define typeflagLen 1
#define linknameLen 100
#define magicLen 6
#define versionLen 2
#define unameLen 32
#define gnameLen 32
#define devmajorLen 8
#define devminorLen 8
#define prefixLen 155
#define permissionWidth 10
#define ownerGroupWidth 17
#define sizeWidth 8
#define mtimeWidth 16
#define headerCont 56
#define OFF_name 0
#define OFF_mode 100
#define OFF_uid 108
#define OFF_gid 116
#define OFF_size 124
#define OFF_mtime 136
#define OFF_chksum 148
#define OFF_typeflag 156
#define OFF_linkname 157
#define OFF_magic 257
#define OFF_version 263
#define OFF_uname 265
#define OFF_gname 297
#define OFF_devmajor 329
#define OFF_devminor 337
#define OFF_prefix 345
#define blockSize 512
#include <stdint.h>
#include <stdlib.h>

typedef struct header {

  char name[nameLen];
  char mode[modeLen];
  char uid[uidLen];
  char gid[gidLen];
  char size[sizeLen];
  char mtime[mtimeLen];
  char chksum[chksumLen];
  char typeflag[typeflagLen];
  char linkname[linknameLen];
  char magic[magicLen];
  char version[versionLen];
  char uname[unameLen];
  char gname[gnameLen];
  char devmajor[devmajorLen];
  char devminor[devminorLen];
  char prefix[prefixLen];

} header;


int create_archive(char* filename, int archive_fd, int vflag,int sflag);
int create_header(char* pathname,char* header, int sflag);
void int_to_octal(char* string, int size, unsigned long value);
int insert_special_int(char* where, size_t size, int32_t val);
void print_all_files(char *archive, int argc, char *argv[]);
void print_specific(char *archive, int argc, char *argv[]);
void extract_all(char *archive, int argc, char *argv[]);
void extract_specific(char *archive,int argc, char* argv[]);
int checkPath(char *prefix, char *check);
void getPath(char *buffer,char *path,char *name, char*prefix);
void getPermission(char *permissions,header myHeader);
void getOwnerGroup(char *owner, char *uidStr, char *gidStr);
void printHeader(header myHeader, char *path);
void setValues(char *buffer, header myHeader);

#endif
