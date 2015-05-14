#ifndef FILEFUNC_H_
#define FILEFUNC_H_

#include "fileinfo.h"

// buffer flag : 현 상태가 복사인지 자르기인지 아무것도 아닌지
#define FLG_EMPTY 0
#define FLG_COPY 1
#define FLG_CUT 2

// file buffer : 복사(자르기) 상태일 경우 파일의 path 저장
char bufFilePath[2048];
char bufFileName[1024];
int bufFileFlg;
int bufFileDir;

// file function
void copyFile(char *fname);
void cutFile(char *fname);
void pasteFile();
void removeFile(char *fname);

#endif
