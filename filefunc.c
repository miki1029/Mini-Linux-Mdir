#include "filefunc.h"

// file function
void copyFile(char *fname) {
    strcpy(bufFileName, fname);
    getcwd(bufFilePath, sizeof(bufFilePath));
    strcat(bufFilePath, "/");
    strcat(bufFilePath, bufFileName);
    bufFileFlg = FLG_COPY;
}

void cutFile(char *fname) {
    strcpy(bufFileName, fname);
    getcwd(bufFilePath, sizeof(bufFilePath));
    strcat(bufFilePath, "/");
    strcat(bufFilePath, bufFileName);
    bufFileFlg = FLG_CUT;
}

void pasteFile() {
    copyProcess(bufFilePath, bufFileName);

    if (bufFileFlg == FLG_CUT) {
        if (bufFileDir) {
            rmdir(bufFilePath);
        } else {
            unlink(bufFilePath);
        }
    }

    bufFileFlg = FLG_EMPTY;
    bufFilePath[0] = 0;
    bufFileName[0] = 0;
}

void removeFile(char *fname) {
    char *rmFilePath;
    rmFilePath = (char*) malloc(sizeof(char) * 256);
    getcwd(rmFilePath, sizeof(bufFilePath));
    strcat(rmFilePath, "/");
    strcat(rmFilePath, fname);
    if (bufFileDir) {
        rmdir(rmFilePath);
    } else {
        unlink(rmFilePath);
    }
}

// copy process
int copyProcess(char *infile, char *outfile) {
    int fd1, fd2;
    ssize_t rcount, wcount;
    char buffer[BUFSIZ];
    int errors = 0;
    struct stat statbuf;
    mode_t inmode = 0644;

    // infile open
    if ((fd1 = open(infile, O_RDONLY)) < 0) {
        fprintf(stderr, "%s: cannot open for reading: %s\n", infile, strerror(errno));
        return 1;
    }

    // outfile open
    if ((fd2 = open(outfile, O_WRONLY | O_CREAT)) < 0) {
        fprintf(stderr, "%s: cannot open for writing: %s\n", outfile, strerror(errno));
        return 1;
    }

    // mode
    if (fstat(fd1, &statbuf) < 0) {
        fprintf(stderr, "%s: fstat fail: %s\n", infile, strerror(errno));
        errors++;
    } else {
        inmode = statbuf.st_mode;
        if (fchmod(fd2, statbuf.st_mode) < 0) {
            fprintf(stderr, "%s: fchmod fail: %s\n", outfile, strerror(errno));
            errors++;
        }
    }

    // copy
    while ((rcount = read(fd1, buffer, sizeof(buffer))) > 0) {
        wcount = write(fd2, buffer, rcount);
        if (wcount != rcount) {
            fprintf(stderr, "%s: write error: %s\n", outfile, strerror(errno));
            errors++;
            break;
        }
    }
    if (rcount < 0) {
        fprintf(stderr, "%s: read error: %s\n", infile, strerror(errno));
        errors++;
    }

    // infile close
    if (fd1 != 0) {
        if (close(fd1) < 0) {
            fprintf(stderr, "%s: close error: %s\n", infile, strerror(errno));
            errors++;
        }
    }

    // outfile close
    if (fd2 != 0) {
        if (close(fd2) < 0) {
            fprintf(stderr, "%s: close error: %s\n", outfile, strerror(errno));
            errors++;
        }
    }

    return (errors != 0);
}
