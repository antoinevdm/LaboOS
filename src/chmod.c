#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/stat.h>

//possibility to use -x or --xxx for the same flag in getop
struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {"changes", no_argument, NULL, 'c'},
    {"verbose", no_argument, NULL, 'v'},
    {0,0,0,0}
};

int main(int argc, char **argv) {
    int c = 0;
    int h = 0;
    int w = 0;
    int v = 0;

    //Getting the option, and put the right flags
    while ((w = getopt_long(argc, argv, "vhc", longopts, NULL)) != -1) {
        switch(w){
          case 'h':
            h = 1;
            break;

          case 'c':
            c = 1;
            break;

          case 'v':
            v = 1;
            break;

          case '?':
            if (isprint(optopt))
              fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
              fprintf(stderr,"Unknown option character! `\\x%x'.\n", optopt);

            return 1;

          default:
            return 1;
        }
    }

    //h flag, print the manual and exit
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
      printf("      use -c or --changes to output a diagnostic when change is made\n");
      printf("      use -v or --verbose to output a diagnostic before doing the change\n\n");
      exit(EXIT_SUCCESS);
    }

    //init the char, int, ..
    char *mode = argv[optind];
    char *buf = argv[optind+1];
    int i;
    int one;
    one = (mode[1] - '0');
    int two;
    two = (mode[2] -'0');
    int three;
    three = (mode[3] - '0');

    //testing if value xxxx is right for define a permission value in chmod(2)
    if ((strlen(mode) != 4) | ((mode[0] - '0') != 0) | (one > 7) | (two > 7) | (three > 7)) {
        printf("Error in your permission value\n");
        printf("use -h or --help for help\n");
        exit(EXIT_FAILURE);
    }

    //if buf is empty, the user doesn't specify the pathname
    if (buf == NULL) {
        printf("Missing pathname\n");
        printf("use -h or --help for help\n");
        exit(EXIT_FAILURE);
    }

    //flag v, say what's you going to do ...
    if (v == 1)
        printf("You'r going to change the files %s with a %s authorization\n", argv[3], argv[2]);

    //Using the syscall chmod
    i = strtol(mode, 0, 8);
    if (chmod(buf,i) < 0) {
        fprintf(stderr, "%s: error in chmod(%s, %s) - %d (%s)\n",
                argv[0], buf, mode, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    //... and done when it's done
    if (v == 1)
        printf("Done\n");

    //flag c, say you what you did
    if (c == 1)
        printf("The files %s was change with a %s authorization\n", argv[3], argv[2]);

    return(0);
}
