#include <stdio.h>
#include "header.h"
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#define MAX_PATH 256

//checks to see if the prefix 
int checkPath(char *prefix, char *check)
 {
 
   if(strncmp(check, prefix, strlen(prefix))==0)
    {
      return 0;

    } 
   else
   {
     return 1;

   }

 }

void getPath(char *buffer,char *path,
  char *name, char*prefix)
{
       if(*prefix)
        {
          snprintf(path, MAX_PATH, "%s/%s", prefix,name);

        }
        else if(*name)
        {
           snprintf(path, PATH_MAX, "%s", name);
        }
         
         sprintf(path, "%s%c",path, '\0');
         //printf("%s\n", name);
}


void getPermission(char *permissions,header myHeader)
{
 // converts mode string into int
 int mode = strtol(myHeader.mode, NULL,modeLen);
//checking the type 
 switch(myHeader.typeflag[0])
  {
    case '2':
     permissions[0] = 'l';
     break;
  
    case '5':
     permissions[0] = 'd';
     break;

    default:
     permissions[0] = '-';
     break;
  }

  permissions[1] = (mode & S_IRUSR) ? 'r': '-';
  permissions[2] = (mode & S_IWUSR) ? 'w': '-';
  permissions[3] = (mode & S_IXUSR) ? 'x': '-';
  permissions[4] = (mode & S_IRGRP) ? 'r': '-';
  permissions[5] = (mode & S_IWGRP) ? 'w': '-';
  permissions[6] = (mode & S_IXGRP) ? 'x': '-';
  permissions[7] = (mode & S_IROTH) ? 'r': '-';
  permissions[8] = (mode & S_IWOTH) ? 'w': '-';
  permissions[9] = (mode & S_IXOTH) ? 'x': '-';

  permissions[10] = '\0'; //null character check tho

}
 
void printHeader(header myHeader, char *path)
{
  
  char permissions[permissionWidth+1];
  char mtime[mtimeWidth+1];
  char filesize[sizeWidth+1];
  time_t t;
  char ownerGroup[unameLen+gnameLen+1];
  getPermission(permissions, myHeader);
  //gets the file size 
  snprintf(filesize, sizeWidth+1, "%lu", 
         strtol(myHeader.size, NULL, sizeWidth));
  
  //gets the time format
  t = (time_t)strtol(myHeader.mtime, NULL, 8);
  strftime(mtime, sizeof(mtime), "%Y-%m-%d %H:%M", localtime(&t));
  
  //formats the owner and group 
  snprintf(ownerGroup, unameLen+gnameLen, "%s/%s", myHeader.uname,
     myHeader.gname);
 //sets the format for the string 
  printf("%s %s %14s %s %s\n", permissions, ownerGroup, filesize, mtime, path);
   
  
  //permissions should have the rwx form
  // owner group should be good besdies extra spacing
    
}

 
void print_specific(char *archive, int argc, char *argv[])
{
 int i;
// char givenPath[MAX_PATH];
 char *givenPath;
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
 off_t offset;
  //checks every command when argc>3
  for(i=3; i<argc; i++)
   { 
     givenPath = (char*)malloc(MAX_PATH*sizeof(char));
     memset(buffer,0,sizeof(buffer));
     strncpy(givenPath,argv[i],strlen(argv[i]));
      
     if((archFD = open(archive, O_RDONLY|S_IRUSR))==-1)
    {
      perror("Can not open Archive File");
      exit(EXIT_FAILURE);

    }
  //reads 512 block size every iteration 
     while((reader = read(archFD, buffer,blockSize))>0)
   {
     path = (char*)malloc(MAX_PATH*sizeof(char)); 
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
     //memcpy(myHeader.name, name, nameLen);
    // memcpy(myHeader.prefix,prefix, prefixLen);
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
     //skips over file content  
     if(myHeader.typeflag[0] =='0' || myHeader.typeflag[0] == '\0')
         {
           size = strtol(myHeader.size, NULL, 8);
           offset = lseek(archFD, (size + (blockSize - 1)) /
                                blockSize * blockSize, SEEK_CUR);


           if(offset ==-1)
            {
              perror("error getting file offset");
              exit(EXIT_FAILURE);
            }


         }
      //checks if the path contains it 
     if(strcmp(givenPath, path)==0 ||
          (checkPath(givenPath,path)==0&&
           givenPath[strlen(givenPath)-1]=='/'))
     {
     char arg = argv[1][1];
     switch(arg)
      {
        case 'v':
        printHeader(myHeader, path);
         break;

         default:
         printf("%s\n",path);
            
          break;
            }  
    }
  free(path);
   }
   free(givenPath);
  close(archFD);
 }



}



void print_all_files(char *archive, int argc, char *argv[])
{
int archFD;
char buffer[blockSize];
int reader;
char prefix[prefixLen]={'\0'};//takes account of null character
char name[nameLen]={'\0'}; //takes account of null character
//size_t preIndex;
//size_t nameIndex;
//char *bufferPre;
//char *bufferName;
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
off_t offset;



//opens the archive file for read only 
if((archFD = open(archive, O_RDONLY|S_IRUSR))==-1)
{
  perror("Can not open Archive File");
  exit(EXIT_FAILURE); 

}
 //reads all of the header 
 //if(argc <= 3)
  // {   
  while((reader = read(archFD, buffer,blockSize))>0)
   { 
    
     path = malloc(MAX_PATH*sizeof(char));
     if(path != NULL)
      {
        memset(path, '\0', MAX_PATH);
      }
     memset(prefix, 0, sizeof(prefix));
     memset(name, 0, sizeof(name));
     //should store path into path
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
         
       //setValues(buffer,myHeader);
        // skips file contents  
        if(myHeader.typeflag[0] =='0' || myHeader.typeflag[0] == '\0')
         { 
           size = strtol(myHeader.size, NULL, 8);
           offset = lseek(archFD, (size + (blockSize - 1)) / 
                                blockSize * blockSize, SEEK_CUR);           


           if(offset ==-1)
            {
              perror("error getting file offset");
              exit(EXIT_FAILURE);
            }  
           
         
         }
    
        //checks verbose and arguments 
        char arg = argv[1][1];
         switch(arg)
         {
           case 'v':
            printHeader(myHeader, path);
             break;

           default:
             printf("%s\n",path);
           
             break;

         }
         
        
 
   free(path);
  }
  close(archFD); 
 
} 
