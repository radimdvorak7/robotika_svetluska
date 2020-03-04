#include <Arduino.h>
#include "RB3202_lbr.hpp"
//#include <Servo.h>
#include <Wire.h>
#include "time.hpp"
#include <stdint.h>
#include "stopwatch.hpp"
#include "nvs_flash.h"
#include "BluetoothSerial.h"

#include "ini.h"
#include "function.h"

int levy_m;
int pravy_m;

RB3202_PID pid;

bool read_joystick(); // definice dole pod hlavnim programem


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
Stream* serial = nullptr;
RB3202_driver driver;

float speed = 0;
void fc()
{
  digitalWrite(RB3202::LED_G, HIGH);
}

void setup() 
{
  Serial.begin(115200);
  rb_periphery::sed_periphery();
  //pid.motor_go_position(0,1000,100,*fc,70,480);
  //delay(10000);
  if (!SerialBT.begin("Svetluska")) 
    {                                
        Serial.println("!!! Bluetooth initialization failed!");
        serial = &Serial;
    }
    else
    {
        serial = &SerialBT;
        SerialBT.println("!!! Bluetooth work!");
        Serial.println("!!! Bluetooth work!");
    }

    driver.set_motor();
}

timeout send_data { msec(500) }; // timeout zajistuje posilani dat do PC kazdych 500 ms

void loop()
{

    if (send_data) {
        send_data.ack();
        if (L_G_light) L_G_light = false; else  L_G_light = true;
        SerialBT.println(millis());
    }
 

 if(!digitalRead(RB3202::SW0))
  {
    speed += 0.1;
    Serial.println(speed);
  }
  else if(!digitalRead(RB3202::SW1))
  {
    speed ++;
    Serial.println(speed);
  }
  else if(!digitalRead(RB3202::SW2))
  {
    speed -= 0.1;
    Serial.println(speed);
  }
  else if(!digitalRead(RB3202::SW3))
  {
    speed --;
    Serial.println(speed);
  }
  pid.set_rotate(speed,speed);
  

 if ( read_joystick() ) {
        float axis_0 = (abs(axis[0]) < 3) ? 0 : axis[0] /128.0;
        axis_0 = axis_0*axis_0*axis_0;
        float axis_1 = (abs(axis[1]) < 3) ? 0 : axis[1] /128.0;
        axis_1 = axis_1*axis_1*axis_1;
        int levy_m = (axis_1 - (axis_0 /2 )) * speed_coef;
        int pravy_m = (axis_1 + (axis_0 /2 )) * speed_coef;
        printf(" %i %i \n ", levy_m, pravy_m );
        
        driver.solo_power(levy_m,LEFT_MOTOR);
        driver.solo_power(pravy_m,RIGHT_MOTOR);
       

        if (axis[5] > 100 ) // pouze zapnuto/vypnuto
        {
            SerialBT.print("osa 5 roste");
            Serial.print("osa 5 roste");
            
        }
        if (axis[5] < -100 ) // pouze zapnuto/vypnuto
        {
            SerialBT.print("osa 5 klesa");
            Serial.print("osa 5 klesa");
            
        }
        float rychlost = (-axis[2]+129)/128.0;
        if (btn[5] == 1 )
            ;
        else if (btn[6] == 1 )
             ;
        else
            ;

        if (btn[8] == 1 )
            ;
        else if (btn[7] == 1 )
            ;
        else
            ;

    }

    if(btn[9])
    {
      
    }
    if(btn[10])
    {
      
    }
    if(btn[11])
    {
      
    }
    if(btn[12])
    {
    
    }
    
  
}


bool read_joystick()
{
    if ( SerialBT.available() == 0 )
        return false;

    int test = SerialBT.read();
    if (test == 0x80)
    {
        while(SerialBT.available() < 1)
                {
                    // DO NOTHING - WAITING FOR PACKET
                    delay(1);
                }
        int axis_count = SerialBT.read();
        //Serial.print("axis_count: "); Serial.println(axis_count);
        if (axis_count >= AXIS_COUNT)
        {
            Serial.println("********* CHYBA V POCTU OS !!! ************");
        }
        else
        {
            for (int x = 0; x < axis_count; x++)
            {
                while(SerialBT.available() < 1)
                {
                    // DO NOTHING - WAITING FOR PACKET
                    delay(1);
                }

                int8_t tmp = SerialBT.read();
                axis[x] = tmp;
                Serial.print(x);
                Serial.print(": ");
                Serial.print(axis[x], DEC);
                Serial.print(" ");
                SerialBT.print(x);
                SerialBT.print(": ");
                SerialBT.print(axis[x], DEC);


            }
            SerialBT.println(" ");
            return true;
        }


    }
    else if  ( test == 0x81 )
    {
        while(SerialBT.available() < 1) {
            // DO NOTHING - WAITING FOR PACKET
            delay(1);
        }
        byte a = SerialBT.read();
        if ( a >= BTN_COUNT )
        {
            Serial.println("********* CHYBA V POCTU TLACITEK !!! ************");
        }
        else
        {
            while(SerialBT.available() < 1) {
                // DO NOTHING - WAITING FOR PACKET
                delay(1);
            }
            btn_last[a] = btn[a];
            btn[a] = SerialBT.read();
            Serial.print(a, DEC); Serial.print(": "); Serial.print(btn[a], DEC); Serial.print("last: "); Serial.print(btn_last[a], DEC);
            return true;
        }

    }
    return false;
}
