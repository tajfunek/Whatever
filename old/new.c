#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>


int main(void) {
  char pathname_out[] = "./hoii";
  char pathname_in[] = "./hiio";

  // Setting process priority
  errno = 0;
  nice(-4);
  if(errno != 0) {
    printf("Unable to lower process priority\n");
    printf("Run as sudo\n");
    abort();
  }


  if(mkfifo(pathname_out, 777) != 0) {
    printf("Unable to create FIFO\n");
    return 10;
  }

  if(mkfifo(pathname_in, 777) != 0) {
    printf("Unable to create FIFO\n");
    unlink(pathname_out);
    return 11;
  }
  printf("FIFOs created\n");

  while(1) {
    
  }

  unlink(pathname_out);
  unlink(pathname_in);
  return 0;
}
