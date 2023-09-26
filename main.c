/*
	This program cycles through the colors of the RGB LED (at a rate of 1Hz or .5Hz) on the launch pad in the order Red -> Green -> Blue
	When SW2 is pressed, the cycle resets back to red
	When SW1 is pressed, the time the colors are displayed is toggled between one second and two seconds
*/


#include <stdint.h>
#include <stdbool.h>
#include "TM4C123.h"
#include "alphabet_mem_binary.h"


#define RED 0x02;
#define BLUE 0x04;
#define GREEN 0x08;
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register


// Define OLED dimensions
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define slaveaddress 0x3C
// Define command macros
#define OLED_SETCONTRAST 0x81
#define OLED_DISPLAYALLON_RESUME 0xA4
#define OLED_DISPLAYALLON 0xA5
#define OLED_NORMALDISPLAY 0xA6
#define OLED_INVERTDISPLAY 0xA7
#define OLED_DISPLAYOFF 0xAE
#define OLED_DISPLAYON 0xAF
#define OLED_SETDISPLAYOFFSET 0xD3
#define OLED_SETCOMPINS 0xDA
#define OLED_SETVCOMDETECT 0xDB
#define OLED_SETDISPLAYCLOCKDIV 0xD5
#define OLED_SETPRECHARGE 0xD9
#define OLED_SETMULTIPLEX 0xA8
#define OLED_SETLOWCOLUMN 0x00
#define OLED_SETHIGHCOLUMN 0x10
#define OLED_SETSTARTLINE 0x40
#define OLED_MEMORYMODE 0x20
#define OLED_COLUMNADDR 0x21
#define OLED_PAGEADDR   0x22
#define OLED_COMSCANINC 0xC0
#define OLED_COMSCANDEC 0xC8
#define OLED_SEGREMAP 0xA0
#define OLED_CHARGEPUMP 0x8D

static const uint8_t flexbison[32][16] =  //this is parsed and massaged bmp data
{
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0x01, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0x03, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff},
	{0xff, 0xff, 0xfe, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff},
	{0xff, 0xff, 0xfe, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x01, 0xff, 0xff},
	{0xff, 0xff, 0xfc, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x08, 0x13, 0xfe, 0x7f, 0xfc, 0x00, 0xff, 0xff},
	{0xff, 0xff, 0xfe, 0x00, 0x1f, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x70, 0x03, 0xc0, 0x00, 0x3f, 0xff},
	{0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x06, 0x00, 0x01, 0x80, 0x08, 0x00, 0x00, 0x00, 0x0f, 0xff},
	{0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x07, 0x1c, 0x08, 0xc0, 0x00, 0x00, 0x00, 0x3f, 0xff},
	{0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x3e, 0x50, 0xb9, 0x78, 0x00, 0x00, 0x00, 0x7f, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xb0, 0x6c, 0x3e, 0x00, 0x00, 0x07, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0x9f, 0xdf, 0x87, 0x80, 0x3f, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3e, 0x2f, 0xe0, 0x60, 0x07, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x20, 0x7e, 0x64, 0x4f, 0xbf, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0b, 0x9f, 0xc7, 0xde, 0x1f, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xe3, 0xf9, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7c, 0xfe, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0x0f, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xe0, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x78, 0x10, 0x7f, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x80, 0x0f, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd0, 0x0f, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};

//   Insert Function Prototypes here
//These functions are defined in startup.s at line 592. They utilized assembly commands.
//void DisableInterrupts(void); // Disable interrupts
//void EnableInterrupts(void);  // Enable interrupts
//long StartCritical (void);    // previous I bit, disable interrupts
//void EndCritical(long);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void SystickInit(void);				// Initialize Systick
void SysTick_Handler(void);			// Systick Handler
void GPIOF_Handler(void);		//GPIO port F interrupt handler
//return_type name( variables);
void PORTF_init(void);
void SystemInit(void);
void SystemCoreClockUpdate (void);
void Delay(uint32_t nTime);
void TimingDelayDecrement(void);
void I2C_init(I2C0_Type *i2c);
void LCD_cursor(void);

// Function declarations
 void flexthebison(uint8_t x);
 uint8_t getpixel(uint8_t x, uint8_t y);
 uint8_t getlettercol(uint8_t letter, uint8_t column);
 void write_letter(uint8_t letter);
 void OLED_Clear(void);
 
