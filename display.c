#include "display.h"

// ncurses 라이브러리 위주의 코딩으로 되어있음
char *changeDir(char *dir, int *pState) {
    int i;
    MENU *my_menu;      // ncurses MENU
    ITEM **my_items;    // ncurses ITEM : all items
    ITEM *cur_item;     // ncurses ITEM : curser
    int c;              // 사용자 키 입력
    int n_choices;      // 현재 디렉토리 파일 개수
    struct fileinfo* fis = NULL;    // fileinfo 구조체 배열
    char **choices = NULL;          // fileinfo->string
    char *newpath = ".";    // return value. 디렉토리 이동시 새 path가 설정됨
    char path[256];         // 현재 디렉토리의 path
    int bFlg = TRUE;        // while문 제어 flag(FALSE시 빠져 나옴)

    // 디렉토리 이동
    chdir(dir);
    // 파일 정보를 fileinfo 구조체 배열로 가져옴
    fis = get_fileinfo(&n_choices);
    // 가져온 fileinfo 구조체 배열을 화면에 보여주기 위해 string으로 가져옴
    choices = get_list(n_choices, fis);

    /* Initialize curses */
    initscr();
    start_color();
    raw();
    //cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);		// cur
    //init_pair(2, COLOR_BLUE, COLOR_BLACK);		// directory
    //init_pair(3, COLOR_GREEN, COLOR_BLACK);		// execute
    //init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

    /* Initialize items */
    my_items = (ITEM **) calloc(n_choices + 1, sizeof(ITEM *));
    char** buf;
    buf = (char**) malloc(sizeof(char*) * n_choices);
    for (i = 0; i < n_choices; ++i) {
        buf[i] = malloc(sizeof(char) * 2);
        sprintf(buf[i], "%d", i);
        my_items[i] = new_item(buf[i], choices[i]);
    }
    my_items[n_choices] = (ITEM *) NULL;

    /* Create menu */
    // 디렉토리 파일 목록
    my_menu = new_menu((ITEM **) my_items);
    set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);

    /* Post the menu */
    getcwd(path, sizeof(path));
    // 현재 버퍼값(path)을 보여줌
    if (bufFileFlg == FLG_COPY)
        sprintf(message, "buffer(copy): %s", bufFilePath);
    else if (bufFileFlg == FLG_CUT)
        sprintf(message, "buffer(cut): %s", bufFilePath);
    // 디렉토리를 이동했을 시, cd 명령어를 보여줌
    if (strcmp(dir, ".") != 0)
        sprintf(command, "command: cd %s", dir);
    // 화면 구성
    mvprintw(17, 0, message);
    mvprintw(18, 0, command);
    mvprintw(19, 0, path);
    mvprintw(20, 0, "Ctrl+c: copy | Ctrl+x: cut | Ctrl+v: paste | Ctrl+e: execute | Ctrl+q: quit | del: remove");
    post_menu(my_menu);
    refresh();
    message[0] = 0;
    command[0] = 0;

    // 사용자 입력을 제어함, bFlg: 반복문 제어 flag
    while (bFlg && ((c = getch()) != KEY_F(1))) {
        switch (c) {
        // 커서 이동
        case KEY_DOWN:
            menu_driver(my_menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(my_menu, REQ_UP_ITEM);
            break;

        case 10:	// Enter
        {
            ITEM *cur;
            int idx;

            cur = current_item(my_menu);
            idx = cur->index;
            // 디렉토리일 경우 : change directory
            if (S_ISDIR(fis[idx].statbuf.st_mode)) {
                newpath = (char*) malloc(sizeof(char) * 1024);
                // 새로운 path를 설정하고 반복문을 빠져나옴 -> main에서 재호출시 디렉토리 이동됨.
                sprintf(newpath, "%s", fis[idx].name);
                bFlg = FALSE;
            }
            // 그 외 : edit vi
            else {
                char viCmd[1024] = "vi ";
                strcat(viCmd, fis[idx].name);

                def_prog_mode();
                endwin();
                system(viCmd);
                reset_prog_mode();
                refresh();

                sprintf(message, "message: edit %s", fis[idx].name);
                move(17, 0);
                clrtoeol();
                mvprintw(17, 0, message);
                sprintf(command, "command: vi %s", fis[idx].name);
                move(18, 0);
                clrtoeol();
                mvprintw(18, 0, command);
                bFlg = FALSE;
            }
            break;
        }
        case 5:		// Ctrl + e : 프로그램 실행
        {
            ITEM *cur;
            int idx;
            char exePath[1024] = "./";

            cur = current_item(my_menu);
            idx = cur->index;
            // execute program
            if (access(exePath, X_OK) == 0) {
                strcat(exePath, fis[idx].name);
                def_prog_mode();
                endwin();
                system(exePath);
                getchar();
                reset_prog_mode();
                refresh();

                sprintf(message, "message: execute %s", exePath);
                move(17, 0);
                clrtoeol();
                mvprintw(17, 0, message);
                sprintf(command, "command: %s", exePath);
                move(18, 0);
                clrtoeol();
                mvprintw(18, 0, command);
                bFlg = FALSE;
            }
            break;
        }
        case 3:		// Ctrl + c : 파일 복사
        {
            ITEM *cur;
            int idx;

            cur = current_item(my_menu);
            idx = cur->index;
            bufFileDir = FALSE;
            if (S_ISDIR(fis[idx].statbuf.st_mode)) {
                bufFileDir = TRUE; // directory
            }
            // file function
            copyFile(fis[idx].name);
            sprintf(message, "message: copy %s", fis[idx].name);
            move(17, 0);
            clrtoeol();
            mvprintw(17, 0, message);
            break;
        }
        case 17:	// Ctrl + q : 프로그램 종료
            *pState = FALSE;
            bFlg = FALSE;
            break;
        case 22:	// Ctrl + v : 파일 붙여넣기
            sprintf(message, "message: paste %s", bufFilePath);
            move(17, 0);
            clrtoeol();
            mvprintw(17, 0, message);
            // 파일 복사였을 경우 : cp
            if (bufFileFlg == FLG_COPY)
                sprintf(command, "command: cp %s %s", bufFilePath, "./");
            // 파일 잘라내기였을 경우 : mv
            else if (bufFileFlg == FLG_CUT)
                sprintf(command, "command: mv %s %s", bufFilePath, "./");
            move(18, 0);
            clrtoeol();
            mvprintw(18, 0, command);
            // file function
            pasteFile();
            bFlg = FALSE;
            break;
        case 24:	// Ctrl + x : 파일 잘라내기
        {
            ITEM *cur;
            int idx;

            cur = current_item(my_menu);
            idx = cur->index;
            bufFileDir = FALSE;
            if (S_ISDIR(fis[idx].statbuf.st_mode)) {
                bufFileDir = TRUE; // directory
            }
            // file function
            cutFile(fis[idx].name);
            sprintf(message, "message: cut %s", fis[idx].name);
            move(17, 0);
            clrtoeol();
            mvprintw(17, 0, message);
            break;
        }
        case 330:	// delete : 파일 삭제
        {
            ITEM *cur;
            int idx;

            cur = current_item(my_menu);
            idx = cur->index;
            sprintf(message, "message: remove %s", fis[idx].name);
            move(17, 0);
            clrtoeol();
            mvprintw(17, 0, message);
            bufFileDir = FALSE;
            if (S_ISDIR(fis[idx].statbuf.st_mode)) {
                sprintf(command, "command: rmdir %s", fis[idx].name);
                bufFileDir = TRUE;
            } else {
                sprintf(command, "command: rm %s", fis[idx].name);
            }
            move(18, 0);
            clrtoeol();
            mvprintw(18, 0, command);
            // file function
            removeFile(fis[idx].name);
            bFlg = FALSE;
            break;
        }
        default:
            break;
        }
    }
    unpost_menu(my_menu);
    for (i = 0; i < n_choices; ++i)
        free_item(my_items[i]);
    free_menu(my_menu);
    endwin();

    return newpath;
}
