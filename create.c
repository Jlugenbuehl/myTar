#include <arpa/inet.h>
#include <stdio.h>
#include <grp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>
#include "header.h"
#include <fcntl.h>
#include <stdint.h>
#define PATH_MAX 256

int create_archive(char* filename, int archivefd,int vflag, int sflag){
  struct stat stats;
  struct dirent *entry;
  DIR *dir;
  char file_contents[512];
  memset(file_contents,'\0',512);
  char header[512];
  memset(header,'\0',512);
  char pathname[PATH_MAX];
  memset(pathname,'\0',sizeof(pathname));
  if(lstat(filename,&stats)==-1){
    return 0;
  }

  if(!S_ISDIR(stats.st_mode)){
      if(vflag){
	printf("%s\n",filename);
      }
      create_header(filename,header,sflag);
      write(archivefd,header,512);
      memset(header,'\0',512);
      size_t bytes_read;
      int file;
      if((file=open(filename, O_RDONLY, S_IRUSR,S_IWUSR))==-1){
	return 0;
      }
      while((bytes_read=read(file,file_contents,512))>0){
	write(archivefd,file_contents,512);
	memset(file_contents,'\0',512);
      }
      close(file);
      return 0;
    }
    else{
  snprintf(pathname,PATH_MAX,"%s/", filename);
  if(vflag){
    printf("%s\n",pathname);
  }
  create_header(pathname, header,sflag);
    write(archivefd, header, 512);
    memset(header,'\0',512);
    }
  if((dir=opendir(filename))==NULL){
    perror("could not read file given");
    exit(EXIT_FAILURE);
  }
    while((entry=readdir(dir))!=NULL){
      //gets stat info
      //skips . and .. files
      if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0){
	continue;
      }
      snprintf(pathname, PATH_MAX,"%s/%s",filename,entry->d_name);
      if(lstat(pathname,&stats)==-1){
	perror("stat");
	continue;
      }

      //if pathname is a directory, archive that directories files recursively
      if(S_ISDIR(stats.st_mode)){
	create_archive(pathname, archivefd,vflag,sflag);
      }
    else{
      create_header(pathname,header,sflag);        
      write(archivefd,header,512); 
      memset(header,'\0',512);
      if(vflag){ 
	printf("%s\n",pathname);
      }
      size_t bytes_read;
      int file=open(pathname, O_RDONLY, S_IRUSR,S_IWUSR);

      while((bytes_read=read(file,file_contents,512))>0){
	  write(archivefd,file_contents,512);
	  memset(file_contents,'\0',512);
	}
	close(file);
    }
    }
    closedir(dir);

  return 0;
    }