// Function prototypes initialize, tranmit and read functions 
 void I2C0_Init ( void );  
 static int I2C_wait_till_done(void);
 void OLED_2B_CMD(I2C0_Type *i2c, uint32_t slaveAddr, char memAddr, uint8_t data);
 void OLED_1B_CMD(I2C0_Type *i2c, uint32_t slaveAddr, uint8_t data); 



//Global Variables
static volatile int Systick;
static volatile uint8_t colors[7] = {0x02, 0x08, 0x04, 0x06,  0x0A, 0x0C, 0x0E};
static volatile int color_cnt;
static volatile int color_duration[2] = {1000, 2000};
static volatile int color_dly = 1000;
static volatile int delay_cnt;
static volatile uint32_t TimingDelay;


//static volatile uint8_t oledram [4][127];
																					
int main()
{
	uint8_t i, j, pixel; // bit, k, fbrow, fbcol, bitshift = 0;
	//int8_t l;
	SystemInit();
	SystemCoreClockUpdate ();
	SystickInit();
	PORTF_init();
	I2C_init(I2C0);
	

//oled is going to paint picture such that MSB of byte being written is on the bottom of the image
//uint8_t oleddata[4][127]; 
//uint8_t coldata = 0x00;
//bool bit = 0; 

//===============================
//initially how I displayed image, copied memory to new OLED memory after transforming data
//===============================

//	for(i=0;i<4;i++){
//		for(j=0;j<128;j++){
//			oledram[i][j] = 0x00;
//		}
//	}

//for(i=0; i<4; i++){  //each row of original image
//	for(j=0; j<16; j++){ //each column of original image
//	uint8_t test = 0x01<<7;
//	
//		//iterate rows inside of page
//		for(l=7; l>=0; l--){ //each bit of selected original column
//			for(k=0; k<8; k++){ //iterate rows inside of page
//				test = flexbison[(i*8)+k][j];
//				fbrow = i*8 +k;
//				fbcol = j;
//				bit = ((flexbison[fbrow][fbcol]) & (0x01<<l));
//				bitshift = bit << (7-l);
//				coldata |= bitshift>>(7-k); 
//				//coldata |= ((flexbison[fbrow][fbcol]) & (0x01<<l))>>(7-k);  //should return a column of data
//				oledram[i][(8*j)+(7-l)] = coldata;  //should return a column of data 
//			}
//			coldata = 0x00;
//		}
//		
//	}
//}


	//==========================
	// CLEAR OLED
	//==========================
		I2C0->MDR = 0x00;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0xb0;  //page select
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
		
		for(i=0; i<16; i++){
			write_letter(i);
		}	

		I2C0->MDR = 0x00;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0xb0|2;  //page select
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
		
		for(i=0; i<16; i++){
			write_letter(i+16);
		}	

	GPIOF->DATA = 0x00;
	

	for (i = 0; i<50; i++){ 
		GPIOF->DATA = colors[i%7];
		Delay(25);
		GPIOF->DATA = 0x00;
		Delay(25);
	}

	//Sit in this while loop for ever.
	while(1)
	{
		
		GPIOF->DATA = 0x00;					/// turn off any colors that are on
		GPIOF->DATA = colors[color_cnt%7];	// display color we want
//============================		
//		Delay(color_dly);
//		color_cnt++;
//			Systick = 0;
//============================		
		if(Systick > color_dly){									// color has been on for 1 or 2 seconds, switch colors and reset counter
			color_cnt++;
			Systick = 0;			
		}
		
	} //end infinite while
	
}




