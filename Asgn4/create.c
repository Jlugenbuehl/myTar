#include <stdio.h>
#include <stdib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include "header.h"

int create_archive(char* filename, char* archive,int vflag){
  struct stat stats;
  struct dirent *entry;
  DIR *dir;
  char file_contents[1024];
  dir=opendir(filename);
  if((int archivefd=open(archive,O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR))!=NULL){
    while(entry=readdir(dir)!=NULL){
      char pathname[PATH_MAX];//creates pathname
      snprintf(pathname,sizeof(pathname),"%s/%s",filename,entry->d_name);
      //gets stat info
      if(lstat(pathname,&stats)==-1){
	perror("stat");
	continue;
      }
      //skips . and .. files
      if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0){
	continue;
      }

      char* archive_entry=strdup(create_archive_entry(pathname));
      write(archivefd,archive_entry,strlen(archive_entry));

      if(vflag){
	printf("%s\n",pathname);
      }

      //if pathname is a directory, archive that directories files recursively
      if(S_ISDIR(stats.st_mode)){
	create_archive(pathname, archive);
	continue;
      }
      else{
      size_t bytes_read;
      int file=open(pathname, O_RDONLY, S_IRUSR,S_IWUSR);

      while((bytes_read=read(file,file_contents,512)>0){
	  write(archivefd,file_contents,bytes_read);
	}
	close(file);
	}}}
  else{
    return -1;}
      close(archivefd);
      closedir(dir);
       
  return 0;
    }

char* create_archive_entry(char* pathname){
  struct stat stats;
  if(char* ret=malloc(7+ 10+17+8+16+strlen(pathname))==NULL){
    perror("malloc in create_archive_entry");}

  lstat(pathname,&stats);

  //sees if it is directory, symlink, or file
  switch (stats.st_mode & S_IFMT) {
  case S_IFDIR:  
    strcat(ret,"d")
  case S_IFLNK:  
    strcat(ret,"l");
  case S_IFREG:  
    strcat(ret,"-");
  }

  strcat(ret, " ");

  //user permissions
  strcat(ret, (stats.st_mode & S_IRUSR) ? "r" : "-");
  strcat(ret, (stats.st_mode & S_IWUSR) ? "w" : "-");
  strcat(ret, (stats.st_mode & S_IXUSR) ? "x" : "-");

  // Group permissions
  strcat(ret, (stats.st_mode & S_IRGRP) ? "r" : "-");
  strcat(ret, (stats.st_mode & S_IWGRP) ? "w" : "-");
  strcat(ret, (stats.st_mode & S_IXGRP) ? "x" : "-");

  // Other permissions
  strcat(ret, (stats.st_mode & S_IROTH) ? "r" : "-");
  strcat(ret, (stats.st_mode & S_IWOTH) ? "w" : "-");
  strcat(ret, (stats.st_mode & S_IXOTH) ? "x" : "-");

  strcat(ret, " ");

  //adds owner/group
  struct passwd *pw = getpwuid(stats.st_uid);
  struct group  *gr = getgrgid(stats.st_gid);  
  int counter=0;
  counter+=strlen(pw->pw_name);
  counter+=strlen(gr->gr_name);
  counter++;
  strcat(ret, pw->pw_name);
  strcat(ret,"/");
  strcat(ret, gr->gr_name);

  //adds extra spaces if does not occupy all 17 bytes allocated
  while(counter!=18){
    strcat(ret, " ");
    counter++;
  }

  //adds size of file as 8 byte value
  strcat(ret, (uint8_t)stats.st_size);
  strcat(ret, " ");
  
  //adds the mtime
  char time[16];
  struct tm lt;
  time_t t=stats.st_mtime;
  localtime_r(&t,&lt);
  //converts time to string format
  strftime(mtime,16, "%Y-%m-%d %H:%M", &lt);
  strcat(ret, mtime);
  strcat(ret, " ");

  //adds pathname
  strcat(ret, pathname);

  return ret;

}
