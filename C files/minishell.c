#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

char line[4096];

//Use to show the shell
void show_invite() {
  printf("> ");
  fflush(stdout);
}

//read the line you're typing
void read_line() {
  if (!fgets(line, sizeof(line)-1, stdin)) {
    printf("\n");
    exit(0);
  }
}

//wait the end of the son process
void wait_son(pid_t pid) {
  //loop to check fatal error
  while (1) {
    int status;
    int r = waitpid(pid, &status, 0);
    if (r < 0) {
      if (errno==EINTR)
        continue;
      printf("erreur de waitpid (%s)\n", strerror(errno));
      break;
    }
    if (WIFEXITED(status))
      printf("terminaison normale, status %i\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))
      printf("terminaison par signal %i\n", WTERMSIG(status));
    break;
  }
}

//exe the line you've typed
void execute() {
  pid_t pid;

  //del the final /n
  if (strchr(line, '\n'))
    *strchr(line, '\n') = 0;

  //don't use the empty lines
  if (!strcmp(line, ""))
    return;

  //write quit in the shell to exit it
  if(!strcmp(line, "quit"))
    exit(1);

  // create the son
  pid = fork();

  //error management, fork failed if pid = -1
  if (pid < 0) {
    printf("fork a échoué (%s)\n", strerror(errno));
    return;
  }

  //if pid == 0, i'm the son
  if (pid == 0) {
    //parsing of the line
    char *a = strtok (line, " ");
    char *b = strtok (NULL, " ");
    char *c = strtok (NULL, " ");
    char *d = strtok (NULL," ");

    char path[50];
    char file[50];

    //adding / .out or .out for pathname or file name
    sprintf(path, "./%s.out", a);
    sprintf(file, "%s.out", a);

    //launch the son in funciton of the line you typed
    execlp(path, file, b, c, d, NULL);

    //if failed to launch the son, print error and exit
    printf("impossible d'éxecuter \"%s\" (%s)\n", line,strerror(errno));
    exit(1);
  }

  else
    wait_son(pid);
}

int main(int argc, char *argv[]) {

  //batch mode
  if (argc != 1) {
    char test[50];
    sprintf(test, "./%s", argv[1]);

    execlp(test, argv[1], NULL);
    return 0;
  }

  //interactive mode
  while (1) {
    show_invite();
    read_line();
    execute();
  }

  return 0;
}
