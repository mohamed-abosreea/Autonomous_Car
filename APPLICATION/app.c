/*******************************************************************************
 *                                                                             *
 * [FILE NAME]:   app.c                                                       *
 *                                                                             *
 * [AUTHOR]:      Ahmed Saeed, Amr Gafar, Kareem Abd-elrasheed,                *
 *                Mohamed Abosreea, Ahmed Maher, and Mohamed Ramadan           *
 *                                                                             *
 * [Version]:     1.0.0                                                        *
 *                                                                             *
 * [DATE]:        09/09/2023                                                   *
 *                                                                             *
 * [DESCRIPTION]: This file contain the hardware connection of project         *
 *                                                                             *
 *******************************************************************************/
#include "app.h"

#define LDR_SET_POINT_POSTIVE 500U
#define LDR_SET_POINT_NEGATIVE -100


/*Global variables*/
uint16_t LDR1_Read = 0;
uint16_t LDR2_Read = 0;
short LDR_Difference = 0;
short LDR_Difference_LCD = 0;
uint16_t obstacle_distance = 0 ;
uint8_t start_flag = 0;
uint8_t stop_flag = 0;
uint8_t counter = 0;
uint8_t obstacle_flag = 0;
uint16_t temperature_value = 0 ;
uint32_t hours , mins , secs ;
extern uint32_t seconds_count;

button_t start_switch = {
                         .btnPort =PORTF_ID,
                         .btnPin = PIN0_ID,
                         .btn_conn = PULL_UP,
                         .btn_state = RELEASED
};

button_t stop_switch = {
                        .btnPort =PORTF_ID,
                        .btnPin = PIN4_ID,
                        .btn_conn = PULL_UP,
                        .btn_state = RELEASED
};

carSide_t LEFT_SIDE  = {
                        .carSidePort = PORTB_ID,
                        .carSideDir1Pin = PIN7_ID,
                        .carSideDir2Pin = PIN1_ID,
                        .carSideEnPin = PIN6_ID,
                        .carSideSpeed = 10

};

carSide_t RIGHT_SIDE = {
                        .carSidePort = PORTB_ID,
                        .carSideDir1Pin = PIN4_ID,
                        .carSideDir2Pin = PIN5_ID,
                        .carSideEnPin = PIN0_ID,
                        .carSideSpeed = 10
};

LDR_Sensor_t LDR1 = {
                     .ldrPort = PORTE_ID,
                     .ldrPin = PIN1_ID,
                     .adcChannel = CH_1,
                     .sequencer = SS_0,
                     .sample = S_0,
                     .adc_module = ADC_0,
                     .temp_sensor_mode =NO

};

LDR_Sensor_t LDR2 = {
                     .ldrPort = PORTE_ID,
                     .ldrPin = PIN2_ID,
                     .adcChannel = CH_2,
                     .sequencer = SS_0,
                     .sample = S_0,
                     .adc_module = ADC_1,
                     .temp_sensor_mode =NO
};

Temp_Sensor_t TEMP_SENSOR = {
                             .ldrPort = PORTE_ID,
                             .ldrPin = PIN3_ID,
                             .adcChannel = CH_0,
                             .sequencer = SS_3,
                             .sample = S_0,
                             .adc_module = ADC_1,
                             .temp_sensor_mode =YES

};

ultraSonic_t Ultrasonic_Sensor = {
                                  .ultrasonicPort = PORTC_ID,
                                  .ultrasonicTriggerPin = PIN5_ID,
                                  .ultrasonicEchoPin = PIN6_ID
};

GPTM_ConfigType Stop_watch = {
                              .timer_global_operation = _16BIT_IN_16_OR_32_BIT_TIMER,
                              .timer_mode = PERIODIC,
                              .timer_number = TIMER3,
                              .timer_type = _16_OR_32_BIT_TIMER,
                              .timer_use = INDIVIDUAL_A
};

void manage_time(void)
{
    lcd_move_cursor(1,0);

    hours = seconds_count / 3600 ;
    mins = (seconds_count - 3600*hours) / 60;
    secs = seconds_count % 60 ;
    if(hours >= 10)
    {
        lcd_intger_to_string(hours);
    }else if(hours == 0)
    {
        lcd_display_string_row_column(1, 0, "00");
    }
    else{
        lcd_move_cursor(1,1);
        lcd_intger_to_string(hours);
    }

    lcd_move_cursor(1,2);
    lcd_display_character(':');
    if(mins >= 10)
    {
        lcd_move_cursor(1,3);
        lcd_intger_to_string(mins);
    }else if(mins == 0)
    {
        lcd_move_cursor(1,3);
        lcd_display_string("00");
    }
    else{
        lcd_move_cursor(1,4);
        lcd_intger_to_string(mins);
    }
    lcd_move_cursor(1,5);
    lcd_display_character(':');
    if(secs >= 10)
    {
        lcd_move_cursor(1,6);
        lcd_intger_to_string(secs);
    }else if (secs == 0)
    {
        lcd_move_cursor(1,6);
        lcd_display_string("00");
    }
    else {
        lcd_move_cursor(1,7);
        lcd_intger_to_string(secs);
    }
}

void stopwatch_init(void)
{

    timer_init(&Stop_watch, 1000);
}


