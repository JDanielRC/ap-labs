#define _XOPEN_SOURCE 500
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/inotify.h>
#include <limits.h>
#include "logger.h"

int inotifyFd, wd;

//used from: https://linux.die.net/man/3/nftw
static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
    if(tflag == FTW_D && ftwbuf->level <= 2){ //check for level, either one mentioned or one above
         wd = inotify_add_watch(inotifyFd, fpath, IN_CREATE | IN_DELETE | IN_MOVE);
        if (wd == -1)
            errorf("inotify_add_watch");
    }
    return 0; 
}

//used from: The Linux Programming Interface inotify/demo_inotify.c
static void displayInotifyEvent(struct inotify_event *i)
{
    if (i->mask & IN_CREATE){
        infof("CREATED FILE WITH NAME: %s\n",i->name);
    }        
    if (i->mask & IN_DELETE){
        infof("DELETED FOLLOWING FILE: %s\n",i->name);
    }       
    if (i->mask & IN_MOVED_FROM){
        infof("RENAMING FILE WITH NAME: %s ",i->name);
    }   
    if (i->mask & IN_MOVED_TO){
        infof("TO FILE WTIH NAME: %s\n", i->name);
    }

}

int main(int argc, char** argv){
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    struct inotify_event *ocurrence;
    ssize_t changesRead;
    char *p;
    
    int flags = 0;

    if (argc < 2){
        errorf("Wrong number of arguments");
        exit(EXIT_FAILURE);
    }

    inotifyFd = inotify_init();              
    if (inotifyFd == -1){
        errorf("inotify_init failed");
    }
    if (argc > 2 && strchr(argv[2], 'd') != NULL){
        flags |= FTW_DEPTH;
    }
    if (argc > 2 && strchr(argv[2], 'p') != NULL){
        flags |= FTW_PHYS;
    }   
    if (nftw(argv[1], display_info, 20, flags) == -1) {
        errorf("nftw");
        exit(EXIT_FAILURE);
    }
    for (;;) { 
        changesRead = read(inotifyFd, buf, BUF_LEN);
        if (changesRead == 0){
            errorf("read() from inotify fd returned 0!");
        }
        if (changesRead == -1){
            errorf("read failed");
        }
        for (p = buf; p < buf + changesRead; ) {
            ocurrence = (struct inotify_event *) p;
            displayInotifyEvent(ocurrence);

            p += sizeof(struct inotify_event) + ocurrence->len;
        }
    }
    exit(EXIT_SUCCESS);
}
