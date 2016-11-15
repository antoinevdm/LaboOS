/* en-têtes standard */
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/wait.h>   /* pour avoir wait & co. */
#include <ctype.h>      /* pour avoir isspace & co. */
#include <string.h>
#include <errno.h>      /* pour avoir errno */

char ligne[4096];       /* contient la ligne d'entrée */


void affiche_invite()
{
  printf("> ");
  fflush(stdout);
}

void lit_ligne()
{
  if (!fgets(ligne,sizeof(ligne)-1,stdin)) {
    /* ^D ou fin de fichier => on quittte */
    printf("\n");
    exit(0);
  }
}

/* attent la fin du processus pid */
void attent(pid_t pid)
{
  /* il faut boucler car waitpid peut retourner avec une erreur non fatale */
  while (1) {
    int status;
    int r = waitpid(pid,&status,0); /* attente bloquante */
    if (r<0) {
      if (errno==EINTR) continue; /* interrompu => on recommence à attendre */
      printf("erreur de waitpid (%s)\n",strerror(errno));
      break;
    }
    if (WIFEXITED(status))
      printf("terminaison normale, status %i\n",WEXITSTATUS(status));
    if (WIFSIGNALED(status))
      printf("terminaison par signal %i\n",WTERMSIG(status));
    break;
  }
}

/* execute la ligne */
void execute()
{
  pid_t pid;

  /* supprime le \n final */
  if (strchr(ligne,'\n')) *strchr(ligne,'\n') = 0;

  /* saute les lignes vides */
  if (!strcmp(ligne,"")) return;

  pid = fork();
  if (pid < 0) {
    printf("fork a échoué (%s)\n",strerror(errno));
    return;
  }

  if (pid==0) {
    //fils
    char *a = strtok (ligne," ");
    char *b = strtok (NULL," ");
    char *c = strtok (NULL," ");

    char path[50];
    char file[50];


    sprintf(path, "./%s.out", a);
    sprintf(file, "%s.out", a);


    execlp(path, file, b, c, NULL);
    //execlp("/Users/Antoinevdm/Desktop/LaboOS/Shell/ls.out", "ls.out", NULL);
    //on n'arrive ici que si le exec a échoué
    printf("impossible d'éxecuter \"%s\" (%s)\n",ligne,strerror(errno));
    exit(1);
  }
  else {

    attent(pid);
  }
}

int main(int argc, char *argv[])
{
  if(argc != 1){
    //batch mode
    return 0;
  }
  /* boucle d'interaction */
  while (1) {
    affiche_invite();
    lit_ligne();
    execute();
  }
  return 0;
}
