#ifndef THREADS
#define THREADS


#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <poll.h>
#include "defines.h"



typedef struct Ship{
    size_t x;
    size_t y;
} Ship;

typedef struct Obstacle{
    size_t x;
    size_t y;
} Obstacle;

Obstacle obs[OBS_COUNT];
Ship ship;
char table[HEIGHT][WIDTH];


void configureTable(char** table, Obstacle* obs){
    for(int x=0;x<HEIGHT;x++){
        for(int y=0;y<WIDTH;y++){
            table[x][y]=' ';
        }
    }
    for(int i=0;i<OBS_COUNT;i++){
        size_t x = obs[i].x;
        size_t y = obs[i].y;
        table[x][y]='#';
    }
}


void* UART_RX(void* arg){
    extern int tty_fd;
    int start;
    int end;
    extern struct pollfd pollfds;

    //startbit beolvasasa
    if(poll(&pollfds,1,-1)==-1){
        write(STDERR_FILENO, "Poll error!\n", 13);
        return NULL;
    }
    if(read(tty_fd,&start,1)==-1){
        write(STDERR_FILENO, "Cannot read from UART!", 23);
        return NULL;
    }


    while(start=START){
        size_t x,y;
        for(int i=0;i<OBS_COUNT;i++){
            if(poll(&pollfds,1,-1)==-1){
                write(STDERR_FILENO, "Poll error!\n", 13);
                return NULL;
            }
            if(read(tty_fd,&x,1)==-1){
                write(STDERR_FILENO, "Cannot read from UART!", 23);
                return NULL;
            }


            if(poll(&pollfds,1,-1)==-1){
                write(STDERR_FILENO, "Poll error!\n", 13);
                return NULL;
            }
            if(read(tty_fd,&y,1)==-1){
                write(STDERR_FILENO, "Cannot read from UART!", 23);
                return NULL;
            }
            obs[i].x=x;
            obs[i].y=y;
        }

        configureTable((char**)table, obs);

        ship.x=HEIGHT/2+1;
        ship.y=0;
        while(1){
            extern sem_t semaphore;
            sem_post(&semaphore);
            sem_wait(&semaphore);

            if(poll(&pollfds,1,-1)==-1){
                write(STDERR_FILENO, "Poll error!\n", 13);
                return NULL;
            }
            if(read(tty_fd, &x, 1)==-1){
                write(STDERR_FILENO, "Cannot read from UART!", 23);
                return NULL;
            }

            if(x==END){
                break;
            }
            
            if(poll(&pollfds,1,-1)==-1){
                write(STDERR_FILENO, "Poll error!\n", 13);
                return NULL;
            }
            if(read(tty_fd, &y, 1)==-1){
                write(STDERR_FILENO, "Cannot read from UART!", 23);
                return NULL;
            }
        }

        if(poll(&pollfds,1,-1)==-1){
            write(STDERR_FILENO, "Poll error!\n", 13);
            return NULL;
        }
        if(read(tty_fd, &start, 1)==-1){
            write(STDERR_FILENO, "Cannot read from UART!", 23);
            return NULL;
        }

    }

    return NULL;
}

void* draw(void* arg){
    extern sem_t semaphore;
    sem_wait(&semaphore);
    
    for(int i=0;i<WIDTH+2;i++){
        write(STDOUT_FILENO,"#",1);
    }
    write(STDOUT_FILENO, "\n", 1);
    
    for(int x=0;x<HEIGHT;x++){
        write(STDOUT_FILENO, "#", 1);
        for(int y=0;y<WIDTH;y++){
            if(ship.x==x && ship.y==y){
                char c='>';
                write(STDOUT_FILENO, &c, 1);
            }else{
                write(STDOUT_FILENO, &(table[x][y]), 1);
            }
        }
        write(STDOUT_FILENO, "#\n", 2);
        

    }
    for(int i=0;i<WIDTH+2;i++){
        write(STDOUT_FILENO,"#",1);
    }
    write(STDOUT_FILENO, "\n", 1);

    sem_post(&semaphore);

    return NULL;
}

#endif