void system_init(void)
{
    /*initialization of lcd */
    lcd_init();

    /*car side configuration initialization*/
    car_side_init(&LEFT_SIDE , &RIGHT_SIDE);
    /*LDR init*/
    ldr_sensor_init(&LDR1);
    ldr_sensor_init(&LDR2);
    /*TEMP_Sensor init*/
    temperature_sensor_init(&TEMP_SENSOR);
    /*Ultrasonic init*/
    ultarsonic_sensor_init(&Ultrasonic_Sensor);
    /*start switch init*/
    button_init(&start_switch);
    /*stop switch init*/
    button_init(&stop_switch);
    stopwatch_init();
}
void robot_move_forward(void)
{
    car_side_direction_set(&LEFT_SIDE , forward);
    car_side_direction_set(&RIGHT_SIDE , forward);
}
void robot_move_back(void)
{
    car_side_direction_set(&LEFT_SIDE , backward);
    car_side_direction_set(&RIGHT_SIDE , backward);
}
void robot_move_stop(void)
{
    car_side_direction_set(&LEFT_SIDE , stop);
    car_side_direction_set(&RIGHT_SIDE , stop);
}
void robot_move_left(void)
{
    car_side_direction_set(&LEFT_SIDE , forward);
    car_side_direction_set(&RIGHT_SIDE , stop);

}
void robot_move_right(void)
{
    car_side_direction_set(&LEFT_SIDE , stop);
    car_side_direction_set(&RIGHT_SIDE , forward);
}
void robot_move_rotate(void)
{
    car_side_direction_set(&LEFT_SIDE , backward);
    car_side_direction_set(&RIGHT_SIDE , forward);
    _delay_ms(350);
    car_side_direction_set(&LEFT_SIDE , stop);
    car_side_direction_set(&RIGHT_SIDE , stop);
}

void switches_update(void)
{
    static buttonState_t current_start_switch_state , last_start_switch_state = RELEASED;
    static buttonState_t current_stop_switch_state , last_stop_switch_state = RELEASED;

    button_get_state(&start_switch, &current_start_switch_state);
    button_get_state(&stop_switch, &current_stop_switch_state);

    /****************** Start operation************************/
    /*when both current and last are equal do nothing*/
    if(current_start_switch_state == PRESSED && last_start_switch_state == RELEASED)
    {
        start_flag = 1 ;
        stop_flag = 0 ;
        /*reset second counter*/
        seconds_count = 0;
    }
    else /*Button_currentState == RELEASED && Button_lastState == PRESSED*/
    {
        /*then we are in the pre-release state */
        /*make the last stated = pressed*/
        last_start_switch_state = current_start_switch_state;
    }
    /****************** Stop operation************************/
    if(seconds_count >= 60 || ( current_stop_switch_state == PRESSED && last_stop_switch_state == RELEASED))
    {
        start_flag = 0U ;
        stop_flag = 1U ;
    }
    else /*Button_currentState == RELEASED && Button_lastState == PRESSED*/
    {
        /*then we are in the pre-release state */
        /*make the last stated = pressed*/
        last_stop_switch_state = current_stop_switch_state;
    }
}
void lcd_update(void)
{
    if(start_flag == 1U)
    {
        /*Set cursor position to ROW_0 and COL_0*/
        lcd_move_cursor(0,0);
        /*Displaying Temperature sensor value*/
        lcd_display_string("Temp = ");
        lcd_intger_to_string(temperature_value);
        lcd_display_string(" C");
        /*Displaying Time */
        manage_time();
        /*clear this part od lcd*/
        lcd_move_cursor(1,10);
        lcd_display_string("     ");
        /*Displaying LDR difference value*/
        lcd_move_cursor(1,10);
        lcd_intger_to_string(LDR_Difference_LCD);
    }
    else{
        /*Do Nothing*/
    }
}
void ultrasonic_update(void)
{

    if(start_flag == 1U)
    {
        /*get distance*/
        obstacle_distance = ultarsonic_sensor_read_value(&Ultrasonic_Sensor);
        /*check the car is blocked*/
        if((10U < obstacle_distance) || (0U == obstacle_distance ))
        {
            robot_move_forward();
        }
        else
        {
            robot_move_back();
            _delay_ms(200);
            robot_move_rotate();
            //counter++;
            /*state machine to determine what will we do now depending on time slots (counter)*/
            //            switch(counter)
            //            {
            //
            //            case 1:
            //            {
            //                if(obstacle_flag == 0){
            //                    obstacle_flag = 1;
            //                    counter = 0;
            //                }else{
            //                    /*go to state of  rotation*/
            //                    counter++;
            //                    obstacle_flag = 0;
            //                }
            //            }
            //            break;
            //            case 7: /* (7-1)*50 = 300 ms*/
            //            {
            //                counter = 0;
            //            }break;
            //            default:
            //                robot_move_rotate();
            //                counter++;
            //                break;
            //
            //            }






        }
    }else{
        /*Do Nothing*/
    }
}
void ldr_update(void)
{
    if(start_flag == 1U)
    {
        LDR1_Read = ldr_sensor_reading(&LDR1);
        LDR2_Read = ldr_sensor_reading(&LDR2);

        LDR_Difference = LDR1_Read - LDR2_Read;
        LDR_Difference_LCD = LDR_Difference;
        /*check LDRs value*/
        if( LDR_SET_POINT_POSTIVE > LDR_Difference)
        {
            robot_move_forward();
        }
        else
        {

            if(LDR_SET_POINT_POSTIVE > LDR_Difference)
            {
                robot_move_left();
            }
            else
            {
                robot_move_right();
            }
            LDR_Difference=0U;

        }
    }else{
        /*Do Nothing*/
    }
}

void robot_state(void)
{
    if(stop_flag == 1U)
    {
        robot_move_stop();
    }else{
        /*Do Nothing*/
    }
}
void temperature_update(void)
{
    if(start_flag == 1U)
    {
        temperature_value = temperature_sensor_reading(&TEMP_SENSOR);
        temperature_value = (147.5 - ((75*3.3*temperature_value)/4096));
    }else{
        /*Do Nothing*/
    }
}
