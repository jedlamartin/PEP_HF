
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
sem_t semUART, semDraw, semScore;
struct pollfd pollfds;
struct pollfd pollIn;

#include "setup_tty.c"
#include "threads.c"

int main(int argc, char* argv[]){
    int opt;
    while((opt=getopt(argc, argv, "d:"))!=-1){
        switch (opt){
        case 'd':
            tty_fd=open(optarg, O_RDWR);
            
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
        return 0;
    }
    //tty_fd=open("/dev/ttyACM0", O_RDWR);

    setup_tty(tty_fd);
    pollfds.fd=tty_fd;
    pollIn.fd=STDIN_FILENO;
    
    pollfds.events=POLLIN;
    pollIn.events=POLLIN;

    sem_init(&semUART,0,0);
    sem_init(&semDraw,0,0);
    sem_init(&semScore,0,0);


    //eloszor game start, az akadalyok koordinatai es a hajo koordinatai, utana csak a hajo koordinatai

    pthread_t uart, drawP, scoreP;

    pthread_create(&uart, NULL, UART_RX, NULL);
    printf("uart fut\n");
    pthread_create(&drawP, NULL, draw, NULL);
    printf("draw fut\n");
    pthread_create(&scoreP, NULL, score, NULL);
    printf("score fut\n");
    //pthread_join(uart, NULL);
    //pthread_join(drawP, NULL);
    pthread_join(scoreP, NULL);

    return 0;
}