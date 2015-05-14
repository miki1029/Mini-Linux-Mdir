#include "fileinfo.h"

// mode -> string 변환
const char *perms_to_string(mode_t mode) {
    char ftype = '?';
    static char perms_buff[30];

    if (S_ISREG(mode))
        ftype = '-';
    if (S_ISLNK(mode))
        ftype = 'l';
    if (S_ISDIR(mode))
        ftype = 'd';
    if (S_ISBLK(mode))
        ftype = 'b';
    if (S_ISCHR(mode))
        ftype = 'c';
    if (S_ISFIFO(mode))
        ftype = '|';

    sprintf(perms_buff, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,
            mode & S_IRUSR ? 'r' : '-', mode & S_IWUSR ? 'w' : '-',
            mode & S_IXUSR ? 'x' : '-', mode & S_IRGRP ? 'r' : '-',
            mode & S_IWGRP ? 'w' : '-', mode & S_IXGRP ? 'x' : '-',
            mode & S_IROTH ? 'r' : '-', mode & S_IWOTH ? 'w' : '-',
            mode & S_IXOTH ? 'x' : '-', mode & S_ISUID ? 'U' : '-',
            mode & S_ISGID ? 'G' : '-', mode & S_ISVTX ? 'S' : '-');

    return (const char *) perms_buff;
}

// fileinfo -> string 변환
char *fileinfo_to_string(struct fileinfo* fi) {
    char *filestring;
    char *target;

    filestring = (char*) malloc(sizeof(char) * 1024);
    target = filestring;

    // Print out type, permissions, and number of links.
    target += sprintf(target, "%10.10s", perms_to_string(fi->statbuf.st_mode));
    target += sprintf(target, "%3d", fi->statbuf.st_nlink);

    if (fi->isset_pw)
        target += sprintf(target, " %s", fi->pwent.pw_name);
    else
        target += sprintf(target, " %d", fi->statbuf.st_uid);

    if (fi->isset_g)
        target += sprintf(target, " %s", fi->grp.gr_name);
    else
        target += sprintf(target, " %d", fi->statbuf.st_gid);

    // Print size of file.
    target += sprintf(target, " %5d", (int )fi->statbuf.st_size);

    target += sprintf(target, " %s", fi->datestring);
    target += sprintf(target, " %s", fi->name);

    return filestring;
}

// 디렉토리를 스캔하여 fileinfo형 배열로 가져옴
struct fileinfo *get_fileinfo(int *pCount) {
    int i;
    struct direct **files;
    struct passwd *pwentp;
    struct group *grpt;
    char *buf;
    struct tm time;
    struct fileinfo *fis; // fileinfo dynamic array, return value

    // 현재 디렉토리를 스캔하여
    *pCount = scandir(".", &files, 0, alphasort);
    if (*pCount > 0) {
        fis = (struct fileinfo*) malloc(sizeof(struct fileinfo) * *pCount);

        // 디렉토리 내의 파일을 하나씩 fileinfo형으로 가져온다.
        for (i = 0; i < *pCount; ++i) {
            // name
            strncpy(fis[i].name, files[i]->d_name, sizeof(fis[i].name));

            // statbuf
            if (stat(files[i]->d_name, &fis[i].statbuf) == 0) {
                // datestring
                localtime_r(&fis[i].statbuf.st_mtime, &time);
                strftime(fis[i].datestring, sizeof(fis[i].datestring), "%F %T", &time);

                // pwent
                fis[i].isset_pw = FALSE;
                buf = (char*) malloc(sizeof(char) * 1024);
                if (!getpwuid_r(fis[i].statbuf.st_uid, &fis[i].pwent, buf, 1024, &pwentp))
                    fis[i].isset_pw = TRUE;

                // grp
                fis[i].isset_g = FALSE;
                buf = (char*) malloc(sizeof(char) * 1024);
                if (!getgrgid_r(fis[i].statbuf.st_gid, &fis[i].grp, buf, 1024, &grpt))
                    fis[i].isset_g = TRUE;
            }
            free(files[i]);
        }
        free(files);
    }

    return fis;
}

// fileinfo형 배열을 string 배열로 반환
char **get_list(int count, struct fileinfo *fis) {
    int i;
    char **choices;

    if (count > 0) {
        choices = (char**) malloc(sizeof(char*) * count);

        for (i = 0; i < count; ++i) {
            // fis -> choices
            choices[i] = fileinfo_to_string(&fis[i]);
        }
    }

    return choices;
}
