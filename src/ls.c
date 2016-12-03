#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <time.h>
#include <getopt.h>
#include <ctype.h>

//Initialisation of the var, struc, ..
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

void checkFolderContent();
void openfile();

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
    {"all", no_argument, NULL, 'a'},
    {"recursive", no_argument, NULL, 'r'},
    {"withoutfolder", no_argument, NULL, 'W'},
    {"long", no_argument, NULL, 'l'},
    {0,0,0,0}
};

int main(int argc, char *argv[]) {
  int h = 0;
  int a = 0;
  int r = 0;
  int l = 0;
  int w = 0;

  //Getting the option, and put the right flags
  while ((c = getopt_long(argc, argv, "aRhlW", longopts, NULL)) != -1) {
    switch(c) {
      case 'h':
        h = 1;
        break;

      case 'a':
        a = 1;
        break;

      case 'R':
        r = 1;
        break;

      case 'l':
        l = 1;
        break;

      case 'W':
          w = 1;
          break;

      //Show error message if the option is not define in our getop
      case '?':
        if (isprint(optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character! `\\x%x'.\n", optopt);

        return 1;

      default:
        return 1;
    }
  }

  char *mode = argv[optind];

  //Get the file descriptor of the directory you want ..
  if ( mode != NULL) {
    char path[50];
    sprintf(path, "%s", argv[optind]);
    fd = open(path , O_RDONLY | O_DIRECTORY);
    // if ( fd == -1) {
    //   printf("%s\n", argv[optind]);
    //   exit(EXIT_SUCCESS);
    // }
  }

  //.. or the current directory
  else
    fd = open(".", O_RDONLY | O_DIRECTORY);

  if (fd == -1)
    handle_error("open");

  //Main code for showing ls info
  while (1) {
    //getdents is use to get inforamtion about files inside a directory
    nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);

    if (nread == -1)
        handle_error("getdents");

    if (nread == 0)
        break;

    //h flag, print the manual and exit
    if (h == 1) {
      printf("\nUse this function to list your files\n \n");
      printf("-h or --help for help, but you're already doing it\n");
      printf("-a or --all to list even the hided files\n");
      printf("-R or --recursive to list files and files inside a directory\n");
      printf("-l or --long if you want informations about the files you're listing\n");
      printf("-W or --withoutfolder if you don't want to list the folders inside your repository\n");
      exit(EXIT_SUCCESS);
    }

    //for each file in the directory
    for (bpos = 0; bpos < nread; ) {
      d = (struct linux_dirent *)(buf + bpos);
      d_type = *(buf + bpos + d->d_reclen - 1);

      //print the name, but not if the name starts by '.' (hid file)
      if ((*d->d_name != '.') & (l != 1) & (w != 1) & (a != 1)) {
          printf("%s\n", d->d_name);
      }

      //a flag, print every name
      if ((a == 1) & (w != 1)) {
          printf("%s\n", d->d_name);
      }

      //l falg, show information about ...
      if ((*d->d_name != '.') & (l == 1) & (w != 1)) {
        char info [] = {'-','-','-','-','\0'};

        //r, w, or x possibility
        if (d_type == DT_DIR)
          info[0] = 'd';
        if (access(d->d_name, R_OK) == 0)
          info[1] = 'r';
        if (access(d->d_name, W_OK) == 0)
          info[2] = 'w';
        if (access(d->d_name, X_OK) == 0)
          info[3] = 'x';

        printf ("%5s", info);
        printf("%16s",d->d_name);

        if (stat(d->d_name, &sb) == -1) {
          perror("stat");
          exit(EXIT_FAILURE);
        }

        //and user id, last edit, ...
        printf("  Owner: UID=%ld  GID=%ld ", (long) sb.st_uid, (long) sb.st_gid);
        printf("Last edit:   %s", ctime(&sb.st_mtime));
      }

      //w flag, don't print the name if the file is a folder
      if ((*d->d_name != '.') & (w == 1) & (l != 1) & (d_type != DT_DIR))
        printf("%s\n", d->d_name);

      //r flag, annd the file is a folder, tcheck what's inside
      if ((d_type == DT_DIR) & (*d->d_name != '.') & (r == 1) & (w != 1))
        checkFolderContent();

      bpos += d->d_reclen;
    }

  }

  close(fd);
  exit(EXIT_SUCCESS);
}

//Used to print the folder name (on r flag) ...
void checkFolderContent() {
    if ((d_type == DT_DIR) & (*d->d_name != '.'))
        printf("--Inside that folder :\n");
        openfile();
}

//... and print each file's name on the that folder
void openfile(int sfd, int oread,int sbpos) {
    sfd = open(d->d_name, O_RDONLY | O_DIRECTORY);
    oread = syscall(SYS_getdents, sfd, sbuf, BUF_SIZE);

    for (sbpos = 0; sbpos < oread; ) {
        sd = (struct linux_dirent *)(sbuf + sbpos);
        d_type = *(buf + sbpos + sd->d_reclen - 1);

        if (*sd->d_name != '.')
            printf("      -%s\n",sd->d_name);

        sbpos += sd->d_reclen;
    }
    close(sfd);

    //simply use for parsing datas
    printf("----------\n");
}
