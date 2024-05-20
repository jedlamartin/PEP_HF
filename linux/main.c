
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "defines.h"


int tty_fd=-1;
sem_t semaphore;
struct pollfd pollfds;


#include "setup_tty.c"
#include "threads.c"

int main(int argc, char* argv[]){
    int opt;
    int speed=-1;
    while((opt=getopt(argc, argv, "d:"))!=-1){
        switch (opt){
        case 'd':
            //tty_fd=open(optarg, O_RDWR);
            tty_fd=open("/dev/ttyACM0", O_RDWR);
            
            if(tty_fd<0){
                write(STDERR_FILENO, "Device not found!",18);
                return 0;
            }else{
                printf("Connected to device %s.", optarg);
            }
            break;
        default:
            break;
        }
    }
    if(tty_fd<0){
        write(STDERR_FILENO, "Device not given by parameter -d!",34);
        //return 0;
    }
    tty_fd=open("/dev/ttyACM0", O_RDWR);
    setup_tty(tty_fd);
    pollfds.fd=tty_fd;
    pollfds.events=POLLIN;
    sem_init(&semaphore,0,0);
    //eloszor game start, az akadalyok koordinatai es a hajo koordinatai, utana csak a hajo koordinatai

    pthread_t uart, drawP;

    pthread_create(&uart, NULL, UART_RX, NULL);
    printf("uart fut\n");
    pthread_create(&drawP, NULL, draw, NULL);
    printf("draw fut\n");
    pthread_join(uart, NULL);
    pthread_join(drawP, NULL);




    return 0;
}