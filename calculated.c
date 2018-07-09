#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <signal.h>
#include <png.h>

#define THREAD_NO 24
#define DIR_IMG "./images_new2/"
#define IMG_NO 192
#define BUF_LEN 64
#define FILE_NO 192

int progress;
int threads_running = 0;

struct arg_calc {
  int pipefd;
  char filename[16];
};

struct ret_png {
  int width;
  int height;
  png_bytepp row_pointers;
};

struct sequence {
  int start;
  int end;
  float avg;
  int len;
};

static pthread_mutex_t mutex;

/*void padding(char* str, int pad) {
  int len = strlen(str);
  int pad_len = pad - len;
  char padding[pad_len];
  for(int i = 0; i < pad_len; i++) {
    padding[i] = '\0';
  }
  strcat(str, padding);
}*/

struct ret_png* read_png(char*);
void getpointConst(struct ret_png*, int);

void* calculate(void* args) {
  char buf[BUF_LEN];
  sleep(1);
  struct arg_calc* args_ptr = (struct arg_calc*)args;
  memset(buf, 0, BUF_LEN * sizeof(char));

  strncpy(buf, DIR_IMG, 64);
  strcat(buf, args_ptr->filename);

  struct ret_png* png = read_png(buf);
  memset(buf, 0, BUF_LEN * sizeof(char));
  if(png->row_pointers != NULL) {
    sprintf(buf, "prtSuccecful reading: %s, width = %i, height = %i",
                args_ptr->filename, png->width, png->height);
  } else {
    sprintf(buf, "prtFailed: %s", args_ptr->filename);
  }
  write(args_ptr->pipefd, buf, 64);

  for(int i = 0; i < png->height; i++) {
    getpointConst(png, i);
    free(png->row_pointers[i]);
  }

  pthread_mutex_lock(&mutex);
  progress++;
  threads_running--;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

void* file_buffer(void* args) {
  FILE* output = fopen("./output.txt", "w");
  int pipe = *(int*)args;
  char buf[BUF_LEN];

  while(1) {
    read(pipe, buf, BUF_LEN);
    //printf("Received: %s\n", buf);
    char cmd[4];
    strncpy(cmd, buf, 3);
    cmd[3] = '\0';
    if(!strcmp(cmd, "prt")) {
      printf("%s\n", &buf[3]);
    } else if(strcmp(cmd, "res")) {
      fprintf(output, "%s", &buf[3]);
    }
    fflush(stdout);
    if(!strcmp(buf, "finish")) {
      break;
    }
  }

  return NULL;
}

int main(void) {
  time_t start = time(NULL);
  printf("Program started\n");

  int pipefd[2]; // pipefd[0] - read end / pipefd[1] - write end
  if(pipe(pipefd) != 0) {
    printf("ERROR: Pipe cannot be created; %i", errno);
  }

  if(pthread_mutex_init(&mutex, NULL)) {
    printf("ERROR while mutexing\n");
    return 5;
  }

  DIR* images_dir = opendir(DIR_IMG);
  char filenames_full[FILE_NO+2][16];
  struct dirent* entry = readdir(images_dir);
  unsigned int i = 0;
  while(entry != NULL) {
    strncpy(filenames_full[i], entry->d_name, 16);
    filenames_full[i][15] = '\0';
    i++;
    entry = readdir(images_dir);
    fflush(stdout);
  }
  //printf("HERE1");
  char filenames[IMG_NO][16];
  for(int j = 0, i = 0; i < FILE_NO + 2; i++) {
    //printf("WorKING\n");
    if((strcmp(filenames_full[i], ".") == 0) || (strcmp(filenames_full[i], "..") == 0)) {
      continue;
    }
    strncpy(filenames[j], filenames_full[i], 16);
    printf("FILE %i: %s\n", j, filenames[j]);
    j++;
  }

  int j;
  for(j = 0, i = FILE_NO; i < IMG_NO; i++, j++) {
    printf("j = %i\n", j);
    if(j == FILE_NO) j = 0;
    strncpy(filenames[i], filenames[j], 16);
  }
  printf("i = %i\n", i);

  printf("Directory read\n");
  fflush(stdout);
  if(i < IMG_NO-1) {
    printf("Less than IMG_NO images in directory\n");
    return 4;
  }
  printf("Number of images is correct: %i\n", i);

  //raise(SIGINT);
  pthread_t threads[IMG_NO];
  pthread_attr_t attributes;
  pthread_attr_init(&attributes);
  pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
  struct arg_calc args[IMG_NO];
  //threads_running = THREAD_NO;

  for(i = 0; i < IMG_NO; i++) {
    args[i].pipefd = pipefd[1];
    strncpy(args[i].filename, filenames[i], 16);
    //free(filenames[i]);
  }

  pthread_t buffer;
  pthread_create(&buffer, NULL, &file_buffer, pipefd);

  //printf("Threads launched\nThreads running: %i\n", threads_running);
  i = 0;
  while(1) {
    int active_threads = threads_running;
    //raise(SIGINT);
    //printf("SPAM: %i\n", active_threads);
    if(i != IMG_NO) {
      if(active_threads != THREAD_NO) {
        printf("Creating thread: %i\n", i);
        pthread_mutex_lock(&mutex);
        pthread_create(&threads[i], &attributes,  &calculate, &args[i]);
        threads_running++;
        pthread_mutex_unlock(&mutex);
        i++;
      }
    }
    if(threads_running == 0) break;
  }

  //sleep(2);
  printf("Thread running: %i\n", threads_running);

  write(pipefd[1], "finish", 64);
  pthread_join(buffer, NULL);

  fflush(stdout);
  close(pipefd[0]);
  close(pipefd[1]);
  printf("Took: %li seconds\n", (time(NULL)- start));
  return 0;
}

struct ret_png* read_png(char* filename) {
  FILE* image = fopen(filename, "rb");
  if(image == NULL) {
    return NULL;
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png_ptr) {
    return NULL;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    return NULL;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
       png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
       fclose(image);
       return NULL;
   }

   png_init_io(png_ptr, image);

   png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);

   int width = png_get_image_width(png_ptr, info_ptr);
   int height = png_get_image_height(png_ptr, info_ptr);

   png_bytepp row_pointers;
   row_pointers = png_get_rows(png_ptr, info_ptr);

   struct ret_png* ret = malloc(sizeof(struct ret_png));
   ret->width = width;
   ret->height = height;
   ret->row_pointers = row_pointers;

   fclose(image);

   return ret;
}

