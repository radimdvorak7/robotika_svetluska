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

//void start();
//void testovani_serv();
//void testovani_motoru();
bool read_joystick(); // definice dole pod hlavnim programem

//void bateri_unloading(int bateri_number);
//oid ultrasounic_pinmode();
//float read_ultrasounic();

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
 //     testovani_serv();  // pozor, kdyz je zapnuto, rusi ostatni ovladani serv
 //void testovani_motoru();

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
            /*rbc().servoBus().set(0, rb::Angle::deg(35), 300.f, 1.5f);
            rbc().servoBus().set(1, rb::Angle::deg(30), 300.f, 1.5f);
            poloha_0 = 35;
            poloha_1 = 175;*/
        }
        if (axis[5] < -100 ) // pouze zapnuto/vypnuto
        {
            SerialBT.print("osa 5 klesa");
            Serial.print("osa 5 klesa");
            /*rbc().servoBus().set(0, rb::Angle::deg(100), 300.f, 1.5f);
            rbc().servoBus().set(1, rb::Angle::deg(50), 300.f, 1.5f);
            poloha_0 = 100;
            poloha_1 = 110;*/
        }
        float rychlost = (-axis[2]+129)/128.0;
        if (btn[5] == 1 )
            rychlost_0 = rychlost;
        else if (btn[6] == 1 )
             rychlost_0 = -rychlost;
        else
            rychlost_0 =0;

        if (btn[8] == 1 )
            rychlost_1 = rychlost;
        else if (btn[7] == 1 )
             rychlost_1 = -rychlost;
        else
            rychlost_1 =0;

        //Serial.print(poloha_0); Serial.print(" ");
        //Serial.print(poloha_1); Serial.print(" ");

    }



    if(btn[9])
    {
      SerialBT.print(levy_m); SerialBT.print(" "); SerialBT.println(pravy_m);
      Serial.print(levy_m); Serial.print(" "); Serial.println(pravy_m);
        //bateri_unloading(0);
    }
    if(btn[10])
    {
        //bateri_unloading(1);
    }
    if(btn[11])
    {
        //bateri_unloading(2);
    }
    if(btn[12])
    {
        //bateri_unloading(3);
    }
    //rbc().servoBus().set(0, poloha_rb::Angle::deg(0), 200.f, 1.5f);
    //rbc().servoBus().set(1, rb::Angle::deg(poloha_1), 200.f, 1.5f);
    delay(10);  
}

// ************************ definice, ktere jinde nefunguji

/*void start()
{
    vpred(1.1);
    Serial.println("vlevo");
    vlevo(1.1);
    Serial.println("vpred1");
    for (int x = 0; x != 4; ++x) {
        vpred(1);
    }
    Serial.println("vpred2");
    vpred(0.5);
    Serial.println("hotovo");
}
*/
// ********************************************************************

/*bool read_joystick()
 {
     if ( SerialBT.available() == 0 )
         return false;

     int test = SerialBT.read();
     if (test == 0x80)
     {
         int axis_count = SerialBT.read();
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
             SerialBT.print(" ");

         }
         return true;
     }
     else if  ( test == 0x81 )
     {
         while(SerialBT.available() < 1) {
             // DO NOTHING - WAITING FOR PACKET
             delay(1);
         }
         byte a = SerialBT.read();
         while(SerialBT.available() < 1) {
             // DO NOTHING - WAITING FOR PACKET
             delay(1);
         }
         btn_last[a] = btn[a];
         btn[a] = SerialBT.read();
         Serial.print(a, DEC); Serial.print(": "); Serial.print(btn[a], DEC); Serial.print("last: "); Serial.print(btn_last[a], DEC);
         return true;
     }
     return false;
 }
*/
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

