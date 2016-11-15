/* en-têtes standard */
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/wait.h>   /* pour avoir wait & co. */
#include <ctype.h>      /* pour avoir isspace & co. */
#include <string.h>
#include <errno.h>      /* pour avoir errno */


int main(int argc, char *argv[])
{
  printf("%s\n", argv[1]);
  printf("Je me suis bien lancer\n");
  return(0);
}
