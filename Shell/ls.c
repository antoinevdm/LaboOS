#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <getopt.h>
#define BUF_SIZE 1024

int fd,fc,nread,bpos;
char buf[BUF_SIZE];

struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
                        /* length is actually (d_reclen - 2 -
                           offsetof(struct linux_dirent, d_name) */
    /*
    char           pad;       // Zero padding byte
    char           d_type;    // File type (only since Linux 2.6.4;
                              // offset is (d_reclen - 1))
    */
};

int main(int argc, char *argv[]){

  fd = open(".", O_RDONLY | O_DIRECTORY);

  if (fd == -1){
    printf("File descriptor error\n");
    exit(EXIT_SUCCESS);
  }


  while (1){

       nread = getdents(fd, buf, BUF_SIZE);

       if (nread == -1){
         printf("Getdents error\n");
         exit(EXIT_SUCCESS);
       }
       if (nread == 0){
         printf("End of the directory\n");
         break;
       }

      for (bpos = 0; bpos < nread;) {
        printf("boucle numer %d\n", bpos);
      }
  }
}
