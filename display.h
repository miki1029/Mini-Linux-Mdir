#ifndef DISPLAY_H_
#define DISPLAY_H_

// ncurses 라이브러리를 사용함
#include <menu.h>
#include <curses.h>
#include "fileinfo.h"
#include "filefunc.h"

// 시스템 메시지와 명령어 string
char message[256];
char command[256];

// 디렉토리 이동 및 조회 결과를 화면에 표시함
char *changeDir(char *dir, int *pState);

#endif
