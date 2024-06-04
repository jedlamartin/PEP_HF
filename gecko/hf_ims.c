/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <em_gpio.h>
#include <em_usart.h>
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "cmsis_compiler.h"             // Compiler agnostic definitions
#include "os_tick.h"

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#define max_x 10  //sor
#define max_y 30  //oszlop
#define obs_count 75
#define orig_step_time 1000 //ms
#define speed_up_rate 1.2   //speed gets multiplied by this at every new map

#define START 99
#define NEWMAP 98
#define GAMEOVER 97

/***************************************************************************//**
 * Global variables
 ******************************************************************************/
TaskHandle_t HandleButtonL;
TaskHandle_t HandleButtonR;
TaskHandle_t HandleShip;
TimerHandle_t HandleTimer;

enum stat
{
  initial, running, gameover
} state = initial;

enum direction
{
  down, straight, up
} dir = straight;

struct ship
{
  int x;
  int y;
} myship =
  { 5, -1 };

struct obs
{
  int x;
  int y;
};

struct obs obstacles[obs_count];

int step_time = orig_step_time; //ms

void
get_obstacles (void)
{
  for (int i = 0; i < obs_count; i++)
    {
      bool duplicate = true;
      while (duplicate)
        {
          duplicate = false;
          obstacles[i].x = rand () % (max_x + 1);
          obstacles[i].y = rand () % max_y + 1; //0. oszlopba ne tegyünk, mert ott lesz a hajó
          int j = 0;
          while (j < i && !duplicate)
            {
              if (obstacles[j].x == obstacles[i].x
                  && obstacles[j].y == obstacles[i].y)
                {
                  duplicate = true;
                }
              j++;
            }
        }
      printf ("%c", obstacles[i].x + 48);
      printf ("%c", obstacles[i].y + 48);
    }
}

void
reset_game (void)
{
  printf ("%c", START);
  get_obstacles ();
  myship.x = 5;
  myship.y = -1;
  dir = straight;
  state = running;
  step_time = orig_step_time;
  xTimerChangePeriod(HandleTimer, step_time, 0);
}

/***************************************************************************//**
 * FreeRTOS tasks.
 ******************************************************************************/
static void
prvTaskButtonL (void *pvParam __attribute__((unused)))
{
  while (1)
    {
      vTaskSuspend (NULL);
      if (state == initial || state == gameover)
        {
          reset_game ();
        }
      else if (state == running)
        if (dir < up)
          dir++;
    }
}

static void
prvTaskButtonR (void *pvParam __attribute__((unused)))
{
  while (1)
    {
      vTaskSuspend (NULL);
      if (state == initial || state == gameover)
        {
          reset_game ();
        }
      else if (dir > down)
        dir--;
    }
}

static void
prvTaskShip (void *pvParam __attribute__((unused)))
{
  while (1)
    {
      if (dir == up)
        {
          if (myship.x > 0)
            myship.x--;
          else
            myship.x = max_x;
        }
      else if (dir == down)
        {
          if (myship.x < max_x)
            {
              myship.x++;
            }
          else
            {
              myship.x = 0;
            }
        }
      else if (dir == straight)
        {
          if (myship.y < max_y)
            myship.y++;
          else
            {
              printf ("%c", NEWMAP);
              printf ("%c", START);
              get_obstacles ();
              myship.y = 0;
              step_time = step_time / speed_up_rate;
              xTimerChangePeriod(HandleTimer, step_time, 0);
            }
        }

      printf ("%c", myship.x + 48);
      printf ("%c", myship.y + 48);
      for (int i = 0; i < obs_count; i++)
        {
          if (myship.x == obstacles[i].x && myship.y == obstacles[i].y)
            {
              printf ("%c", GAMEOVER);
              state = gameover;
              break;
            }
        }
      vTaskSuspend (NULL);
    }
}

/***************************************************************************//**
 * Timer callback
 ******************************************************************************/
void
timerCallback (TimerHandle_t t __attribute__((unused)))
{
  if (state == running)
    vTaskResume (HandleShip);
}

/***************************************************************************//**
 * ISRs
 ******************************************************************************/
void
GPIO_ODD_IRQHandler (void)
{
  BaseType_t switchReq;

  switchReq = xTaskResumeFromISR (HandleButtonL);
  GPIO_IntClear (1 << 9);
  portYIELD_FROM_ISR(switchReq);
}

void
GPIO_EVEN_IRQHandler (void)
{
  BaseType_t switchReq;

  switchReq = xTaskResumeFromISR (HandleButtonR);
  GPIO_IntClear (1 << 10);
  portYIELD_FROM_ISR(switchReq);
}

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void
app_init (void)
{
  GPIO_PinModeSet (gpioPortB, 9, gpioModeInput, 0);
  GPIO_ExtIntConfig (gpioPortB, 9, 9, false, true, true);
  GPIO_PinModeSet (gpioPortB, 10, gpioModeInput, 0);
  GPIO_ExtIntConfig (gpioPortB, 10, 10, false, true, true);
  NVIC_EnableIRQ (GPIO_ODD_IRQn);
  NVIC_EnableIRQ (GPIO_EVEN_IRQn);

  srand ((int) osKernelGetSysTimerCount);     //random generátor inicializálás

  HandleTimer = xTimerCreate ("Timer", step_time, pdTRUE, NULL, timerCallback);
  xTimerStart(HandleTimer, 0);

  xTaskCreate (prvTaskButtonL, "ButtonL",
  configMINIMAL_STACK_SIZE,
               NULL,
               tskIDLE_PRIORITY + 1,
               &HandleButtonL);

  xTaskCreate (prvTaskButtonR, "ButtonR",
  configMINIMAL_STACK_SIZE,
               NULL,
               tskIDLE_PRIORITY + 1,
               &HandleButtonR);

  xTaskCreate (prvTaskShip, "Ship",
  configMINIMAL_STACK_SIZE,
               NULL,
               tskIDLE_PRIORITY + 2,
               &HandleShip);

  vTaskSuspend (HandleShip);
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void
app_process_action (void)
{

}
