#include "header.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char* argv[]){
  int i=0;
  int opt;
  int vflag=0;
  int tflag=0;
  int xflag=0;
  int cflag=0;
  int sflag=0;
  int fflag=0;
  char *archive_file;

  while(argv[1][i]!='\0'){
      switch(argv[1][i]){
      case 'c':
	cflag=1;
	break;
      case 'v':
	vflag=1;
	break;
      case 'x':
	xflag=1;
	break;
      case 't':
	tflag=1;
	break;
      case 's':
	sflag=1;
	break;
      case 'f':
	fflag=1;
	break;
      default:
	perror("invalid type\n");
	exit(EXIT_FAILURE);
      }
      i++;
}
  

  while((opt=getopt(argc,argv, "cvxtsf:"))!=-1){
    }

  if(!(cflag||tflag||xflag)){
    fprintf(stderr, "please choose c,t,or x\n");
    exit(EXIT_FAILURE);
      }
  if(!fflag){
    fprintf(stderr, "please include f\n");
    exit(EXIT_FAILURE);
  }
  else if(argv[2]==NULL){
    perror("archive file name not inlcuded");
    exit(EXIT_FAILURE);
  }
  else{
    archive_file=strdup(argv[2]);
  }

  int archive_fd=0;
  
  /*  if(!access(archive_file,F_OK)){
    perror("bad tarfile");
    exit(EXIT_FAILURE);
    }*/

  if(cflag){
    struct stat stats;
    if(stat(archive_file,&stats)==0){//file already exists
      if((archive_fd=open(archive_file, O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR))==-1){
	perror("could not open archive file");
	exit(EXIT_FAILURE);
      }
      for(i=3;i<argc;i++){
	create_archive(argv[i],archive_fd,vflag,sflag);
      }
    }
    else{//file does not already exist
      if((archive_fd=open(archive_file, O_RDWR|O_CREAT,S_IRUSR|S_IWUSR))==-1){
        perror("could not open archive file");
        exit(EXIT_FAILURE);
      }
      for(i=3;i<argc;i++){
	create_archive(argv[i],archive_fd,vflag,sflag);}
    }
    char null_blocks[512];
    memset(null_blocks,'\0',512);
    write(archive_fd,null_blocks,512);
    write(archive_fd,null_blocks,512);
    close(archive_fd);  
  }


   if(xflag){
    if(argc==3){
      extract_all(archive_file,argc,argv);
    }
   else if(argc>3){
      extract_specific(archive_file,argc,argv);
    }
    else{
      perror("invalid number of arguments for extract");
      exit(EXIT_FAILURE);
    }
    }


  if(tflag){
    if(!access(archive_file,F_OK)){
      perror("bad tarfile");
    }
    if(argc<=3){
    print_all_files(argv[2],argc, argv);
    }
    else{
      print_specific(argv[2],argc,argv);
    }
  }

  free(archive_file);
  return 0;
}
