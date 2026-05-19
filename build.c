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

  /*
   * Başlangıçtan sona kadar geçen süreyi ölçer.
   * startTime zaten dışardan geliyor, burada endTime alıyoruz.
  */

  // saniyeyi ms'e çevr
  elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;
  elapsedTime += (endTime.tv_usec - startTime.tv_usec) / 1000.0;

  /*
   * total süreyi ms cinsinden yazdır.
   * name -> hangi işlem
  */
  printf("[%s]: %fms\n", name, elapsedTime);
}

void cmd(char *name, int *status, char *cmd) {
  printf("[%s]: %s\n", name, cmd);

  struct timeval startTime;
  gettimeofday(&startTime, NULL);

  FILE *fp;
  char *result;

  /*
   * popen komut çalıştırıyo.
   * "r" -> outputu oku
  */
  fp = popen(cmd, "r");
  if (fp == NULL) {
    fprintf(stderr, "[%s] cmd(): FILE açılamadı: %s\n", name, strerror(errno));
    printTime(name, startTime);
    exit(EXIT_FAILURE);
  }

  /*
   * command outputu okur ve yazar.
   * burada result'a doldurup direkt stdout'a veriyoruz
  */
  while (fscanf(fp, NULL, &result) == 1) {
    printf("%s", result);
  }

  /*
   * iş bitti, stream'i kapatıyoruz
   * dönen status -> command'in exit code'u
  */
  int closeStatus = pclose(fp);
  if (closeStatus == -1) {
    fprintf(stderr, "[%s] cmd(): pclose patladı: %s\n", name, strerror(errno));
    printTime(name, startTime);
    exit(EXIT_FAILURE);
  }

  /*
   * debug için status
   * ardından geçen süreyi yazdırıyoruz
  */
  printf("[%s] status: %d\n", name, closeStatus);
  printTime(name, startTime);

  // dışarıya status
  *status = closeStatus;
}

void errorInfo(char *name, int status) {
  /*
   * status 0'dan büyükse bir şeyler ters.
   * burada olayı parçalayarak debug info basıyoruz.
  */
  if (status > 0) {
    printf("[%s] status: %d\n", name, status);

    /*
     * process normal şekilde mi çıkmış?
     * exit code'u alıp basıyoruz
    */
    if (WIFEXITED(status)) {
      printf("[%s] process düzgün kapandı\n", name);
      int exitStatus = WEXITSTATUS(status);
      printf("[%s] exit code: %d\n", name, exitStatus);
    }

    /*
     * signal yüzünden mi patladı?
     * hangi signal olduğunu öğreniyoruz
    */
    if (WIFSIGNALED(status)) {
      printf("[%s] process signal yüzünden göçtü\n", name);
      int exitSignal = WTERMSIG(status);
      printf("[%s] signal: %d\n", name, exitSignal);
    }
    
    /*
     * process durdurulmuş mu (pause gibi)
    */
    if (WIFSTOPPED(status)) {
      printf("[%s] process signal ile durduruldu\n", name);
      int stopSig = WSTOPSIG(status);
      printf("[%s] signal: %d\n", name, stopSig);
    }
  }
}

void exitError(char *name, int status) {
  /*
   * status > 0 ise build bir yerde patlamış demek.
   * hiç uzatmıyoruz, direkt fail verip çıkıyoruz.
  */
  if (status > 0) {
    printf("BUILD FAILED!! %s sıçtı\n", name);
    exit(EXIT_FAILURE);
  }
}

bool isFailed(int status) {
  /*
   * true -> patladı
   * false -> devam
  */
  if (status > 0) return true;
  else return false;
}

void append(char **src, const char *fmt, ...) {
  va_list args;
  char *new_part = NULL;

  /*
   * printf gibi formatlı string oluşturuyoruz
  */
  va_start(args, fmt);
  int n = vasprintf(&new_part, fmt, args); 
  va_end(args);

  if (n == -1) {
    perror("vasprintf a");
    exit(EXIT_FAILURE);
  }

  // boş string geldiyse hiç uğraşma
  if (n == 0) return;

  /*
   * src null ise direkt allocate ediyoruz,
   * değilse realloc + boşluk koyup ekliyoruz
  */
  if (*src == NULL) {
    *src = malloc(strlen(new_part) + 1);
    strcpy(*src, new_part);
  } else {
    *src = realloc(*src, strlen(*src) + strlen(new_part) + 2);
    strcat(*src, " ");  
    strcat(*src, new_part);
  }

  /*
   * geçici parçayı temizliyoruz
  */
  free(new_part);
}