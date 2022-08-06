/*
 | file: 
 */
#include <8051.h>
//#include <string.h>

#define DB P1
#define RS P3_3
#define E P3_2

#define clear P2_0  // SW0
#define msg1 P2_1   // SW1
#define msg2 P2_2   // SW2

void returnHome(void);
void entryModeSet();
void displayOnOffControl(__bit display, __bit cursor, __bit blinking);
void cursorOrDisplayShift(__bit sc, __bit rl);
void functionSet(void);
void clearDisplay(void);
void setDdRamAddress(char address);

void sendChar(char);
void sendString(char*);
void delay(void);
void big_delay(void);
unsigned char scankeypad(void);

char colScan(char c);
void Main(void) __naked {
	
	unsigned char input = 0;
	unsigned int a = 0 ;
	unsigned int b = 0 ;
	unsigned long y = 0;
	unsigned char digit=0;
	unsigned char result[6]={0,0,0,0,0,0};
	char i;
	functionSet();
	entryModeSet(); // increment and no shift
	displayOnOffControl(1, 1, 1); // display on, cursor on and blinking on
	
	while (input != '='){
		input = scankeypad();
		sendChar(input);
		if (input == '+'){
			b = a;
			a = 0;
		}else if (input == '='){
			
			y = (unsigned long)a + (unsigned long)b;
			//digit = y%10;
			//do{
				//sendChar(result+0x30);
				//result = strcat(result,digit+0x30);
				//y=y/10;
				//digit = y%10;
			//}while(y != 0);
			//sendString (result);
			
			for (i=0;i<6;i++){
				result[i] = (y%10 + 0x30);
				y=y/10;
				if (y == 0){break;}
			}
			for (i=0;i<5;i++){
				if (result[i] != 0){
					sendChar(result[i]);
				}
			}
			
			
		}else{
			
			a = (a*10) + (input - 0x30);
			
		}
		
		while ((P0 & 0x70) != 0x70){
			//col = P0 & 0x70;
		}
		
	}
	while(1){}
}
unsigned char scankeypad(void){
	unsigned char row;
	unsigned char col;
	unsigned char keypad[4][3] = {
		{'3','2','1'},
		{'6','5','4'},
		{'9','8','7'},
		{'=','0','+'}
		};
	unsigned char i;
	//unsigned char j;
	unsigned char gotkey = 0;
	
	while(1){
		row = 0xf7;
		//P0 = P0 | 0xf0;
		
		for (i=0;i<4;++i){
			
			P0 = 0xff;
			//B=row;
			//B=row;
			//row = row | 0xf0;
			P0 = P0 & row;
			//delay();
			col = P0 & 0x70;
			if (col != 0x70){
				col = (~col) & 0x70;  // because the selected column returns zero
				col = col >> 5;
				gotkey = keypad[i][col];
				break;
			}
			row = row >> 1;
			row = row | 0xf0;
			//j=i+1;
			//row >> j;
			
		}
		
		if (gotkey != 0){break;}
		
	}
	//while (col != 0x70){
	//	col = P0 & 0x70;
	//}
	
	return gotkey;

		
}
char colScan(char rowmask) {
	// the caller provides a mask with the given row (P0.0-P0.3) set to 0.
	// now we want to return all columns that are pressed (P0.4-P0.6).
	P0 = rowmask; // drive the port with it, read out the columns
	return (~(P0 >> 4)) & 0x07;
}

// LCD Module instructions -------------------------------------------

void returnHome(void) {
	RS = 0;
	P1 = 0x02; // LCD command to return home (the first location of the first lcd line)
	E = 1;
	E = 0;
	big_delay();
}	

void entryModeSet() {
	RS = 0;
	P1 = 0x06;  
	E = 1;
	E = 0;
	delay();
}

void displayOnOffControl(__bit display, __bit cursor, __bit blinking) {
	P1_7 = 0;
	P1_6 = 0;
	P1_5 = 0;
	P1_4 = 0;
	P1_3 = 1;
	P1_2 = display;
	P1_1 = cursor;
	P1_0 = blinking;
	E = 1;
	E = 0;
	delay();
}

void cursorOrDisplayShift(__bit sc, __bit rl) {
	RS = 0;
	P1_7 = 0;
	P1_6 = 0;
	P1_5 = 0;
	P1_4 = 1;
	P1_3 = sc;
	P1_2 = rl;
	P1_1 = 0;
	P1_0 = 0;
	E = 1;
	E = 0;
	delay();
}

void functionSet(void) {
	RS = 0;
	P1 = 0x38; // 8-bit mode, 2 lines LCD
	E = 1;
	E = 0;
	delay();
}

void clearDisplay(void) {
	RS = 0;
	P1 = 0x01; // command to clear LCD
	E = 1;
	E = 0;
	big_delay();
}	

void setDdRamAddress(char address) {
	RS = 0;
	P1 = address | 0x80;  // set the MSB to detect that this is an address
	E = 1;
	E = 0;
	delay();
}

// --------------------------------------------------------------------

void sendChar(char c) {
	RS = 1;
	P1 = c;
	E = 1;
	E = 0;
	delay();
}

void sendString(char* str) {
	char c;
	while (c = *str++) {
		sendChar(c);
	}
}

void delay(void) {
	char c;
	for (c = 0; c < 50; c++);
}

void big_delay(void) {
	unsigned char c;
	for (c = 0; c < 255; c++);
}