#include <Arduino.h>
#include "PinChangeInt.h"
#include "Pins.h"
#include "mode.h"
#include "Command.h"
#include "BalanceCar.h"
#include "Rgb.h"
#include "voltage.h"

unsigned long start_prev_time = 0;
boolean carInitialize_en = true;

void functionMode()
{
  switch (function_mode)
  {
  case IDLE:
    break;
  case N1:
    N1MODE();
    rgb.brightBlueColor();
     break;
  }
}

void setMotionState()
{
  switch (motion_mode)
  {
  case FORWARD:
    switch (function_mode)
    {
    case N1:
      setting_car_speed = 80;
      setting_turn_speed = 4;
      break;
    default:
      setting_car_speed = 10;
      setting_turn_speed = 0;
      break;
    }
    break;
  case BACKWARD:
    switch (function_mode)
    {
    case N1:
      setting_car_speed = -20;
      setting_turn_speed = 0;
      break;
    default:
      setting_car_speed = 0;
      setting_turn_speed = 0;
      break;
    }
    break;
  case STANDBY:
    setting_car_speed = 0;
    setting_turn_speed = 0.8;
    break;
  case STOP:
    if (millis() - start_prev_time > 1000)
    {
      function_mode = IDLE;
      if (balance_angle_min <= kalmanfilter_angle && kalmanfilter_angle <= balance_angle_max)
      {
        motion_mode = STANDBY;
        rgb.lightOff();
      }
    }
    break;
  case START:
    if (millis() - start_prev_time > 2000)
    {
      if (balance_angle_min <= kalmanfilter_angle && kalmanfilter_angle <= balance_angle_max)
      {
        car_speed_integeral = 0;
        setting_car_speed = 0.8;
        motion_mode = STANDBY;
        rgb.lightOff();
      }
    }
    break;
  default:
    break;
  }
}

void keyEventHandle()
{
  if (key_value != '\0')
  {
    key_flag = key_value;

    switch (key_value)
    {
    case 's':
      rgb.lightOff();
      motion_mode = STANDBY;
      break;
    case 'f':
      rgb.flashBlueColorFront();
      motion_mode = FORWARD;
      break;
    case 'b':
      rgb.flashBlueColorback();
      motion_mode = BACKWARD;
      break;
    case '1':
      function_mode = N1;
      break;
    default:
      break;
    }
    if (key_flag == key_value)
    {
      key_value = '\0';
    }
  }
}

void setup()
{

  Serial.begin(9600);
  keyInit();
  rgb.initialize();
  voltageInit();
  start_prev_time = millis();
  carInitialize();

}
void loop()
{
  getKeyValue();
  keyEventHandle();
  voltageMeasure();
  setMotionState();
  functionMode();
  GetN1Mode();
  rgb.blink(100);
  static unsigned long print_time;
  if (millis() - print_time > 100)
  {
    print_time = millis();
    Serial.println(kalmanfilter.angle);
    rgb.brightRedColor();
     if(GetN1Mode_flag = 0)
     {
       rgb.brightRedColor();
     }
     else if(GetN1Mode_flag = 1)
     {
       rgb.brightBlueColor();
     }
     else if(GetN1Mode_flag = 2)
     {
       rgb.brightGreenColor();
     }
    Serial.println(encoder_count_right_a);
  }

  static unsigned long start_time;
  if (millis() - start_time < 10)
  {
    function_mode = IDLE;
    motion_mode = STOP;
    carStop();
  }
  if (millis() - start_time == 2000) // Enter the pendulum, the car balances...
  {
    key_value = '2';
  }
}