void PORTF_init(void)
{
	uint32_t check_value = 0x00000020;	//Value by which to check to see if the clock has been
																													//activated for the current port. Have to preserve the 
																													//The current value of the SYSCTL CGCGPIO  registers as 
																													//other clocks may have been set.
																													//This value is found in tm4c123gh6pm.h at line 10453
//	SYSCTL CGCGPIO  = (SYSCTL CGCGPIO | 0x00000020);								//1)Activate the clock for PortF
	
	SYSCTL->RCGCGPIO = (SYSCTL->RCGCGPIO| 0x00000020);
	
	while((SYSCTL->PRGPIO & check_value) == 0)		//Checking to see if the GPIOF is ready, 
	{}															
	
	GPIOF->LOCK = GPIO_LOCK_KEY;
	GPIOF->CR = 0xFF;
  GPIOF->DIR  = 0xEE;     //PF0 and 4 are SW 2 and SW respectively. PF1 2 3 -> on board RBG LEDs respectively. 1 = output 0 = input
	GPIOF->PUR  = 0x11;
  GPIOF->DEN  = 0xFF;		
	GPIOF->IM  |= 0x00000011;
	GPIOF->ICR  |= 0x11;
	GPIOF->IEV  &= 0xEE; 		
	GPIOF->IBE  &= 0xEE;
	GPIOF->IS  &= 0xEE; 
	NVIC->ISER[0]  |= (0x01<<0x1E);						//Table 2-9 in datasheet, it's easier to shift bits than do decimal -> hex math
	NVIC->IP[7]  |= (0x03<<0x15);//|(0x01<<0x16);		

	//GPIOF_DATA  = 0x00;
}	


void GPIOF_Handler(void){
	if((GPIOF->MIS &0x01) == 0x01){
		color_cnt = 0x00;
		Systick = 0x00000000;
		OLED_Clear();
		GPIOF->ICR  |= 0x01;
	}
	else if((GPIOF->MIS &0x10) == 0x10){
		delay_cnt++;
		Systick = 0;
		color_dly = color_duration[delay_cnt%2];
		flexthebison(delay_cnt%2);
		GPIOF->ICR  |= 0x10;
	}
}


void SystickInit(void){
	//set STRELOAD
	//Clear STCURRENT
	//Configure STCRTL
	SysTick->LOAD  = 0x0c350;  //clk wiz only allows max PLL freq of 200MHz, sysdiv value = 4, therefore sys_clk is 50MHz
	SysTick->VAL  = 0x00003E7F;
	SysTick->CTRL  = 0x00000007;
	
}

void Delay(uint32_t nTime){ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
} // end Delay()

void TimingDelayDecrement(void){
	if (TimingDelay != 0x00){ 
    TimingDelay--;
  }
}  //end TimingDelayDecrement()

void SysTick_Handler (void){
	TimingDelayDecrement();
	Systick++;
}

void OLED_2B_CMD(I2C0_Type *i2c, uint32_t slaveAddr, char memAddr, uint8_t data){
	
	/* send slave address and starting address */
    i2c->MSA = slaveAddr << 1;
    i2c->MDR = 0x00;		//tell controller this is command
    i2c->MCS = 3;                      /* S-(saddr+w)-ACK-maddr-ACK */
	while(i2c->MCS & 0x01); 
    
	i2c->MDR = memAddr;
    i2c->MCS = 1;
    while(i2c->MCS & 0x01);
	
    /* send data */
    i2c->MDR = data;
    i2c->MCS = 5;                      /* -data-ACK-P */
    while(i2c->MCS & 0x01); 	
}

void OLED_1B_CMD(I2C0_Type *i2c, uint32_t slaveAddr, uint8_t data){
	
	/* send slave address and starting address */
    i2c->MSA = slaveAddr << 1;
    i2c->MDR = 0x00;		//tell controller this is command
    i2c->MCS = 3;                      /* S-(saddr+w)-ACK-maddr-ACK */
	while(i2c->MCS & 0x01); 
    /* send data */
    i2c->MDR = data;
    i2c->MCS = 5;                      /* -data-ACK-P */
    while(i2c->MCS & 0x01); 	
}

