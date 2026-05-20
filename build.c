#include <stdbool.h>
#include "./build.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <stdarg.h>

void printTime(char *name, struct timeval startTime) {
  struct timeval endTime;
  gettimeofday(&endTime, NULL);
  double elapsedTime;

  elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;
  elapsedTime += (endTime.tv_usec - startTime.tv_usec) / 1000.0;

  printf("[%s]: %fms\n", name, elapsedTime);
}

void cmd(char *name, int *status, char *cmd) {
  printf("[%s]: %s\n", name, cmd);

  struct timeval startTime;
  gettimeofday(&startTime, NULL);

  FILE *fp;
  char *result;

  fp = popen(cmd, "r");
  if (fp == NULL) {
    fprintf(stderr, "[%s] cmd(): FILE açılamadı: %s\n", name, strerror(errno));
    printTime(name, startTime);
    exit(EXIT_FAILURE);
  }

  while (fscanf(fp, NULL, &result) == 1) {
    printf("%s", result);
  }

  int closeStatus = pclose(fp);
  if (closeStatus == -1) {
    fprintf(stderr, "[%s] cmd(): pclose patladı: %s\n", name, strerror(errno));
    printTime(name, startTime);
    exit(EXIT_FAILURE);
  }

  printf("[%s] status: %d\n", name, closeStatus);
  printTime(name, startTime);

  *status = closeStatus;
}

void errorInfo(char *name, int status) {
  if (status > 0) {
    printf("[%s] status: %d\n", name, status);

    if (WIFEXITED(status)) {
      printf("[%s] process düzgün kapandı\n", name);
      int exitStatus = WEXITSTATUS(status);
      printf("[%s] exit code: %d\n", name, exitStatus);
    }
    
    if (WIFSIGNALED(status)) {
      printf("[%s] process signal yüzünden göçtü\n", name);
      int exitSignal = WTERMSIG(status);
      printf("[%s] signal: %d\n", name, exitSignal);
    }
    
    if (WIFSTOPPED(status)) {
      printf("[%s] process signal ile durduruldu\n", name);
      int stopSig = WSTOPSIG(status);
      printf("[%s] signal: %d\n", name, stopSig);
    }
  }
}

void exitError(char *name, int status) {
  if (status > 0) {
    printf("BUILD FAILED!! %s sıçtı\n", name);
    exit(EXIT_FAILURE);
  }
}

bool isFailed(int status) {
  if (status > 0) return true;
  else return false;
}

void append(char **src, const char *fmt, ...) {
  va_list args;
  char *new_part = NULL;

  va_start(args, fmt);
  int n = vasprintf(&new_part, fmt, args); 
  va_end(args);

  if (n == -1) {
    perror("vasprintf a");
    exit(EXIT_FAILURE);
  }

  if (n == 0) return;

  if (*src == NULL) {
    *src = malloc(strlen(new_part) + 1);
    strcpy(*src, new_part);
  } else {
    *src = realloc(*src, strlen(*src) + strlen(new_part) + 2);
    strcat(*src, " ");  
    strcat(*src, new_part);
  }

  free(new_part);
}