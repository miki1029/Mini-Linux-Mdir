#include "display.h"

int main(void) {
    char* newpath = ".";
    int runState = TRUE;

    bufFileFlg = FLG_EMPTY;
    command[0] = 0;
    message[0] = 0;
    //signal(SIGINT, SIG_IGN); // ncurses 라이브러리에서 처리가 되어있음

    while (runState) {
        newpath = changeDir(newpath, &runState);
    }

    return 0;
}
