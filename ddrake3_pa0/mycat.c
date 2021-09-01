/** Author: Derek Drake
  * Date: 02/02/2021
  * CSCE-451
  *
  * This is a program will:
  * 1. read in and parse the arguments
  * 2. for each argument (let’s assume no bad arguments)
        a. open the file
        b. read the contents of the file
        c. print contents of file to terminal
        d. close file
  *
  */

  #include <stdio.h>
  #include <stdlib.h>

int main (int argc, char *argv[]){

  for(int i = 1; i < argc; i++) {
    FILE *fptr;

    fptr = fopen(argv[i], "r");

    char c;

    c = fgetc(fptr);
    while(c != EOF){
      printf("%c", c);
      c = fgetc(fptr);
    }

    fclose(fptr);
  }

  return 0;
}