int compar(const void* a, const void* b) {
  a_ = (struct sequence*)a;
  b_ = (struct sequence*)b;
  if(a_->avg > b_->avg) return 1;
  if(a_->avg < b_->avg) return -1;
  else return 0;
}

void getpointConst(struct ret_png* png, int row_no) {
  png_bytep row = png->row_pointers[row_no];
  const int RED = 140;
  int* REDlist = malloc(png->width * sizeof(int));//[png->width]
  struct sequence sequences = malloc(50 * sizeof(struct sequence));//[(png->width)/4];
  int red_len = 0;
  int seq_len = 0;

  for(int i = 0; i < png->width; i++) {
    if(row[3 * i + 1] >= RED) { // 3 * i ponieważ nie usunąłem innych kolorów
      REDlist[red_len] = i;
      red_len++;
    }
  }

  for(int i = 0; i < red_len;) {
    if(REDlist[i] + 2 == REDlist[i + 2] || REDlist[i] + 1 == REDlist[i + 1]) {
      struct sequence temp;
      int condition = 1;
      temp.start = i;
      int j = 0;

      while(condition == 1) {
        if(REDlist[j] + 2 == REDlist[j + 2] || REDlist[j] + 1 == REDlist[j + 1]) {
          j++;
        } else {
          condition = 0;
          temp.end = i + j +2;
        }
      }
      i = temp.end;
      temp.len = temp.end - temp.start;
      temp.avg = temp.len * (temp.start + temp.end) / 2;

      memcpy(&sequences[seq_len], &temp, sizeof(struct sequence));
      //free(temp);
      seq_len++;
    } else i++;
  }

  qsort(sequences, seq_len, sizeof(struct sequence), &compar);

  free(REDlist);
  free(sequences);
}
