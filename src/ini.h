#include <atomic>


#define RIGHT_MOTOR  1
#define LEFT_MOTOR  0

byte qrd[12];  // pole, do ktereho se nacitaji okamzite hodnoty QRD senzoru
byte qrd_prumer[12];
std::atomic_bool end_L;
std::atomic_bool end_R;

float poloha_0 = 80;
float poloha_1 = 120;
float rychlost_0 = 0;
float rychlost_1 = 0;
const int AXIS_COUNT = 7;
const int BTN_COUNT = 12;
int axis[AXIS_COUNT] = {5,6,7,8,9,10,11};
byte btn[BTN_COUNT] = {0,0,0,0,0,0,0,0,0,0,0,0};
byte btn_last[BTN_COUNT] = {0,0,0,0,0,0,0,0,0,0,0,0};
int speed_coef = 50; // nasobeni hodnoty, co leze z joysticku
int diff_coef = 0; // o kolik jede jeden motor rychleji nez druhy

int power_motor = 80;
int otacka = 235; // pocet tiku na otacku - nevim jiste
int ctverec = 482; // pocet tiku na ctverec - Praha
int zatoc = 945;  // pocet tiku na zatoceni o 90 stupnu
//static const uint32_t i2c_freq = 400000;
bool L_G_light = false; // pro blikani zelene LED - indikuje, ze deska funguje

struct Driven {
    int left_enc;
    int right_enc;
    bool ok;
};



