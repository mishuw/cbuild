#include <stdbool.h>

/*
 * Verdiğin shell command'i çalıştırır, output'u yazar.
 * pclose'dan dönen status'u status pointer'ına yazar.
 * status 0 değilse bi şeyler ters gitmişdir.
 * detaylı bakmak istiyorsan WIF... macro'larına girersin.
 *
 * name -> log/debug için hangi işlem olduğunu anlamak adına tag gibi düşün
*/
void cmd(char* name, int *status, char* cmd);

/*
 * Command patlarsa debug info yazar.
 * ne düşmüş, neden error?
*/
void errorInfo(char* name, int status);

/*
 * Eğer command fail olduysa direkt çıkış yapar.
 * build step falan burda kesilir, devam etmez.
*/
void exitError(char* name, int status);

/*
 * Command fail mi olmuşmu diye bakar.
 * true -> patladı
 * false -> sıkıntı yok devam
*/
bool isFailed(int status);

/*
 * Command string'ine ekleme yaparsın.
 * printf gibi çalışır, format basarsın üstüne ekler.
 * uzun command build ederken hayat kurtarır.
*/
void append(char **cmd, const char *fmt, ...)
 __attribute__((format(printf, 2, 3)));