void I2C_init(I2C0_Type *i2c){
   
	 uint32_t i, j ;
	uint32_t check_value = 0x00000002;	//Value by which to check to see if the clock has been
	SYSCTL->RCGCI2C |= 0x01; 			//I2C clock gating
	SYSCTL->RCGCGPIO |= 0x02;			//clock gating for GPIOB
	while((SYSCTL->PRGPIO & check_value) == 0)		//Checking to see if the GPIOF is ready, 
	{}
	GPIOB->AFSEL |= 0x0c;					//Alternate function GPIO
	GPIOB->ODR |=0x08;						//PB3 open drain enable
	GPIOB->DEN |= 0x0c;
	GPIOB->ODR |= 0x00000008 ; // SDA (PD1 ) pin as open darin
	GPIOB->PCTL |=0x00003300;			//Alternate function mux select
	i2c->MCR |=  0x00000010;						//Initialize I2C0 master
	i2c->MTPR = 0x18;						//# of sys clks per i2c clk (ref. step 7 of i2c instructions of datasheet)
	

Delay(20);
Delay(200);


//==============================
//OLED INITIALIZATION
//==============================
	OLED_2B_CMD(i2c, slaveaddress, 0xa8, 0x1f);
	OLED_2B_CMD(i2c, slaveaddress, 0xd3, 0x00);
	OLED_1B_CMD(i2c, slaveaddress, 0x40);
	OLED_2B_CMD(i2c, slaveaddress, 0x8d, 0x14);
	OLED_1B_CMD(i2c, slaveaddress, 0xA1);
	OLED_1B_CMD(i2c, slaveaddress, 0xc8);
	OLED_2B_CMD(i2c, slaveaddress, 0xda, 0x02);
	OLED_2B_CMD(i2c, slaveaddress, 0x81, 0x7f);
	OLED_1B_CMD(i2c, slaveaddress, 0xA4);
	OLED_1B_CMD(i2c, slaveaddress, 0xA6);
	OLED_2B_CMD(i2c, slaveaddress, 0xd5, 0x80);
	OLED_1B_CMD(i2c, slaveaddress, 0xAf);
//==============================	
	
	//
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	i2c->MDR = 0x00000000;
	while((i2c->MCS)&0x01);
	i2c->MCS =0x03;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x000000AE;
	i2c->MCS =0x05;
	while((i2c->MCS)&0x01);
	
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	i2c->MDR = 0x00000000;
	while((i2c->MCS)&0x01);
	i2c->MCS =0x03;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x000000Af;
	i2c->MCS =0x05;
	while((i2c->MCS)&0x01);
	
	//page lower nibble
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	i2c->MDR = 0x00000000;
	while((i2c->MCS)&0x01);
	i2c->MCS =0x03;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x00000000;
	i2c->MCS =0x05;
	while((i2c->MCS)&0x01);
	
	//page upper nibble
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	i2c->MDR = 0x00000010;
	while((i2c->MCS)&0x01);
	i2c->MCS =0x03;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x00000000;
	i2c->MCS =0x05;
	while((i2c->MCS)&0x01);
	
	//addressing mode
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	i2c->MDR = 0x00000000;
	while((i2c->MCS)&0x01);
	i2c->MCS =0x03;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x00000020;
	i2c->MCS =0x01;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x00000002;
	i2c->MCS =0x05;
	while((i2c->MCS)&0x01);
	
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	
	//clear OLED
	
	//page lower nibble
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	
	//==========================
	// CLEAR OLED
	//==========================
	for (j=0; j<4; j++){
		i2c->MDR = 0x00;
		while((i2c->MCS)&0x01);
		i2c->MCS =0x03;
		while((i2c->MCS)&0x01);
		i2c->MDR = 0xb0|j;
		i2c->MCS =0x05;
		while((i2c->MCS)&0x01);
		
		for(i=0; i<128; i++){
		i2c->MDR = 0x00000040;
		while((i2c->MCS)&0x01);
		i2c->MCS =0x03;
		while((i2c->MCS)&0x01);
		i2c->MDR = 0x00000000;
		i2c->MCS =0x05;
		while((i2c->MCS)&0x01);
		}
	}
	
	
			//turn on whole screen
//		 I2C0->MDR = 0x00000000;
//		while((I2C0->MCS)&0x01);
//		I2C0->MCS =0x03;
//		while((I2C0->MCS)&0x01);
//		I2C0->MDR = 0xa5;
//		I2C0->MCS =0x05;
//		while((I2C0->MCS)&0x01); 

//col address lower nibble
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	i2c->MDR = 0x00000000;
	while((i2c->MCS)&0x01);
	i2c->MCS =0x03;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x00000021;
	i2c->MCS =0x01;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x00000000;
	i2c->MCS =0x01;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x0000007f;
	i2c->MCS =0x05;
	while((i2c->MCS)&0x01);
	
	//addressing mode
	i2c->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
	i2c->MDR = 0x00000000;
	while((i2c->MCS)&0x01);
	i2c->MCS =0x03;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x00000020;
	i2c->MCS =0x01;
	while((i2c->MCS)&0x01);
	i2c->MDR = 0x00000000;
	i2c->MCS =0x05;
	while((i2c->MCS)&0x01);
		
		Delay(200);
}



