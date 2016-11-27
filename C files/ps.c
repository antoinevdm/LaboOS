#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <getopt.h>
#define BUF_SIZE 1024

//Initialisation of the var, struc, ..
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int c = -1;
int sfd, ssfd, sssfd, fd, fc, nread;
char buf[BUF_SIZE];
struct linux_dirent *d;
struct linux_dirent *sd;
int bpos;
char d_type;
char errno;
int isEnd;

struct stat sb;

struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
};

//possibility to use -x or --xxx for the same flag in getop
struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {"pid", required_argument, NULL, 'p'},
    {"comm", no_argument, NULL, 'c'},
    {"info", required_argument, NULL, 'i'},
    {0,0,0,0}
};

int main(int argc, char *argv[]) {
  int h = 0;
  int p = 0;
  int co = 0;
  int i = 0;
  int file;
  char *buff = argv[optind+1];

  //Getting the option, and put the right flags
  while ((c = getopt_long (argc, argv, "icph", longopts, NULL)) != -1) {
    switch(c){

      case 'h':
        h = 1;
        break;

      case 'p':
        p = 1;
        break;

      case 'c':
        co = 1;
        break;

      case 'i':
        i = 1;
        break;

      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character! `\\x%x'.\n", optopt);

        return 1;

      default:
        return 1;
    }
  }

  //if flag p or i, and no pid specify, shwo message and exit process
  if ((p == 1 | i == 1) && buff == NULL) {
      printf("Missing PID\n");
      printf("use -h or --help for help\n");
      exit(EXIT_FAILURE);
  }

  //h flag, print the manual and exit
  if ( h == 1) {
        printf("\nUse thhis fonction to list all of your current process\n");
        printf("-h or --help to show this help page\n");
        printf("-p or --pid to search a specific process by his PID\n");
        printf("        And don't forget to put the PID you want\n");
        printf("-c or --comm for change the put the path in command name\n");
        printf("-i or --info to display all informations about a specific process\n");
        printf("        And don't forget to put de the PID you want\n");
        exit(EXIT_SUCCESS);
  }

  //Get the file descriptor of the /proc directory
  fd = open("/proc", O_RDONLY | O_DIRECTORY);

  if (fd == -1)
    handle_error("open");

  while (1) {
    //getdents is use to get inforamtion about files inside a directory
    nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);

    if (nread == -1)
        handle_error("getdents");

    if (nread == 0)
        break;

    else {
      printf("PID        COMMAND\n");

      //for each file in the directory
      for (bpos = 0; bpos < nread;) {
        d = (struct linux_dirent *)(buf + bpos);
        d_type = *(buf + bpos + d->d_reclen - 1);

        char a;
        a = *d->d_name;

        //if the name is not a number (pid), do nothing
        if (a == '1' | a == '2' | a == '3' | a == '4' | a == '5' |  \
            a == '6' | a == '7' | a == '8' | a == '9' ) {

            //get the file descriptor of the file in question
            sfd = openat(fd, d->d_name, O_RDONLY | O_DIRECTORY);

            //co flag, open the cmdline file
            if (co == 1)
                ssfd = openat(sfd, "cmdline", O_RDONLY);
            //or open the comm file
            else
                ssfd = openat(sfd, "comm", O_RDONLY);

            //i flag, open the status file too
            if (i == 1)
                sssfd = openat(sfd, "status", O_RDONLY);

            //error management for files descriptors
            if (sfd == -1)
                handle_error("open");

            if (ssfd == -1)
                handle_error("open");

            if (ssfd == -1)
                handle_error("open");

            //p flag, print only the name of the pid you specify
            if (p == 1) {
                if (strcmp(d->d_name, argv[2]) == 0) {
                    isEnd = 1;
                    while((read(ssfd, buf, BUF_SIZE)) > 0)
                        printf("%3s%16s", d->d_name, buf);
                }
            }

            //i flag, print only the name of the pid you specify,
            //and all information about that process too
            else if (i == 1) {
                if (strcmp(d->d_name, argv[2]) == 0) {
                    isEnd = 1;
                    while((read(ssfd, buf, BUF_SIZE)) > 0)
                        printf("%3s%16s\n", d->d_name, buf);

                    printf("Informations :\n\n");

                    while((read(sssfd, buf, BUF_SIZE)) > 0)
                        printf("%s\n", buf);
                }
            }

            //or simply print ...
            else {
                while((read(ssfd, buf, BUF_SIZE)) > 0) {
                    //co flag, the name (from cmdline file)
                    if (co == 1)
                        printf("%3s%16s\n", d->d_name, buf);
                    //no flag, the name (from comm file)
                    else
                        printf("%3s%16s", d->d_name, buf);
                }
            }
        }

        bpos += d->d_reclen;
      }

      //if the for is ended, and nothing was printed,
      //the pid you specyfy is simply not a process
      if (isEnd != 1 && (p == 1 | i == 1)) {
          printf("Le PID %s ne correspond pas à un process\n", argv[2]);
          exit(EXIT_FAILURE);
      }

    }
  }
  close(fd);
  close(sfd);
  close(ssfd);
  exit(EXIT_SUCCESS);
}
