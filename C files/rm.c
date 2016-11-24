#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

//Initialisation of the var, struc, ..
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

extern int errno;
int c = -1;
char line[1024];

//possibility to use -x or --xxx for the same flag in getop
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

  //Getting the option, and put the right flags
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
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character! `\\x%x'.\n", optopt);

        return 1;

      default:
        return 1;
     }
   }

   argc -= optind;

   //h flag, print the manual and exit
   if (h == 1) {
     printf("\nUse this function to delete a file, simply secify wich file\n\n");
     printf("-d or --dir to delete an empty directory\n");
     printf("-i or --prompt if you want to valide it before deleting\n");
     exit(EXIT_SUCCESS);
   }

   //if no file specified, show error message
   else if (argc == 0) {
     printf("You have to specify wich file you want to delete\n");
     printf("Use rm -h or --help for more informations \n");
     exit(EXIT_FAILURE);
   }

   //i flag, ask you if you're sure to do it before doing it
   else if (i == 1) {
     printf("Are you sure you want to delete %s, y or n ?\n", argv[2]);
     if (!fgets(line,sizeof(line)-1,stdin)) {
       /* ^D ou fin de fichier => on quittte */
       printf("\n");
       exit(0);
     }

     //if your awnser is yes, delete the file
     if (strncmp(line, "y", 1) == 0) {
       int del = unlink(argv[2]);

       if (del == -1)
          handle_error("unlink");

       if (del == 0)
          printf("The file %s was deleted from th file system\n", argv[2]);

       exit(EXIT_SUCCESS);
     }

     //if it's no, don't do it and show a message
     else if (strncmp(line, "n", 1) == 0) {
          printf("You did not erased %s\n", argv[2]);
          exit(EXIT_SUCCESS);
     }

     //else, error message, inappropriated character
     else {
         printf("You chose an inappropriated character\n");
         exit(EXIT_FAILURE);
     }
   }

   //d flag, use to delete an empty directory
   else if (d == 1) {
     int deld = rmdir(argv[2]);

     //Error message
     if (errno == ENOTEMPTY) {
       printf("The directory is not empty\n");
       exit(EXIT_FAILURE);
     }

     else if (errno == ENOTDIR) {
       printf("This is not a directory\n");
       exit(EXIT_FAILURE);
     }

     else if (deld == -1)
        handle_error("rmdir");

     //sucess message
     else if (deld == 0)
        printf("The directory %s was deleted from th file system\n", argv[2]);
   }

   //no flag, simply unlink the file, show error or succes message
   else {
     int del = unlink(argv[1]);

     if (del == -1)
        handle_error("unlink");

     if (del == 0)
        printf("The file %s was deleted from th file system\n", argv[1]);
   }
   exit(EXIT_SUCCESS);
}
