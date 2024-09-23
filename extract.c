#include "header.h"
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
#include <errno.h>
#define MAX_PATH 256


void extract_all(char* archive, int argc, char *argv[]){
int archFD;
 int i;
char buffer[blockSize];
int reader;
char prefix[prefixLen];//takes account of null character 
 memset(prefix,'\0',prefixLen);
char name[nameLen]; //takes account of null character
 memset(prefix,'\0',prefixLen);
char *path;
char uidStr[uidLen];
char gidStr[gidLen];
char modeStr[modeLen];
char sizeStr[sizeLen];
char mtimeStr[mtimeLen];
char typeFlag[typeflagLen];
char uname[unameLen];
char gname[gnameLen];

struct header myHeader;
off_t size;



//opens the archive file for read only 
if((archFD = open(archive, O_RDONLY|S_IRUSR))==-1)
  {
    perror("Can not open Archive File");
    exit(EXIT_FAILURE);
  }

 while((reader = read(archFD, buffer,blockSize))>0)
   {

     path = (char*)calloc(1,256);
     if(path != NULL)
       {
	 memset(path, '\0', MAX_PATH);
       }
     memset(prefix, 0, sizeof(prefix));
     memset(name, 0, sizeof(name));
     strncpy(name, buffer+ OFF_name, nameLen);
     memcpy(myHeader.name, name, strlen(prefix)+1);

     strncpy(prefix, buffer + OFF_prefix, prefixLen);
     memcpy(myHeader.prefix, buffer + OFF_prefix, strlen(prefix)+1);
     //should store path into path 
     getPath(buffer, path, name, prefix);


     if(!*name && !*prefix)
       {
	 free(path);
	 continue;
       }

     //stores values into header 
     memcpy(myHeader.name, name, nameLen);
     memcpy(myHeader.prefix,prefix, prefixLen);
     strncpy(modeStr, buffer+ OFF_mode, modeLen);
     memcpy(myHeader.mode, modeStr, strlen(modeStr)+1);

     strncpy(uidStr, buffer + OFF_uid, uidLen);
     memcpy(myHeader.uid, buffer + OFF_uid, strlen(uidStr)+1);
 
     strncpy(gidStr, buffer +OFF_gid, gidLen);
     memcpy(myHeader.gid, gidStr, strlen(gidStr)+1);
        
     strncpy(sizeStr,buffer+ OFF_size, sizeLen);
     memcpy(myHeader.size, sizeStr, strlen(sizeStr)+1);

     strncpy(mtimeStr, buffer +OFF_mtime, mtimeLen);
     memcpy(myHeader.mtime, mtimeStr, strlen(mtimeStr)+1);

     strncpy(typeFlag, buffer + OFF_typeflag, typeflagLen);
     memcpy(myHeader.typeflag, typeFlag, strlen(typeFlag)+1);
        
     strncpy(uname, buffer+ OFF_uname, unameLen); 
     strncpy(gname, buffer+ OFF_gname, gnameLen);
       
     memcpy(myHeader.uname, uname, strlen(uname)+1);
     memcpy(myHeader.gname, gname, strlen(gname)+1); 
     // printf("%s", myHeader.uname);  
     //setValues(buffer,myHeader);
     //printf("%s", myHeader.typeflag);
     if(myHeader.typeflag[0]=='5'){
       int d;
       char* cwd=(char*)calloc(1,256);
       getcwd(cwd,256);
       size_t len = strlen(cwd);
       char * newcwd = (char *)calloc(1,len+1+strlen(path));
       memcpy(newcwd, cwd, len);

       i=strlen(path)-1;
       int counter=0;
       //splits the directory and filename in path
       while(path[i]!='/'&& i!=0){
	 i--;
	 counter++;
       }
       char* oldDir;
       char *filename;
       char *directory;
       if(i!=0){
	 filename=(char*)calloc(1,counter);
	 directory=(char*)calloc(1,strlen(path));
	 memcpy(directory, path,i);
	 i++;
	 memcpy(filename, path+i,counter);
	 oldDir=strdup(newcwd);
	 memcpy(newcwd+len,"/",1);
	 strcat(newcwd,directory);
	 strcat(newcwd,"/");
       }
       else{
	 counter++;
	 filename=(char*)calloc(1,counter);
	 memcpy(filename, path,counter);
	 directory=strdup(newcwd);
	 oldDir=strdup(newcwd);
       }
       if((d=mkdir(newcwd,0777))!=0){
	 continue;
       }
     }
     if(myHeader.typeflag[0] =='0' || myHeader.typeflag[0] == '\0')
       {
	 //saves current working directory
	 char* cwd=(char*)calloc(1,256);
	 getcwd(cwd,256);
	 size_t len = strlen(cwd);
	 char * newcwd = (char *)calloc(1,len+1+strlen(path));
	 memcpy(newcwd, cwd, len);

	 i=strlen(path)-1;
	 int counter=0;
	 //splits the directory and filename in path
	 while(path[i]!='/'&& i!=0){
	   i--;
	   counter++;
	 }
	 char* oldDir;
	 char *filename;
	 char *directory;
	 if(i!=0){
	   filename=(char*)calloc(1,counter);
	   directory=(char*)calloc(1,strlen(path));
	   memcpy(directory, path,i);
	   i++;
	   memcpy(filename, path+i,counter);
	   oldDir=strdup(newcwd);
	   memcpy(newcwd+len,"/",1);
	   strcat(newcwd,directory);
	   strcat(newcwd,"/");
	 }
	 else{
	   counter++;
	   filename=(char*)calloc(1,counter);
	   memcpy(filename, path,counter);
	   directory=strdup(newcwd);
	   oldDir=strdup(newcwd);
	 }
	 int d;
	 //changes to directory specified in path
	 if((d=mkdir(newcwd, 0777)) == 0) {
	   chdir(newcwd);
	 } else {
	   // Check if the error is due to directory already existing
	   if (errno == EEXIST) {
	     chdir(newcwd);
	   } else {
	     // Some other error occurred
	     printf("%d",errno);
	     perror("Error creating directory");
	     exit(EXIT_FAILURE);
	   }
	 }

	 //creates new file based on filename in path
	 int new_file;
	 int mode=strtol(myHeader.mode,NULL,modeLen); 
	 if(mode&S_IXUSR||mode&S_IXGRP||mode&S_IXOTH){
	   new_file=open(filename,O_RDWR|O_CREAT|O_TRUNC, 0777);
	 }
	 else{
	   new_file=open(filename,O_RDWR|O_CREAT|O_TRUNC, 0666);
	 }
	 //writes to new file the contents of the file specified in the archive
	 size = strtol(myHeader.size, NULL, 8);
	 char file_contents[size];
	 read(archFD,file_contents,size);
	 write(new_file,file_contents,size);
	 close(new_file);
	 lseek(archFD,512-(size%512),SEEK_CUR);//seeks past null characters
	 // memcpy(newcwd+len,"\0",1);
	 chdir(oldDir);
    
     //checks verbose and arguments 
       
     if(argv[1][1]=='v')
       {
	 printf("%s\n",path);
       }
     free(path);
       }}
   
 
} 



