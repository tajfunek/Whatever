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
#include <sys/select.h>

#define CAMS_NO 1
#define FILENAME_LEN 12
#define THREADS_NO 2
#define SUCCESS 0
#define IMG_NO 1

struct cam_args_t {
  int no;
  char cam[FILENAME_LEN];
  int socket;
  struct sockaddr_un* addr;
};

struct client_t {
  char id[20];
  int socket;
};

struct sockaddr_un addr= {
  .sun_family = AF_UNIX,
  .sun_path = "/tmp/temp"
};

// Function declaration
void* cam(void* args_void);
void* motor(void* args_void);
int compare(void* a_void, void* b_void);

// Function definitions
void* cam(void* args_void) {
  sleep(1);
  char buf[20];
  memset(buf, 0, 20);
  struct cam_args_t args = *((struct cam_args_t*)args_void);
  int socket_d;
  struct sockaddr addr= *((struct sockaddr*)args.addr);
  printf("Thread up: %s \n", args.cam);
  sleep(1);

  // Setting process priority
  errno = 0;
  nice(-3);
  if(errno != 0) {
    printf("Unable to lower process priority\n");
    abort();
  }

  if((socket_d = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
    printf("Unable to create socket\n");
    abort();
  }

  errno = 0;
  printf("SOCKET: %i\n", socket_d);
  sleep(1);
  if(connect(socket_d, &addr, sizeof(struct sockaddr_un)) != 0) {
    printf("Unable to connect to socket: %s\n", args.cam);
    printf("Error: %i\n", errno);
    abort();
  }

  //printf("ID: %s", args.cam);
  sprintf(buf, "%d;%s", args.no, args.cam);
  //printf("DATALEN: %d", strlen(buf));
  //printf("BUF: %s\n", buf);
  for(int i = strlen(buf); i < 20; i++) buf[i] = '\0';
  if(write(socket_d, buf, strlen(buf)) == -1) {
    printf("Unable to write\n");
    abort();
  }

  struct fdset scks;
  FD_ZERO(&scks);
  FD_SET(socket_d, &scks);
  while(1) {
    if(select(20, &scks, NULL, NULL, NULL) > 0){
      memset(buf, 0, 20);
      recv(socket_d, buf, 1, 0);
      if(atoi(buf) == 1) {
        printf("RECV_BUF: %s\n", buf);
        send(socket_d, buf, 1, 0);
        printf("SENT\n");
      }
    }
  }
}


void* motor(void* args_void) {
  sleep(1);
  printf("Thread up: motor\n");
  sleep(1);
  // Setting process priority
  //struct motor_args_t args = *((struct motor_args_t*)args_void);
  errno = 0;
  struct sockaddr* addr_ptr = (struct sockaddr*)args_void;
  struct sockaddr addr = *addr_ptr;
  nice(-4);
  if(errno != 0) {
    printf("Unable to lower process priority\n");
    abort();
  }

  int socket_d;
  if((socket_d = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
    printf("Unable to create socket\n");
    abort();
  }
  printf("SOCKET: %i\n", socket_d);

  if(connect(socket_d, &addr, sizeof(struct sockaddr_un)) != 0) {
    printf("Unable to connect to socket: motor\n");
    printf("Error: %i\n", errno);
    abort();
  }
  char buf[20];
  memset(buf, 0, 20);
  sprintf(buf, "%d;%s\0", 3, "stepper");

  if(write(socket_d, buf, strlen(buf)) == -1) {
    printf("Unable to write\n");
    abort();
  }
}


/*
int compare(void* a_void, void* b_void) {
  struct client_t a = *((struct client_t*)a_void);
  struct client_t b = *((struct client_t*)b_void);
  int a_id = atoi(&(a.id[0]));
  int b_id = atoi(&(b.id[0]));

  if(a_id > b_id) return 1;
  else if (a_id == b_id) return 0;
  else return -1;
}
*/


int main(void) {
  // Setting process priority
  errno = 0;
  nice(-4);
  if(errno != 0) {
    printf("Unable to lower process priority\n");
    abort();
  }

  pthread_t cams[CAMS_NO];
  int socket_d;

  // Creating socket for Inter-Process Communication with childs
  printf("Creating socket...\n");
  if((socket_d = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
    printf("Unable to create socket\n");
    abort();
  }

  // Binding with file
  printf("Preparation for binding...\n");
  //strncpy(*(addr).sun_path, "./temp", sizeof(*(addr).sun_path)-1);
  printf("Bind\n");
  if(bind(socket_d, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) != 0) {
    printf("Unable to bind file to socket\n");
    abort();
  }

  //Starting to listen for connections
  printf("Listening...\n");
  if(listen(socket_d, 8) != 0) {
    printf("Unable to listen on socket\n");
    abort();
  }
  printf("SOCKET: %i\n", socket_d);

  // Creating thread to manage every camera
  struct cam_args_t args[3];
  printf("Creating threads for cams\n");
  for(int i = 0; i < CAMS_NO; i++) {

    // Filling up args struct for every camera
    args[i].socket = socket_d;
    args[i].addr = &addr;
    args[i].no = i;

    int error;
    if((error = sprintf(args[i].cam, "/dev/video%i", i)) != FILENAME_LEN-1) {
      printf("Error while filling up args: %i,   %i,    %s\n", i, error, args[i].cam);
      abort();
    }
    printf("Creating cam thread: %s\n", args[i].cam);

    // Creating thread and checking for error
    if(pthread_create(&cams[i], NULL, &cam, (void*)&args[i]) != SUCCESS) {
      printf("Unable to create camera thread: %i\n", i);
      abort();
    }
  }

  // Arguments for servo function
  printf("Creating thread for motor\n");

  // Create thread for servo controlling
  printf("Creating thread...\n");
  pthread_t motor_thread;
  if(pthread_create(&motor_thread, NULL, &motor, (void*)&addr) != SUCCESS) {
    printf("Unable to create motor thread\n");
    abort();
  }

  // Accepting connections from threads
  printf("Waiting for connection...\n");
  struct client_t ids[THREADS_NO];
  char buf[20];
  size_t socklen = sizeof(struct sockaddr_un);
  for(int i = 0; i < THREADS_NO; /* INSIDE LOOP */) {
    if((ids[i].socket = accept(socket_d, (struct sockaddr*)&addr, &socklen)) != -1) {
      recv(ids[i].socket, buf, 20, 0);
      strncpy(ids[i].id, buf, 15);
      printf("Connected to %s\n", ids[i].id);
      i++;
    } else continue;
  }

  for(int i = 0; i < IMG_NO; i++) {
    for(int i = 0; i < CAMS_NO - 1; i++) // Without top one
      memset(buf, 0, 20);
      sprintf(buf, "%d", 1);
      send(ids[i+1].socket, buf, 1, 0);
  }

  for(int i = 0; i < CAMS_NO - 1; i++) {
    printf("MAIN THREAD WAITING\n");
    memset(buf, 0, 20);
    recv(ids[i+1].socket, buf, 1 ,0);
    if(atoi(buf) == 1) {
      printf("Photo done");
    }
  }


  /*
  // Sorts array
  printf("Sorting...\n");
  qsort(ids, THREADS_NO, sizeof(struct client_t), (__compar_fn_t)&compare);
  printf("LAST THREAD ID: %s", ids[THREADS_NO-1]);
  */

  // Closing connections
  for(int  i = 0; i < THREADS_NO; i++) {
    shutdown(ids[i].socket, SHUT_WR);
  }
  // Deleting socket
  unlink("./temp");

  return 0;
}
