#ifndef THREADS
#define THREADS


#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <poll.h>
#include <stdlib.h>
#include <stdbool.h>
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
bool gameOver=false;
extern sem_t semUART;
extern sem_t semDraw;

void configureTable(char table[HEIGHT][WIDTH], Obstacle* obs){
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
    char start;
    int end;
    extern struct pollfd pollfds;

    while(1){

        //startbit beolvasasa
        if(poll(&pollfds,1,-1)==-1){
            write(STDERR_FILENO, "Poll error!\n", 13);
            return NULL;
        }
        if(read(tty_fd,&start,1)==-1){
            write(STDERR_FILENO, "Cannot read from UART!\n", 23);
            return NULL;
        }


        while(start==START){
            size_t x,y;
            for(int i=0;i<OBS_COUNT;i++){
                if(poll(&pollfds,1,-1)==-1){
                    write(STDERR_FILENO, "Poll error!\n", 13);
                    return NULL;
                }
                if(read(tty_fd,&x,1)==-1){
                    write(STDERR_FILENO, "Cannot read from UART!\n", 23);
                    return NULL;
                }


                if(poll(&pollfds,1,-1)==-1){
                    write(STDERR_FILENO, "Poll error!\n", 13);
                    return NULL;
                }
                if(read(tty_fd,&y,1)==-1){
                    write(STDERR_FILENO, "Cannot read from UART!\n", 23);
                    return NULL;
                }
                obs[i].x=x-48;
                obs[i].y=y-48;
            }

            configureTable(table, obs);



            sem_post(&semUART);

            while(1){
                sem_wait(&semUART);

                if(poll(&pollfds,1,-1)==-1){
                    write(STDERR_FILENO, "Poll error!\n", 13);
                    return NULL;
                }
                if(read(tty_fd, &x, 1)==-1){
                    write(STDERR_FILENO, "Cannot read from UART!\n", 23);
                    return NULL;
                }

                if(x==NEWMAP){
                    break;
                }else if(x==GAMEOVER){
                    gameOver=true;
                    sem_post(&semDraw);
                    break;
                }
                
                if(poll(&pollfds,1,-1)==-1){
                    write(STDERR_FILENO, "Poll error!\n", 13);
                    return NULL;
                }
                if(read(tty_fd, &y, 1)==-1){
                    write(STDERR_FILENO, "Cannot read from UART!\n", 23);
                    return NULL;
                }
                ship.x=x-48;
                ship.y=y-48;
                sem_post(&semDraw);

            }

            if(poll(&pollfds,1,-1)==-1){
                write(STDERR_FILENO, "Poll error!\n", 13);
                return NULL;
            }
            if(read(tty_fd, &start, 1)==-1){
                write(STDERR_FILENO, "Cannot read from UART!\n", 23);
                return NULL;
            }
            gameOver=false;
        }
    }
    write(STDERR_FILENO, "No START recieved!\n",45);
    return NULL;
}

void* draw(void* arg){
    while(1){
        sem_wait(&semDraw);
        system("clear");
        for(int i=0;i<WIDTH+2;i++){
            write(STDOUT_FILENO,"#",1);
        }
        write(STDOUT_FILENO, "\n", 1);
        
        if(gameOver){
            for(int x=0;x<HEIGHT;x++){
                write(STDOUT_FILENO, "#", 1);
                for(int j=0;j<(WIDTH-9)/2;j++){
                    write(STDOUT_FILENO, " ", 1);
                }
                write(STDOUT_FILENO, "GAME OVER", 10);
                for(int j=0;j<(WIDTH-9)/2;j++){
                    write(STDOUT_FILENO, " ", 1);
                }
                write(STDOUT_FILENO, "#\n", 2);
            }
        }
        else{
            for(size_t x=0;x<HEIGHT;x++){
                write(STDOUT_FILENO, "#", 1);
                for(size_t y=0;y<WIDTH;y++){
                    if(ship.x==x && ship.y==y){
                        char c='>';
                        write(STDOUT_FILENO, &c, 1);
                    }else{
                        write(STDOUT_FILENO, &(table[x][y]), 1);
                    }
                }
                write(STDOUT_FILENO, "#\n", 2);
            }
        }
        for(int i=0;i<WIDTH+2;i++){
            write(STDOUT_FILENO,"#",1);
        }
        write(STDOUT_FILENO, "\n", 1);

        sem_post(&semUART);
    }
    return NULL;
}

#endif