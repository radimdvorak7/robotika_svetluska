/*
bool read_qrd()
{
    bool check_sum;
    byte sum_qrd = 0;
    Serial1.write(0x80);
    //printf("Data odesla %lu \n ", millis() );
    char c = 0;
wait_for_start_sequence:
    while ( ! Serial1.available() )
        sleep(0);
    c = Serial1.read();
    //printf("start1 %lu %02hhx \n ", millis(), c );
    if(c != 0x80)
        goto wait_for_start_sequence;

    while ( ! Serial1.available() )
        sleep(0);
    c = Serial1.read();
    //printf("start2 %lu %02hhx \n ", millis(), c );
    if(c != 0x42)
        goto wait_for_start_sequence;

    while(Serial1.available() < 12)
        sleep(0);

    //printf("Data prisla, %lu,  ", millis() );


    for(int i = 0; i != 12; ++i) {
            qrd[i] = Serial1.read();
            sum_qrd = sum_qrd + qrd[i]; // tvorba checksum
        }
    byte check_byte = Serial1.read();
    if (check_byte == sum_qrd)
        check_sum = true; else  check_sum = false;
    //for(byte b = 0; b<12; b++) printf("#%u: %u ",b, qrd[b]); //kontrolni tisk
    //printf("Check_sum: %u %u \n", check_byte, sum_qrd );

    return check_sum;
}

void end_left(rb::Encoder&)
{
    end_L = true;
    Serial.println("stop L");
}

void end_right(rb::Encoder&)
{
    end_R = true;
    Serial.println("stop R");
}

void drive(int levy, int pravy) // pocet tiku enkoderu levny motor, pravy motor , vraci neujetou vzdalenost, 0 znamena, ze dojel, jak ma
{
    Driven a;
    a.left_enc = levy;
    a.right_enc = pravy;
    // do
    {   end_L = (levy == 0);
        end_R = (pravy == 0);
        //int left_enc_begin = rbc().motor(LEFT_MOTOR)>encoder()->value();
        //int right_enc_begin = rbc().motor(RIGHT_MOTOR)>encoder()->value();
        Serial.println("drive");
        bool stopped = true;
        while (! (end_L and end_R) ) {
            if  ( !( digitalRead(33) and digitalRead(5) and digitalRead(27) and digitalRead(26) and digitalRead(25) ) ) {
                Serial.println("obstacle detected");
                rbc().setMotors().stop(LEFT_MOTOR)
                                 .stop(RIGHT_MOTOR)
                                 .set();
                delay(100); // ceka, jestli souper neodjel
                stopped = true;
            }
            else if(stopped) {
                stopped = false;
                //delay(10);
                a.left_enc = levy - (rbc().motor(LEFT_MOTOR)->encoder()->value() - left_enc_begin);
                a.right_enc = pravy - (rbc().motor(RIGHT_MOTOR)->encoder()->value() - right_enc_begin);
                printf( "levy %i, pravy %i, a.left_enc %i, a.right_enc %i \n", levy, pravy , a.left_enc, a.right_enc );
                a.ok = ( ( ((levy >= 0) and (a.left_enc < 2)) or  ((levy <= 0) and (a.left_enc > 2)) ) and
                       ( ((pravy >= 0) and (a.right_enc < 2)) or  ((pravy <= 0) and (a.right_enc > 2)) ) ) ? true : false ;
                Serial.println(a.ok ? "dojeto" : "nedojeto");
                if (a.ok)
                    break;

                if(a.right_enc)
                    rbc().motor(RIGHT_MOTOR)->drive(a.right_enc, power_motor, end_right);
                if(a.left_enc)
                    rbc().motor(LEFT_MOTOR)->drive(a.left_enc, power_motor, end_left);
                // pokud zavolam rbc().motor(RIGHT_MOTOR)->drive znova driv, nez skonci, tak puvodni drive napred zavola svuj callback, (to je to end_right nebo end_left)
                // potom se ukonci a potom rozjede novy drive s novym callbackem, proto se sem musi pridat ty dva nasledujici radky
                end_L = (a.left_enc == 0);
                end_R = (a.right_enc == 0);
            }
            delay(100);
        }
        // delay(1000);
    }
    // while (!a.ok);
}

void vpred(float kolik)  // kolik - pocet ctvercu
{
    drive(kolik*ctverec, kolik*ctverec);
}

void vpravo(float kolik) // kolik - pocet otocek o 90 stupnu
{
    drive(kolik*zatoc, 0);
}

void vpravo_na_miste(float kolik) // kolik - pocet otocek o 90 stupnu
{
     drive(kolik*zatoc/2, -kolik*zatoc/2);
}

void vlevo(float kolik) // kolik - pocet otocek o 90 stupnu
{
    drive(0, kolik*zatoc);
}
*/