// //******************************************
/*void testovani_serv()
{

     if(Serial.available()) {
        char c = Serial.read();
        switch(c) {
            case 'w':
                poloha_0++;
                Serial.print("poloha_0  ");
                Serial.println(poloha_0);
                break;
            case 's':
                poloha_0--;
                Serial.print("poloha_0  ");
                Serial.println(poloha_0);
                break;
            case 'a':
                poloha_1++;
                Serial.print("poloha_1  ");
                Serial.println(poloha_1);
                break;
            case 'd':
                poloha_1--;
                Serial.print("poloha_1  ");
                Serial.println(poloha_1);
                break;
            default:
                Serial.write(c);
        }
    }
    //rbc().servoBus().set(0,rb::Angle::deg(poloha_0),100.f,1.5f); // stav 0 = 25/130
    //rbc().servoBus().set(1,rb::Angle::deg(poloha_0),180.f,1.5f); // stav 1 = 180/80
}
*//*
void testovani_motoru()
{
   if(Serial.available()) {
        char c = Serial.read();
        switch(c) {
            case 'w':
                 driver.solo_power(power_motor,LEFT_MOTOR);
                 driver.solo_power(power_motor,RIGHT_MOTOR);
       
                break;
            case 's':
                 driver.solo_power(-power_motor,LEFT_MOTOR);
                 driver.solo_power(-power_motor,RIGHT_MOTOR);
                break;
            case 'a':
                 driver.solo_power(-power_motor,LEFT_MOTOR);
                 driver.solo_power(power_motor,RIGHT_MOTOR);
                break;
            case 'd':
                 driver.solo_power(power_motor,LEFT_MOTOR);
                 driver.solo_power(-power_motor,RIGHT_MOTOR);
                break;
            case '*':
                c = '0' + 10;
            case '0' ... '9':
                power_motor = (c - '0') * 10;
                Serial.println(power_motor);
                break;
                //rbc().motor(LEFT_MOTOR)->drive(otacka * (c - '0'), 64, nullptr);
                //rbc().motor(RIGHT_MOTOR)->drive(otacka * (c - '0'), 64, nullptr); //  tik; na otacku
                break;

            case ' ':
                 driver.solo_power(0,LEFT_MOTOR);
                 driver.solo_power(0,RIGHT_MOTOR);
                break;

            case 'i':// vpred(1);
                break;
            case 'k':// vpravo(1);
                break;
            case 'm': //vpred(-1);
                break;
            case 'j': //vlevo(1);
                break;
            case 'p': //vpravo_na_miste(1);
                break;

            default:
                Serial.write(c);
                break;
        }
    }


}*/
/*
void bateri_unloading(int bateri_number)
{
    if (bateri_number == 0)
    {
        rbc().setMotors().power(RIGHT_MOTOR, 60)
                         .power(LEFT_MOTOR, 60)
                         .set();
        delay(1500);
        rbc().motor(LEFT_MOTOR )->drive(-otacka*0.5, 60, nullptr);
        rbc().motor(RIGHT_MOTOR)->drive(-otacka*0.5, 60, nullptr);
        delay(1000);
        while (read_ultrasounic()<13)
        {
            rbc().motor(LEFT_MOTOR )->drive(-otacka*0.04547, 40, nullptr);
            rbc().motor(RIGHT_MOTOR)->drive(-otacka*0.04547, 40, nullptr);
            delay(50);
        }
        while(read_ultrasounic()>13.5)
        {
            rbc().motor(LEFT_MOTOR )->drive(otacka*0.04547, 40, nullptr);
            rbc().motor(RIGHT_MOTOR)->drive(otacka*0.04547, 40, nullptr);
            delay(50);
        }
        rbc().servoBus().set(1,rb::Angle::deg(175),180.f,1.5f);
        delay(1000);
        rbc().motor(LEFT_MOTOR)->drive(-otacka * 0.5, 50, nullptr);
        rbc().motor(RIGHT_MOTOR)->drive(-otacka * 0.5, 50, nullptr);
        delay(1000);
        rbc().servoBus().set(1,rb::Angle::deg(35),180.f,1.5f);
        delay(200);
    }
    else if (bateri_number == 1)
    {
        rbc().setMotors().power(RIGHT_MOTOR, 60)
                         .power(LEFT_MOTOR, 60)
                         .set();
        delay(1500);
        rbc().motor(LEFT_MOTOR )->drive(-otacka*0.5, 60, nullptr);
        rbc().motor(RIGHT_MOTOR)->drive(-otacka*0.5, 60, nullptr);
        delay(1000);
        while (read_ultrasounic()<4.3)
        {
            rbc().motor(LEFT_MOTOR )->drive(-otacka*0.04547, 50, nullptr);
            rbc().motor(RIGHT_MOTOR)->drive(-otacka*0.04547, 50, nullptr);
            delay(50);
        }
        while(read_ultrasounic()>4.5)
        {
            rbc().motor(LEFT_MOTOR )->drive(otacka*0.04547, 50, nullptr);
            rbc().motor(RIGHT_MOTOR)->drive(otacka*0.04547, 50, nullptr);
            delay(50);
        }
        rbc().servoBus().set(1,rb::Angle::deg(140),100.f,15.0f);
        delay(1000);
        rbc().servoBus().set(1,rb::Angle::deg(75),100.f,15.0f);
        rbc().motor(RIGHT_MOTOR)->drive(-otacka * 1.5 , 50, nullptr);
        rbc().motor(LEFT_MOTOR )->drive(-otacka * 1.5 , 50, nullptr);
        delay(1200);
        rbc().servoBus().set(1,rb::Angle::deg(35),180.f,1.5f);
    }
    else if (bateri_number == 2)
    {
        rbc().setMotors().power(RIGHT_MOTOR, 60)
                         .power(LEFT_MOTOR, 60)
                         .set();
        delay(1500);
        rbc().motor(LEFT_MOTOR )->drive(-otacka*0.5, 60, nullptr);
        rbc().motor(RIGHT_MOTOR)->drive(-otacka*0.5, 60, nullptr);
        delay(1000);
        while (read_ultrasounic()<4.3)
        {
            rbc().motor(LEFT_MOTOR )->drive(-otacka*0.04547, 50, nullptr);
            rbc().motor(RIGHT_MOTOR)->drive(-otacka*0.04547, 50, nullptr);
            delay(50);
        }
        while(read_ultrasounic()>4.5)
        {
            rbc().motor(LEFT_MOTOR )->drive(otacka*0.04547, 50, nullptr);
            rbc().motor(RIGHT_MOTOR)->drive(otacka*0.04547, 50, nullptr);
            delay(50);
        }
        rbc().servoBus().set(0,rb::Angle::deg(35),100.f,15.0f);
        delay(1000);
        rbc().servoBus().set(0,rb::Angle::deg(100),100.f,15.0f);
        rbc().motor(RIGHT_MOTOR)->drive(-otacka * 1.5 , 50, nullptr);
        rbc().motor(LEFT_MOTOR )->drive(-otacka * 1.5 , 50, nullptr);
        delay(1000);
        rbc().servoBus().set(0,rb::Angle::deg(30),180.f,1.5f);
    }
    else if (bateri_number == 3)
    {
        rbc().setMotors().power(RIGHT_MOTOR, 60)
                         .power(LEFT_MOTOR, 60)
                         .set();
        delay(1500);
        rbc().motor(LEFT_MOTOR )->drive(-otacka*0.5, 60, nullptr);
        rbc().motor(RIGHT_MOTOR)->drive(-otacka*0.5, 60, nullptr);
        delay(1000);
        while (read_ultrasounic()<4.3)
        {
            rbc().motor(LEFT_MOTOR )->drive(-otacka*0.04547, 50, nullptr);
            rbc().motor(RIGHT_MOTOR)->drive(-otacka*0.04547, 50, nullptr);
            delay(50);
        }
        while(read_ultrasounic()>4.5)
        {
            rbc().motor(LEFT_MOTOR )->drive(otacka*0.04547, 50, nullptr);
            rbc().motor(RIGHT_MOTOR)->drive(otacka*0.04547, 50, nullptr);
            delay(50);
        }
        rbc().servoBus().set(0,rb::Angle::deg(70),100.f,15.0f);
        delay(1000);
        rbc().servoBus().set(0,rb::Angle::deg(135),100.f,15.0f);
        rbc().motor(RIGHT_MOTOR)->drive(-otacka * 1.5 , 50, nullptr);
        rbc().motor(LEFT_MOTOR )->drive(-otacka * 1.5 , 50, nullptr);
        delay(1200);
        rbc().servoBus().set(1,rb::Angle::deg(175),180.f,1.5f);
    }
}

void ultrasounic_pinmode()
{
    pinMode(Echo, INPUT);
    pinMode(Trig, OUTPUT);
}

float read_ultrasounic()
{
    digitalWrite(Trig, LOW);
    delayMicroseconds(2);
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW); // Spočítá vzdálenost
    float distance = pulseIn(Echo, HIGH);
    distance = distance*0.017315f; // odešle informace na sérivý port
    Serial.print(distance);
    Serial.print("cm\n");
    delay(200);
    return distance;
}
*/
