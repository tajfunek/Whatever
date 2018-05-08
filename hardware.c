#include <wiringPi.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#define CAMS_NO 3
#define FILENAME_LEN 12
#define THREADS_NO 4

struct cam_args_t {
  char cam[FILENAME_LEN];
  int socket;
  struct sockaddr* addr;
};

struct motor_args_t {
  int socket;
  struct sockaddr* addr;
};

struct client_t {
  char id[15];
  int socket;
};

// Function declaration
void cam(struct cam_args_t* args);
void motor(struct motor_args_t* args);

int main(void) {
  // Setting process priority
  errno = 0;
  nice(-4);
  if(errno != 0) {
    printf("Unable to lower process priority");
    abort();
  }

  pthread_t cams[CAMS_NO];
  int socket_d;

  // Creating socket for Inter-Process Communication with childs
  if((socket_d = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
    printf("Unable to create socket");
    abort();
  }

  // Binding with file
  struct sockaddr_un* addr;
  addr->sun_family = AF_UNIX;
  strncpy(addr->sun_path, "./temp", sizeof(addr->sun_path)-1);
  if(bind(socket_d, (struct sockaddr*)addr, sizeof(struct sockaddr_un)) != 0) {
    printf("Unable to bind file to socket");
    abort();
  }

  //Starting to listen for connections
  if(listen(socket_d, 8) != 0) {
    printf("Unable to listen on socket");
    abort();
  }

  // Creating thread to manage every camera
  printf("Creating threads for cams\n");
  for(int i = 0; i < CAMS_NO; i++) {

    // Filling up args struct for every camera
    struct cam_args_t* args;
    sprintf(args->cam, "/dev/video%i", i);
    args->socket = socket_d;
    args->addr = (struct sockaddr*)addr;

    // Creating thread and checking for error
    if(pthread_create(&cams[i], NULL, &cam, args) != thrd_success) {
      printf("Unable to create camera thread: %i", i);
      abort();
    }
  }

  // Arguments for servo function
  struct motor_args_t* args;
  args->socket = socket_d
  args->addr = (struct sockaddr*)addr;

  // Create thread for servo controlling
  pthread_t* motor_thread;
  if(pthread_create(motor_thread, NULL, &motor, args) != thrd_success) {
    printf("Unable to create motor thread");
    abort();
  }

  sleep(1);

  // Accepting connections from threads
  struct client_t* ids[THREADS_NO];
  char buf[20];
  for(int i = 0; i < THREADS_NO; /* INSIDE LOOP */) {
    if((ids[i]->socket = accept(socket_d, (struct sockaddr*)addr, sizeof(struct sockaddr_un))) != -1) {
      recv(ids[i]->socket,buf, sizeof(buf), 0);
      strncpy(ids[i]->id, buf, 15);
      printf("Connected to %s", ids[i]->id);
      i++;
    } else continue;
  }



  // Closing connections
  for(int  i = 0; i < THREADS_NO; i++) {
    shutdown(ids[i]->socket, SHUT_WR);
  }
  // Deleting socket
  unlink(SOCK_NAME);

  return 0;
}

void cam(struct cam_args_t* args) {
  char buf[20];

  // Setting process priority
  errno = 0;
  nice(-3);
  if(errno != 0) {
    printf("Unable to lower process priority");
    abort();
  }

  sprintf(buf, "%i;%s", atoi(&(args->cam[11])), args->cam);
  if(connect(args->socket, args->addr, sizeof(struct sockaddr_un)) != 0) {
    printf("Unable to connect to socket: %s", args->cam);
    abort();
  }

  if(write(args->socket, buf, strlen(buf)) == -1) {
    printf("Unable to write");
    abort();
}


}

void motor(struct motor_args_t* args) {
  // Setting process priority
  errno = 0;
  nice(-4);
  if(errno != 0) {
    printf("Unable to lower process priority");
    abort();
  }
}
