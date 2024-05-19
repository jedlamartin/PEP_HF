
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include "setup_tty.c"

int main(int argc, char* argv[]){
    int opt;
    int tty_fd=-1;
    int speed=-1;
    while((opt=getopt(argc, argv, "d:"))!=-1){
        switch (opt){
        case 'd':
            tty_fd=open(optarg, O_RDONLY);
            if(tty_fd<0){
                printf("Specify the correct device path with -d!");
                return 0;
            }else{
                printf("Connected to device %s.", optarg);
            }
            break;
        default:
            break;
        }
    }
setup_tty(tty_fd);




    return 0;
}