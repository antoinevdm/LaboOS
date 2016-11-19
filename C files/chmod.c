#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

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
    {"changes", required_argument, NULL, 'c'},
    {0,0,0,0}
};



int main(int argc, char **argv) {
    int c;
    int h;
    int v;

    while ((v = getopt_long (argc, argv, "hc", longopts, NULL)) != -1) {
        switch(v){
          case 'h':
            h = 1;
            break;

          case 'c':
            c = 1;
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
      printf("\nUse this fonction to change permission on a file\n\n");
      printf(">First, put the permission you want\n");
      printf("      Start with 0 then 3 digits\n");
      printf("      The firs is for User, the second for Groupe and the third for All Users\n");
      printf("          For Read is 4, Write is 2 and execute is 1\n");
      printf("          And you can add numbers as you like -> Ex :\n");
      printf("              Read and Write : 4 + 2 = 6 \n\n");
      printf(">After that you have to select the file\n\n");
      printf(">And then the option\n");
      printf("      use -c or --changes to output a diagnostic when change is made\n\n");
      exit(EXIT_SUCCESS);
    }


    char *mode = argv[optind];
    char *buf = argv[optind+1];
    int i;
    int one;
    one = (mode[1] - '0');
    int two;
    two = (mode[2] -'0');
    int three;
    three = (mode[3] - '0');

    if (strlen(mode) != 4 | (mode[0] - '0') != 0 | one > 7 |two > 7 | three > 7) {
        printf("Error in your permission value\n");
        printf("use -h or --help for help\n");
        exit(EXIT_FAILURE);
    }

    if (buf == NULL) {
        printf("Missing pathname\n");
        printf("use -h or --help for help\n");
        exit(EXIT_FAILURE);
    }

    i = strtol(mode, 0, 8);
    if (chmod (buf,i) < 0) {
        fprintf(stderr, "%s: error in chmod(%s, %s) - %d (%s)\n",
                argv[0], buf, mode, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (c == 1) {
        printf("The files %s was change with a %s authorization\n", argv[3], argv[2]);
    }

    return(0);
}
