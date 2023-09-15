/*******************************************************************************
 *                                                                             *
 * [FILE NAME]:   main.c                                                       *
 *                                                                             *
 * [AUTHOR]:      Ahmed Saeed, Amr Gafar, Kareem Abd-elrasheed,                *
 *                Mohamed Abosreea, Ahmed Maher, and Mohamed Ramadan           *
 *                                                                             *
 * [Version]:     1.0.0                                                        *
 *                                                                             *
 * [DATE]:        09/09/2023                                                   *
 *                                                                             *
 * [DESCRIPTION]:                                                              *
 *                                                                             *
 *******************************************************************************/
#include "APPLICATION/app.h"

void main(void)
{
    /*initialization the system*/
    system_init();

    /*create the tasks*/
    create_task(switches_update, 1, 0);
    create_task(lcd_update, 5, 0);
    create_task(ldr_update, 2, 0);
    create_task(temperature_update, 10, 0);
    create_task(robot_state, 1, 0);
    create_task(ultrasonic_update, 1 , 0);

    /*initialization of the schedular with tick time 50 ms*/
    init_scheduler(50);

    tasks_scheduler();

    while(1){}

}
