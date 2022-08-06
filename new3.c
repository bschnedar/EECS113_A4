/*
// 
//
// To work, you have to change the EdSim51 LCD connection
// to connect RS to P3.3 and E to P3.2
//
// To compile, use the commands
// % sdcc --model-small -mmcs51 --Werror -I. -c lcd.c
// % sdcc --model-small -mmcs51 --Werror -I. --iram-size 0x100 \
//   --code-size 0x1000 --code-loc 0x0000 --stack-loc 0x30 --data-loc 0x30\
//   --out-fmt-ihx lcd.rel
// % packihx lcd.ihx > lcd.hex
*/

#include <8051.h>
#include <string.h>

#define DB P1
#define RS P3_3
#define E P3_2

#define msg1 P3_2   // SW0
#define msg2 P3_3   // SW1

//VARS


void returnHome(void);
void entryModeSet();
void displayOnOffControl(__bit display, __bit cursor, __bit blinking);
void cursorOrDisplayShift(__bit sc, __bit rl);
void functionSet(void);
void clearDisplay(void);
void setDdRamAddress(char address);

void sendChar(char c);
void sendString(char* str);
void delay(void);
void big_delay(void);
volatile  char scankeypad();
void timer_delay(void);

unsigned char row;
	unsigned char col;
	unsigned char keypad[4][3] = {
		{'3','2','1'},
		{'6','5','4'},
		{'9','8','7'},
		{41,'0',35}
		};
	unsigned char i;
	volatile  unsigned char gotkey;
	
//
int main();
	int set = 0;
	int log = 0;
	
	int tempa;
	int tempb;
	
	volatile char pressed;
	
	int hint = 0;
	char hour[2];
	
	int miint = 0;
	char min[2];
	
	int sint = 0;
	char sec[2];
	
	int dint = 1;
	char day[2];
	
	int moint = 1;
	char mon[2];
	
	int yint = 0;
	char year[2];
	
	
	//