void extract_specific(char *archive, int argc, char *argv[]){
  int i;
  char givenPath[MAX_PATH];
  int archFD;
  char buffer[blockSize];
  int reader;
  char *path;
  char prefix[prefixLen];
  char name[nameLen];
  char uidStr[uidLen];
  char gidStr[gidLen];
  char modeStr[modeLen];
  char sizeStr[sizeLen];
  char mtimeStr[mtimeLen];
  char typeFlag[typeflagLen];
  char uname[unameLen];
  char gname[gnameLen];
  header myHeader;
  off_t size;
  
  for(i=3; i<argc; i++)
    {
      memset(buffer,0,sizeof(buffer));
      strncpy(givenPath,argv[i],strlen(argv[i])); 
      if((archFD = open(archive, O_RDONLY|S_IRUSR))==-1)
	{
	  perror("Can not open Archive File");
	  exit(EXIT_FAILURE);

	}   
      while((reader = read(archFD, buffer,blockSize))>0)
	{
	  path = calloc(sizeof(char),PATH_MAX); 
	  if(path != NULL)
	    {
	      memset(path, '\0', MAX_PATH);
	    }


	  memset(prefix, 0, sizeof(prefix));
	  memset(name, 0, sizeof(name));

	  strncpy(name, buffer+ OFF_name, nameLen);
	  memcpy(myHeader.name, name, strlen(prefix)+1);

	  strncpy(prefix, buffer + OFF_prefix, prefixLen);
	  memcpy(myHeader.prefix, buffer + OFF_prefix, strlen(prefix)+1);
     
	  getPath(buffer, path, name, prefix);
     
	  if(!*name && !*prefix)
	    {
	      free(path);
	      continue;
	    }
	  memcpy(myHeader.name, name, nameLen);
	  memcpy(myHeader.prefix,prefix, prefixLen);
	  strncpy(modeStr, buffer+ OFF_mode, modeLen);
	  memcpy(myHeader.mode, modeStr, strlen(modeStr)+1);

	  strncpy(uidStr, buffer + OFF_uid, uidLen);
	  memcpy(myHeader.uid, buffer + OFF_uid, strlen(uidStr)+1);

	  strncpy(gidStr, buffer +OFF_gid, gidLen);
	  memcpy(myHeader.gid, gidStr, strlen(gidStr)+1);

	  strncpy(sizeStr,buffer+ OFF_size, sizeLen);
	  memcpy(myHeader.size, sizeStr, strlen(sizeStr)+1);

	  strncpy(mtimeStr, buffer +OFF_mtime, mtimeLen);
	  memcpy(myHeader.mtime, mtimeStr, strlen(mtimeStr)+1);

	  strncpy(typeFlag, buffer + OFF_typeflag, typeflagLen);
	  memcpy(myHeader.typeflag, typeFlag, strlen(typeFlag)+1);

	  strncpy(uname, buffer+ OFF_uname, unameLen);
	  strncpy(gname, buffer+ OFF_gname, gnameLen);

	  memcpy(myHeader.uname, uname, strlen(uname)+1);
	  memcpy(myHeader.gname, gname, strlen(gname)+1);
	  //setValues(buffer,myHeader);
	  if(myHeader.typeflag[0]=='5'){
	    int d;
	    char* cwd=(char*)calloc(1,256);
	    getcwd(cwd,256);
	    size_t len = strlen(cwd);
	    char * newcwd = (char *)calloc(1,len+1+strlen(path));
	    memcpy(newcwd, cwd, len);

	    i=strlen(path)-1;
	    int counter=0;
	    //splits the directory and filename in path
	    while(path[i]!='/'&& i!=0){
	      i--;
	      counter++;
	    }
	    char* oldDir;
	    char *filename;
	    char *directory;
	    if(i!=0){
	      filename=(char*)calloc(1,counter);
	      directory=(char*)calloc(1,strlen(path));
	      memcpy(directory, path,i);
	      i++;
	      memcpy(filename, path+i,counter);
	      oldDir=strdup(newcwd);
	      memcpy(newcwd+len,"/",1);
	      strcat(newcwd,directory);
	      strcat(newcwd,"/");
	    }
	    else{
	      counter++;
	      filename=(char*)calloc(1,counter);
	      memcpy(filename, path,counter);
	      directory=strdup(newcwd);
	      oldDir=strdup(newcwd);
	    }
	    if((d=mkdir(newcwd,0777))!=0){
	      continue;
	      }
	  }
	  if(myHeader.typeflag[0] =='0' || myHeader.typeflag[0] == '\0')
	    {
	      //saves current working directory
	      char* cwd=(char*)calloc(1,256);
	      getcwd(cwd,256);
	      size_t len = strlen(cwd);
	      char * newcwd = (char *)calloc(1,len+1+strlen(path));
	      memcpy(newcwd, cwd, len);

	      i=strlen(path)-1;
	      int counter=0;
	      //splits the directory and filename in path
	      while(path[i]!='/'&& i!=0){
		i--;
		counter++;
	      }
	      char* oldDir;
	      char *filename;
	      char *directory;
	      if(i!=0){
		filename=(char*)calloc(1,counter);
		directory=(char*)calloc(1,strlen(path));
		memcpy(directory, path,i);
		i++;
		memcpy(filename, path+i,counter);
		oldDir=strdup(newcwd);
		memcpy(newcwd+len,"/",1);
		strcat(newcwd,directory);
		strcat(newcwd,"/");
	      }
	      else{
		counter++;
		filename=(char*)calloc(1,counter);
		memcpy(filename, path,counter);
		directory=strdup(newcwd);
		oldDir=strdup(newcwd);
	      }
	      int d;
	      //changes to directory specified in path
	      if((d=mkdir(newcwd, 0777)) == 0) {
		chdir(newcwd);
	      } else {
		// Check if the error is due to directory already existing
		if (errno == EEXIST) {
		  chdir(newcwd);
		} else {
		  // Some other error occurred
		  printf("%d",errno);
		  perror("Error creating directory");
		  exit(EXIT_FAILURE);
		}
	      }

	      //creates new file based on filename in path
	      int new_file;
	      int mode=strtol(myHeader.mode,NULL,modeLen); 
	      if(mode&S_IXUSR||mode&S_IXGRP||mode&S_IXOTH){
		new_file=open(filename,O_RDWR|O_CREAT|O_TRUNC, 0777);
	      }
	      else{
		new_file=open(filename,O_RDWR|O_CREAT|O_TRUNC, 0666);
	      }
    //writes to new file the contents of the file specified in the archive
	      size = strtol(myHeader.size, NULL, 8);
	      char file_contents[size];
	      read(archFD,file_contents,size);
	      write(new_file,file_contents,size);
	      close(new_file);
	      lseek(archFD,512-(size%512),SEEK_CUR);//seeks past null characters
	      // memcpy(newcwd+len,"\0",1);
	      chdir(oldDir);

	      if(argv[1][1]=='v'){
		  printf("%s\n",path);
		}  
	  free(path);
	    }
	}
  }
  close(archFD);
  return;}
