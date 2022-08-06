#include <stdlib.h> 
#include <stdio.h>
#include <wiringPi.h> 
#include <pcf8574.h> 
#include <lcd.h> 
#include <time.h> 
 
//#define pcf8574_address 0x27 // default I2C address of Pcf8574 
#define pcf8574_address 0x3F        // default I2C address of Pcf8574A 
#define BASE 64         // BASE is not less than 64 
//////// Define the output pins of the PCF8574, which are directly connected to the LCD1602 pin. 
#define RS      BASE+0 
#define RW      BASE+1 
#define EN      BASE+2 
#define LED     BASE+3 
#define D4      BASE+4 
#define D5      BASE+5 
#define D6      BASE+6 
#define D7      BASE+7 
 
int lcdhd;// used to handle LCD 
void printCPUTemperature(){// sub function used to print CPU temperature 