/* wait untill I2C Master module is busy */
/*  and if not busy and no error return 0 */
static int I2C_wait_till_done(void)
{
    while(I2C0->MCS & 1);   /* wait until I2C master is not busy */
    return I2C0->MCS & 0xE; /* return I2C error code, 0 if no error*/
}



/*******************************************************************************
 * Function: void OLED_Clear()
 *
 * Returns: Nothing
 *
 * Description: Clears the OLED
 * 
 ******************************************************************************/

void OLED_Clear(void)
{
    //==========================
	// CLEAR OLED
	//==========================
	//col address lower nibble
		I2C0->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
		I2C0->MDR = 0x00000000;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0x00000021;
		I2C0->MCS =0x01;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0x00000000;
		I2C0->MCS =0x01;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0x0000007f;
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
		
		//addressing mode
		I2C0->MSA |= (0x3c<<1);			//Set up slave address (see datasheet)
		I2C0->MDR = 0x00000000;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0x00000020;
		I2C0->MCS =0x01;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0x00000000;
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
	
	for (uint8_t j=0; j<4; j++){
		I2C0->MDR = 0x00;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0xb0|j;
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
		
		for(uint8_t i=0; i<128; i++){
		I2C0->MDR = 0x00000040;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0x00000000;
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
		}
	}
}

void flexthebison(uint8_t x){
	//==========================
	// write flex bison
	//==========================
	uint8_t bit;
//	uint32_t time, save;
//	int blah, f;
	//OLED_Clear();
//	SysTick->VAL = 0x00;  //clear for timing
//	blah = Systick;
//	save = SysTick->VAL;
	for (uint8_t j=0; j<4; j++){
		I2C0->MDR = 0x00;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		I2C0->MDR = 0xb0|j;  //page select
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
		
		for(uint8_t i=0; i<128; i++){
		I2C0->MDR = 0x00000040;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		//bit = x ? ~oledram[j][i]:oledram[j][i];
		bit = getpixel(i,j);
		I2C0->MDR = x ? ~bit : bit;
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
		}
	}
	
//	time = SysTick->VAL - save;
//	f = Systick-blah;
}

uint8_t getpixel(uint8_t x, uint8_t y){
	uint8_t  result; //, test, row, col, shift; //,bit;
	bool bit;
//	row = y*8;
//	col = x/8;
	
	result =0;
	
	for (uint8_t i=0; i<8; i++){
//		row = (y*8) +i;  	//debug
//		col = x/8;   			//debug
//		shift = x%8;			//debug
//		test = flexbison[(y*8) + i][x/8];		//debug
		bit = ( flexbison[(y*8) + i][x/8] & 0x01<< (7-(x%8)) );
		result |= bit<<(i);
	}	
	
	return result;
}

uint8_t getlettercol(uint8_t letter, uint8_t column){
	uint8_t  result; //, test, row, col, shift; //,bit;
	bool bit;
//	row = y*8;
//	col = x/8;
	
	result =0;
	
	for (uint8_t i=0; i<8; i++){
//		row = (y*8) +i;  	//debug
//		col = x/8;   			//debug
//		shift = x%8;			//debug
//		test = flexbison[(y*8) + i][x/8];		//debug
		bit = ( alphabet[letter][i]&0x01 << (7-(column%8))) ;
		result |= bit<<(i);
	}	
	
	return result;
}


void write_letter(uint8_t letter){
	//==========================
	// write flex bison
	//==========================
	uint8_t bit;
//	uint32_t time, save;
//	int blah, f;
	//OLED_Clear();
//	SysTick->VAL = 0x00;  //clear for timing
//	blah = Systick;
//	save = SysTick->VAL;
	
		
		for(uint8_t i=0; i<8; i++){
		I2C0->MDR = 0x00000040;
		while((I2C0->MCS)&0x01);
		I2C0->MCS =0x03;
		while((I2C0->MCS)&0x01);
		//bit = x ? ~oledram[j][i]:oledram[j][i];
		bit = getlettercol(letter,i);
		I2C0->MDR = bit;
		I2C0->MCS =0x05;
		while((I2C0->MCS)&0x01);
		}
	}

