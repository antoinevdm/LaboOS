#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define _GNU_SOURCE
#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <getopt.h>

struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {0,0,0,0}
};


int main(int argc, char **argv) {
    int c;
    int h;
    int a;

    while ((c = getopt_long (argc, argv, "ha", longopts, NULL)) != -1) {
        switch(c){
          case 'h':
            h = 1;
            break;

          case 'a':
            a = 1;
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

    if (h == 1) {
      printf("  \nUse this fonction to change permission on a file\n\n");
      printf("First, put the option you want\n");
      printf("and select the file\n\n");
      printf("use -a or --all for rwx to every body\n");
      printf("use - or -- for ");
      exit(EXIT_SUCCESS);
    }

    if (a == 1 ) {
        char mode[] = "0777";
        char buf[100] = "test.txt";
        int i;

        i = strtol(mode, 0, 8);

        if (chmod (buf,i) < 0) {
            fprintf(stderr, "%s: error in chmod(%s, %s) - %d (%s)\n",
                    argv[0], buf, mode, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

    }


    return(0);
}