int create_header(char* pathname, char* header,int sflag){
  struct stat stats;
  int i=0;
  memset(header,'\0',512);

  char name[100];
  memset(name,'\0',100);
  char prefix[155];
  memset(prefix, '\0', 155);

  if(strlen(pathname)>100){//handles if prefix needed
    i=strlen(pathname)-101;
    while(pathname[i]!='/'){
      if(i>=strlen(pathname)-1){
        perror("name does not fit in allocated space. Cannot partition");
        exit(EXIT_FAILURE);
      }
      i++;
    }
    i++;
    memcpy(prefix,pathname,i-1);
    memcpy(name, pathname+(i),strlen(pathname)-i);
  }
  else{
    memcpy(name, pathname,strlen(pathname));
  }
  memcpy(header,name,100);//writes name of the file

  lstat(pathname,&stats);

  //writes mode of the file
  char ocval[8];
  int_to_octal(ocval,sizeof(ocval),stats.st_mode);
  memset(ocval,'0',4);
  memcpy(header+100,ocval,8);


  char uid[8];
  char gid[8];
  //writes uid
  if (stats.st_uid <= 07777777) {
    /* if uid can fit into 7 octal digits,
     * create octal string and put it in the header */
    int_to_octal(uid,sizeof(uid),stats.st_uid);
  } else if (!sflag) {
    /* else if S is not ON, use insert_special_int */
    insert_special_int(uid, sizeof(uid), stats.st_uid);
  } else {
    /* otherwise, throw error */
    perror("UID does not fit in header: turn off S option\n");
    exit(EXIT_FAILURE);
  }
  memcpy(header+108,uid,8);

  //writes gid
  if (stats.st_gid <= 07777777) {
    int_to_octal(gid, sizeof(gid), stats.st_gid);
  } else if (!sflag) {
    insert_special_int(gid, sizeof(gid), stats.st_gid);
  } else {
    perror("GID does not fit in header: turn off S option\n");
    exit(EXIT_FAILURE);
  }
  memcpy(header+116,gid,8);

  //writes size
  char file_size[12];
  if(S_ISDIR(stats.st_mode)){
    int_to_octal(file_size,sizeof(file_size),0);
  }
  else{
    int_to_octal(file_size,sizeof(file_size),stats.st_size);
  }
  memcpy(header+124,file_size,12);

  //writes mtime
  char mtime[12];
  int_to_octal(mtime,sizeof(mtime),(unsigned int)stats.st_mtime);
  memcpy(header+136,mtime,12);
  
  //writes chcksum
  char chksum[8];
  for(i=0;i<8;i++){
    chksum[i]=' ';
  }
  memcpy(header+148,chksum,8);
  
  //writes typeflag
  char type[1];
  if(S_ISREG(stats.st_mode)){
    type[0]='0';
  }
  else if(S_ISDIR(stats.st_mode)){
    type[0]='5';
  }
  else{
    type[0]='2';
  }
  memcpy(header+156,type,1);
  

  //writes linkname
  char linkname[100];
  memset(linkname,'\0',100);
  char buffer[PATH_MAX];
  if(type[0]=='2'){
    if(readlink(pathname,buffer,PATH_MAX)==-1){
      perror("pathname incorrect for linkname");
      exit(EXIT_FAILURE);
    }
    if(strlen(buffer)>100){
      perror("linkname too long");
      exit(EXIT_FAILURE);
    }
    memcpy(linkname,buffer,strlen(buffer));
  }
  memcpy(header+157,linkname,100);


  //writes magic number
  char magic[6]="ustar";
  memcpy(header+257,magic,6);

  //writes version 
  char version[2];
  version[0]='0';
  version[1]='0';
  memcpy(header+263,version,2);

  struct passwd *pw = getpwuid(stats.st_uid);
  struct group  *gr = getgrgid(stats.st_gid);

  //writes uname
  char uname[32];
  memset(uname, '\0',32);
  memcpy(uname,pw->pw_name,strlen(pw->pw_name));
  memcpy(header+265,uname,32);

  //writes gname
  char gname[32];
  memset(gname, '\0', 32);
  memcpy(gname,gr->gr_name,strlen(gr->gr_name));
  memcpy(header+297,gname,32);

  //writes devmajor and devminor
  char devmajor[8];
  memset(devmajor,'\0',8);
  char devminor[8];
  memset(devminor,'\0',8);
  unsigned int minor;
  unsigned int major;
  if(S_ISCHR(stats.st_mode)||S_ISBLK(stats.st_mode)){
    major=major(stats.st_mode);
    minor=minor(stats.st_mode);
    int_to_octal(devmajor,sizeof(devmajor),major);
    int_to_octal(devminor,sizeof(devminor),minor);
  }
  memcpy(header+329,devmajor,8);
  memcpy(header+337,devminor,8);

  //prints prefix
  memcpy(header+345,prefix, 155);

  //populates checksum with correct value
  unsigned int counter=0;
  for(i=0;i<512;i++){
    counter+=(unsigned char)header[i];
  }
  int_to_octal(chksum,8,counter);
  memcpy(header+148,chksum,8);
  return 0;

}

void int_to_octal(char* string, int size, unsigned long value){
  snprintf(string,size,"%0*lo", size-1,value);
  return;
}


int insert_special_int(char *where, size_t size, int32_t val) {
  /* For interoperability with GNU tar. GNU seems to
   * set the high–order bit of the first byte, then
   * treat the rest of the field as a binary integer
   * in network byte order.
   * Insert the given integer into the given field
   * using this technique. Returns 0 on success, nonzero
   * otherwise
   */
  int err=0;
  if ( val < 0 || ( size < sizeof(val)) ) {
    /* if it’s negative, bit 31 is set and we can’t use the flag
     * if len is too small, we can’t write it. Either way, we’re
     * done.
     */
    err++;
  } else {
      memset(where, 0, size); //clear out the buffer 
      *(int32_t *)(where+size-sizeof(val)) = htonl(val); // place the int
      *where |= 0x80; // set that high–order bit 
  }
  return err;
}
