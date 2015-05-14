#ifndef FILEINFO_H_
#define FILEINFO_H_

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

// 파일의 정보를 갖는 구조체
struct fileinfo {
    char name[1024];
    char datestring[256];
    struct stat statbuf;
    struct passwd pwent;
    int isset_pw;
    struct group grp;
    int isset_g;
};

// 디렉토리를 스캔하여 fileinfo형 배열로 가져옴
struct fileinfo *get_fileinfo(int *pCount);
// fileinfo형 배열을 string 배열로 반환
char **get_list(int count, struct fileinfo *fis);

#endif
