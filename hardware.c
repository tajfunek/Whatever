#include <wiringPi.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>a
#include <unistd.h>
#include <errno.h>

#define CAMS_NO 3
#define SOCK_NAME "./cam.socket"
#define FILENAME_LEN 12
#define THREADS_NO 4

struct cam_args_t {
  char cam[FILENAME_LEN];
  int socket;
  struct sockaddr_un* addr;
};

struct motor_args_t {
  int socket;
  struct sockaddr_un* addr;
};

struct client_t {
  char id[15];
  int socket;
};

// Function declaration
void cam(struct cam_args_t*);
void motor(struct motor_args_t*);

int main(void) {
  // Setting process priority
  errno = 0;
  nice(-4);
  if(errno != 0) {
    printf("Unable to lower process priority");
    abort();
  }

  pthread_t cams[CAMS_NO];

  // Creating socket for Inter-Process Communication with childs
  if((socket_d = socket(PF_UNIX, SOCK_STREAM, 0) == -1) {
    printf("Unable to create socket");
    abort();
  }

  // Binding wiht file
  struct sockaddr_un* addr;
  addr->sun_family = AF_UNIX;
  addr->sun_path = SOCK_NAME;
  if(bind(socket_d, addr, sizeof(sockaddr_un)) != 0) {
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
    cam_args_t* args;
    sprintf(&args->cam, "/dev/video%i", i);
    args->socket = socket_d;
    args->addr = addr;

    // Creating thread and checking for error
    if(pthread_create(&cams[i], cam, args) != thrd_success) {
      printf("Unable to create camera thread: %i", i);
      abort();
    }
  }

  // Arguments for servo function
  motor_args_t* args;
  args->socket = socket_d
  args->addr = addr

  // Create thread for servo controlling
  pthread_t* motor;
  if(pthread_create(motor, motor, args != thrd_success) {
    printf("Unable to create motor thread");
    abort();
  }

  sleep(1);

  // Accepting connections from threads
  struct client_t* ids[THREADS_NO];
  char buf[20];
  for(int i = 0; i < THREADS_NO; /* INSIDE LOOP */) {
    if((ids[i]->socket = accept(socket_d, addr, sizeof(sockaddr_un)) != -1) {
      read(ids[i]->socket,buf, sizeof(buf));
      strncpy(ids[i]->id, buf, 15);
      printf("Connected to %s", ids[i]->id)
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

void cam(struct cam_args_t args) {
  // Setting process priority
  errno = 0;
  nice(-3);
  if(errno != 0) {
    printf("Unable to lower process priority");
    abort();
  }

  if(connect(args->socket, args->addr, sizeof(sockaddr_un)) != 0) {
    printf("Unable to connect to socket: %s", args->cam);
    abort();
  }


}

void motor(struct motor_args_t args) {
  // Setting process priority
  errno = 0;
  nice(-4);
  if(errno != 0) {
    printf("Unable to lower process priority");
    abort();
  }
}
