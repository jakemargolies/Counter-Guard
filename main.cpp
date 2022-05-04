#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(p28,p27,p30); // serial tx, serial rx, reset pin;
InterruptIn motion_detector(p12);
DigitalOut led(LED1);
DigitalOut sprayer(p11);
DigitalOut flash(LED4);
Serial blue(p9,p10);
InterruptIn battery_indicator(p13);

volatile bool armed;
volatile bool armable;
volatile bool button_ready = 0;
volatile char bnum=0;
volatile char bhit=0;

//state used to remember previous characters read in a button message
enum statetype {start = 0, got_exclm, got_B, got_num, got_hit};
statetype state = start;

void arm_system(){
    if(armable){
        uLCD.filled_rectangle(10,25,120,55, RED);
        uLCD.textbackground_color(RED);
        uLCD.locate(2,2);
        uLCD.printf("ARMED");
        armed = 1;
    }
    else{
        blue.puts("\n\rBattery Low, charge to arm");
    }
}

void disarm_system(){
    uLCD.filled_rectangle(10,25,120,55, GREEN);
    uLCD.textbackground_color(GREEN);
    uLCD.locate(3,2);
    uLCD.printf("SAFE");
    armed = 0;
}
 
void attack() {
    if (armed) {
        led = 1;
        sprayer = 1;
        wait(2);
        sprayer = 0;
        wait(0.2);
        sprayer = 1;
        led = 0;
        wait(1);
        sprayer = 0;
    }
}

//Interrupt routine when motor battery dies
void battery_off(){
    disarm_system();
    armable = 0;
    blue.puts("\n\rBattery Low, charge to arm");
}

//Interrupt routine for when battery gets charged 
void battery_charged(){
    armable = 0;
    blue.puts("\n\rBattery has charge, system armable");
}
    
//Interrupt routine to parse message with one new character per serial RX interrupt
void parse_message()
{
    switch (state) {
        case start:
            if (blue.getc()=='!') state = got_exclm;
            else state = start;
            break;
        case got_exclm:
            if (blue.getc() == 'B') state = got_B;
            else state = start;
            break;
        case got_B:
            bnum = blue.getc();
            state = got_num;
            break;
        case got_num:
            bhit = blue.getc();
            state = got_hit;
            break;
        case got_hit:
            if (blue.getc() == char(~('!' + ' B' + bnum + bhit))) button_ready = 1;
            state = start;
            break;
        default:
            blue.getc();
            state = start;
    }
}
 
int main() {
    armed = 0;
    armable = 0;
    sprayer = 0;
    
    motion_detector.mode(PullUp);
    battery_indicator.mode(PullUp);
    
    wait(2); // Wait for motion detector to stabilize
    
    // Setup Interrupts
    motion_detector.fall(&attack);
    battery_indicator.fall(&battery_off);
    battery_indicator.rise(&battery_charged);
    blue.attach(&parse_message,Serial::RxIrq);
    
    // Setup for printing to LCD
    uLCD.color(WHITE);
    uLCD.text_height(2);
    uLCD.text_width(2);
    
    while(1) {           // wait around, interrupts will interrupt
        flash = !flash;
        wait(0.2);
        if (button_ready) {
            if(bnum == '5' && bhit == '1'){
                arm_system();
                blue.puts("\n\rSystem Armed");
            }
            else if(bnum == '6' && bhit == '1'){
                disarm_system();
                blue.puts("\n\rSystem Disarmed");
            }
            button_ready = 0;
        }
    }
}