void int0_isr (void) __interrupt (0) __using (1){
		setDdRamAddress(set);
		if (msg1 == 0){	
			pressed = scankeypad();
			
				if(pressed == 41){ //right
					if(set > 64){
						set = 0;
						log = 0;
					}
					else{
						set = set + 4;
						log++;
					}
					setDdRamAddress(set);	 
				}
				else if(pressed == 35){ //left
					if(set < 0){ //wraps
						set = 64;
						log = 8;
				}
			
		}
	
	
	/*
	
		if (msg1 == 0){	//TIME
		log = 0;
			setDdRamAddress(set);	    
			while (msg1 == 0){
				pressed = scankeypad();
				if(pressed == 41){ //right
					if(set > 64){
						set = 0;
						log = 0;
					}
					else{
						set = set + 4;
						log++;
					}
					setDdRamAddress(set);	 
				}
				else if(pressed == 35){ //left
					if(set < 0){ //wraps
						set = 64;
						log = 8;
					}
					else{
						set = set - 4;
						log--;
					}
					setDdRamAddress(set);	
				}
				else if(pressed == 48 || pressed == 49 || pressed == 50 ||
				pressed == 51 || pressed == 52 || pressed == 53 ||
				pressed == 54 || pressed == 55 || pressed == 56 || pressed == 57 ){		
					if(log == 0 && pressed < 2){
						sendChar(pressed);
						set =set+4;
						setDdRamAddress(set);
						
					}
					else if(log == 1){
						sendChar(pressed);
					}
				}
				else{
					break;
				}
			}  // Stay here till the msg1 switch is released
		}	
		else if (msg2 == 0){ //DATE	
		log = 0;
			setDdRamAddress(0x40);	    
			while (msg1 == 0){
				pressed = scankeypad();
				if(pressed == 43){  //+
					if(set > 128){
						set = 64;
						log = 0;
					}
					else{
						set = set + 4;
						log++;
					}
					setDdRamAddress(set);	 
				}
				else if(pressed == 35){  //#
					if(set < 64){ //wraps
						set = 128;
					}
					else{
						set = set - 4;
					}
					setDdRamAddress(set);	
				}
				else if(pressed == 48 || pressed == 49 || pressed == 50 ||
				pressed == 51 || pressed == 52 || pressed == 53 ||
				pressed == 54 || pressed == 55 || pressed == 56 || pressed == 57 ){
					sendChar(pressed);
				}
				else{
					break;
				}
			}  // Stay here till the msg1 switch is released
		}
		
	*/
}
	

//
	//

int main() __naked{
	IT0 = 1;   // make INT0 to edge triggered
	IE = 0x81;  //enable external interupt0
	

	

	//hour		
	hour[0] = ((hint/10) %10) +'0';
	hour[1] = (hint%10) +'0';
	//min
	min[0] = ((miint/10) %10) +'0';
	min[1] = (miint%10) +'0';
	//sec
	sec[0] = ((sint/10) %10) +'0';
	sec[1] = (sint%10) +'0';
	
	//day		
	day[0] = ((dint/10) %10) +'0';
	day[1] = (dint%10) +'0';
	
	//month		
	mon[0] = ((moint/10) %10) +'0';
	mon[1] = (moint%10) +'0';
	
	//year		
	year[0] = ((yint/10) %10) +'0';
	year[1] = (yint%10) +'0';
	
	
	//must define vars above this
	functionSet();
	entryModeSet(); // increment and no shift
	displayOnOffControl(1, 1, 1); // display on, cursor on and blinking on
	sendString(hour);
	sendChar(':');
	sendString(min);
	sendChar(':');
	sendString(sec);
	
	setDdRamAddress(0x40); // set address to start of second line	
	sendString(day);
	sendChar('-');
	sendString(mon);
	sendChar('-');
	sendString(year);
	
	

	while (1) {
		
		//inc the clock and date
		if(sint >= 59){
			sint = 0;
			miint++;
		}
		else{
			sint++;
		}
		if(miint >= 59){
			miint = 0;
			hint++;
		}
		if(hint >= 23){
			hint = 0;
			dint++;
		}
		if(dint >= 28 && moint == 2){
			dint = 1;
			moint++;
		}
		else if(dint >= 30 && (moint == 1 || moint == 3 || moint == 6 || moint == 7 || moint == 8 || moint == 10 || moint == 12)){
			dint = 1;
			moint++;
		}
		else if(dint >= 31 && (moint == 4 || moint == 5 || moint == 9 || moint == 11)){
			dint = 1;
			moint++;
		}
		if(moint >= 12){
			moint = 1;
			yint++;
		}
		
		//Updating time with new values
		//hour		
		hour[0] = ((hint/10) %10) +'0';
		hour[1] = (hint%10) +'0';
		//min
		min[0] = ((miint/10) %10) +'0';
		min[1] = (miint%10) +'0';
		//sec
		sec[0] = ((sint/10) %10) +'0';
		sec[1] = (sint%10) +'0';
	
		//day		
		day[0] = ((dint/10) %10) +'0';
		day[1] = (dint%10) +'0';
	
		//month		
		mon[0] = ((moint/10) %10) +'0';
		mon[1] = (moint%10) +'0';
	
		//year		
		year[0] = ((yint/10) %10) +'0';
		year[1] = (yint%10) +'0';	
		//
		
		
		//rewriting the display
		clearDisplay();
		setDdRamAddress(0x00);
		sendString(hour);
		sendChar(':');
		sendString(min);
		sendChar(':');
		sendString(sec);
		setDdRamAddress(0x40); // set address to start of second line	
		sendString(day);
		sendChar('-');
		sendString(mon);
		sendChar('-');
		sendString(year);
		
	
	//delay 100ns
	timer_delay();
	timer_delay();

		
	}
	return 0;
}





// LCD Module instructions -------------------------------------------

void returnHome(void) {
	RS = 0;
	P1 = 0x02; // LCD command to return home (the first location of the first lcd line)
	E = 1;
	E = 0;
	big_delay(); // This operation needs a bigger delay
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
	P1 = 0x01; // command to clear LCD and return the cursor to the home position
	E = 1;
	E = 0;
	big_delay(); // This operation needs a bigger delay
}	

void setDdRamAddress(char address) {  // Determine the place to place the next character - First LCD location address in 00h for line 1 and 40h for line 2
	RS = 0;
	P1 = address | 0x80;  // set the MSB to detect that this is an address
	E = 1;
	E = 0;
	delay();
}

// --------------------------------------------------------------------

void sendChar(char c) {  // Function to send one character to be displayed on the LCD
	RS = 1;
	P1 = c;
	E = 1;
	E = 0;
	delay();
}

void sendString(char str[2]) {  // Function to send a string of characters to be displayed on the lCD

		sendChar(str[0]);
		sendChar(str[1]);

}

void delay(void) {
	char c;
	for (c = 0; c < 50; c++);
}

void big_delay(void) {
	unsigned char c;
	for (c = 0; c < 255; c++);
}


// Keypad Module instructions -------------------------------------------
char scankeypad(){
	
	
	while(1){
		row = 0xf7;
		//P0 = P0 | 0xf0;
		
		for (i=0;i<4;++i){
			
			P0 = 0xff;

			P0 = P0 & row;
		
			col = P0 & 0x70;
			if (col != 0x70){
				col = (~col) & 0x70;  // because the selected column returns zero
				col = col >> 5;
				gotkey = keypad[i][col];
				break;
			}
			row = row >> 1;
			row = row | 0xf0;

			
		}
		
		if (gotkey != 0){break;}
		
	}
	
	
	return gotkey;

		
}

void timer_delay(void){
	
	TMOD = 0x01;
	TL0 = 0xAF;
	TH0 = 0x3C;
	TR0 = 1;
	while (!TF0) ;
	TR0 = 0;
	TF0 = 0;

		
}

//



	void _sdcc_gsinit_startup(void) {

        __asm
                mov sp, #0x5F
        __endasm;
       main();
}
	
	
 