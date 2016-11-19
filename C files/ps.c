#define _GNU_SOURCE
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

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int c = -1;
int fd,fc,nread;
char buf[BUF_SIZE];
char sbuf[BUF_SIZE];
struct linux_dirent *d;
struct linux_dirent *sd;
int bpos;
char d_type;
char errno;
int rval;

void checkFolderContent();
void openfile();

struct stat sb;

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

struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {0,0,0,0}
};


int main(int argc, char *argv[]) {
  int h = 0;
  int file;

  while ((c = getopt_long (argc, argv, "h", longopts, NULL)) != -1) {
    switch(c){
      case 'h':
        h = 1;
        break;

      case '?':
        if (isprint (optopt)) {
            fprintf (stderr, "Stooop! what are you doing? this is an \
              unknown option! `-%c'.\n", optopt);
        }

        else {
            fprintf (stderr,"Stoooop! what are you doing? this is an \
              unknown option character! `\\x%x'.\n", optopt);
        }

        return 1;

      default:
        return 1;
    }
  }

  fd = open("/proc", O_RDONLY | O_DIRECTORY);  //fd = "file descriptor"

  if (fd == -1)
    handle_error("open");

  while (1) {
    nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);

    if (nread == -1)
        handle_error("getdents");

    if (nread == 0)
        break;

    if (h == 1) {
      printf("\nUse this function to list your working process\n \n");
    }

    else {
      for (bpos = 0; bpos < nread;) {
        d = (struct linux_dirent *)(buf + bpos);
        d_type = *(buf + bpos + d->d_reclen - 1);

        char a;
        a = *d->d_name;

        if (a == '1' | a == '2' | a == '3' | a == '4' | a == '5' |  \
            a == '6' | a == '7' | a == '8' | a == '9' ) {
            printf("%s\n",d->d_name);
        }

        bpos += d->d_reclen;
      }
    }
  }
  close(fd);
  exit(EXIT_SUCCESS);
}
