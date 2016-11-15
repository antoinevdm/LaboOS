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
#include <errno.h>
#define BUF_SIZE 1024

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

extern int errno ;
int c = -1;
char line[1024];

struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {"dir", required_argument, NULL, 'd'},
    {"prompt", required_argument, NULL, 'i'},
    {0,0,0,0}
};

int main(int argc, char *argv[]) {

  int i = 0;
  int d = 0;
  int h = 0;
  int file;

  while ((c = getopt_long (argc, argv, "hdi", longopts, NULL)) != -1) {
    switch(c){
      case 'h':
        h = 1;
        break;

      case 'd':
        d = 1;
        break;

      case 'i':
        i = 1;
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

   argc -= optind;
   if (h == 1){
     printf("\nUse this function to delete a file, simply secify wich file\n\n");
     printf("-d or --dir to delete an empty directory\n");
     printf("-i or --prompt if you want to valide it before deleting\n");
     exit(EXIT_SUCCESS);
   }

   else if (argc == 0){
     printf("You have to specify wich file you want to delete\n");
     printf("Use rm -h or --help for more informations \n");
     exit(EXIT_FAILURE);
   }

   else if (i == 1){
     printf("Are you sure you want to delete %s, y or n ?\n", argv[2]);
     if (!fgets(line,sizeof(line)-1,stdin)) {
       /* ^D ou fin de fichier => on quittte */
       printf("\n");
       exit(0);
     }

     if (strncmp(line, "y", 1) == 0){
       int del = unlink(argv[2]);

       if (del == -1)
          handle_error("unlink");

       if (del == 0){
          printf("The file %s was deleted from th file system\n", argv[2]);

       exit(EXIT_SUCCESS);
       }

       else if (strncmp(line, "n", 1) == 0){
          printf("You did not erased %s\n", argv[2]);
          exit(EXIT_SUCCESS);
       }

       else {
         printf("You chose an inappropriated character\n");
         exit(EXIT_FAILURE);
       }

     }
   }

   else if (d == 1){
     int deld = rmdir(argv[2]);

     if (errno == ENOTEMPTY){
       printf("The directory is not empty\n");
       exit(EXIT_FAILURE);
     }

     else if (errno == ENOTDIR){
       printf("This is not a directory\n");
       exit(EXIT_FAILURE);
     }

     else if (deld == -1)
        handle_error("rmdir");

    else if (deld == 0)
        printf("The directory %s was deleted from th file system\n", argv[2]);
   }

   else {
     int del = unlink(argv[1]);

     if (del == -1)
        handle_error("unlink");

     if (del == 0)
        printf("The file %s was deleted from th file system\n", argv[1]);
   }
   exit(EXIT_SUCCESS);
 }
