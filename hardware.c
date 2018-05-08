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
#define SUCCESS 0

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
void* cam(void* args_void);
void* motor(void* args_void);

// Function definitions
void* cam(void* args_void) {
  char buf[20];
  struct cam_args_t* args = (struct cam_args_t*)args_void;

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


void* motor(void* args_void) {
  // Setting process priority
  struct motor_args_t* args = (struct motor_args_t*)args_void;
  errno = 0;
  nice(-4);
  if(errno != 0) {
    printf("Unable to lower process priority");
    abort();
  }
}


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
  printf("Creating socket...\n");
  if((socket_d = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
    printf("Unable to create socket");
    abort();
  }

  // Binding with file
  printf("Preparation for binding...\n");
  struct sockaddr_un addr= {
    .sun_family = AF_UNIX,
    .sun_path = "./temp"
  };
  //strncpy(*(addr).sun_path, "./temp", sizeof(*(addr).sun_path)-1);
  printf("Bind\n");
  if(bind(socket_d, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) != 0) {
    printf("Unable to bind file to socket");
    abort();
  }

  //Starting to listen for connections
  printf("Listening...\n");
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
    args->addr = (struct sockaddr*)&addr;

    // Creating thread and checking for error
    if(pthread_create(&cams[i], NULL, &cam, args) != SUCCESS) {
      printf("Unable to create camera thread: %i", i);
      abort();
    }
  }

  // Arguments for servo function
  printf("Creating thread for motor\n");
  struct motor_args_t* args;
  args->socket = socket_d;
  args->addr = (struct sockaddr*)&addr;

  // Create thread for servo controlling
  pthread_t* motor_thread;
  if(pthread_create(motor_thread, NULL, &motor, args) != SUCCESS) {
    printf("Unable to create motor thread");
    abort();
  }

  sleep(1);

  // Accepting connections from threads
  printf("Waiting for connection...\n");
  struct client_t* ids[THREADS_NO];
  char buf[20];
  size_t socklen = sizeof(struct sockaddr_un);
  for(int i = 0; i < THREADS_NO; /* INSIDE LOOP */) {
    if((ids[i]->socket = accept(socket_d, (struct sockaddr*)&addr, (socklen_t*)&socklen)) != -1) {
      recv(ids[i]->socket,buf, sizeof(buf), 0);
      strncpy(ids[i]->id, buf, 15);
      printf("Connected to %s\n", ids[i]->id);
      i++;
    } else continue;
  }



  // Closing connections
  for(int  i = 0; i < THREADS_NO; i++) {
    shutdown(ids[i]->socket, SHUT_WR);
  }
  // Deleting socket
  unlink("./temp");

  return 0;
}
