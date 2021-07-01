#include <unistd.h>
#include "gpio_operation.h"
#include "spi_operation.h"

#define HIGH_LEVEL              (1)
#define LOW_LEVEL               (0)
#define PANEL_ENABLE_ON         (HIGH_LEVEL)
#define PANEL_ENABLE_OFF        (LOW_LEVEL)
 

#define SPI_WriteComm(x)    user_spi_cmd(x)
#define SPI_WriteData(x)    user_spi_data(x)

void LCD_Init(void)
{
	user_spi_init();
	
	_user_set_gpio_value(PANEL_ENABLE_PIN, PANEL_ENABLE_ON);
	
	_user_set_gpio_value(PANEL_BL_PIN, 1);
	usleep(300*1000);

	_user_set_gpio_value(SPI_CS1_PIN,  0);
	_user_set_gpio_value(SPI_CS_PIN,  1);   //SPI_CS_PIN=0;
	
	_user_set_gpio_value(PANEL_RST_PIN,  1);   //SPI_RES=1;
	usleep(50*1000);
	_user_set_gpio_value(PANEL_RST_PIN,  0);   //SPI_RES=0;
	usleep(500*1000);
	_user_set_gpio_value(PANEL_RST_PIN,  1);   //SPI_RES=1;
	usleep(300*1000);		
	
	/////////////Start Initial Sequence /////////////////

#if 1
//---------------------------------------------------------------------------------------------------//
SPI_WriteComm(0x11);
usleep(120*1000);;  //Delay 120ms
//------------------------------display and color format setting--------------------------------//
SPI_WriteComm(0x36);
SPI_WriteData(0x48); //00 48
SPI_WriteComm(0x3a);
SPI_WriteData(0x55);

	SPI_WriteComm(0xB0);  //Interface Control
	SPI_WriteData(0x11);//11 12	//RGB interface
	SPI_WriteData(0X00);//00 F0 	//0XF4	/RIM=1:6 BIT BUS WIDTH flying 小端优先


	
	SPI_WriteComm(0xB1);  //RGB Interface Control
	SPI_WriteData(0xE0);  // without RAM=C0 , With RAM = 40h , HV mode=60h
	SPI_WriteData(0x03);  // V Bproch by HV mode= MCU porch
	SPI_WriteData(0x14);  // H Bproch by HV mode= MCU porch
	
	SPI_WriteComm(0xB2);
	SPI_WriteData(0x0C);
	SPI_WriteData(0x0C);
	SPI_WriteData(0x00);
	SPI_WriteData(0x33);
	SPI_WriteData(0x33);


					
	// SPI_WriteComm(0xF6);  //Interface Control
	// SPI_WriteData(0x01);
	// SPI_WriteData(0x13);
	// SPI_WriteData(0x07);  //RIM: Specify the RGB interface mode when the RGB interface is used. 

//--------------------------------ST7789V Frame rate setting -OK ----------------------------------//
SPI_WriteComm(0xb2);
SPI_WriteData(0x0c);
SPI_WriteData(0x0c);
SPI_WriteData(0x00);
SPI_WriteData(0x33);
SPI_WriteData(0x33);
SPI_WriteComm(0xb7);
SPI_WriteData(0x35);
//---------------------------------ST7789V Power setting-OK--------------------------------------//
SPI_WriteComm(0xbb);
SPI_WriteData(0x31); //28 31
SPI_WriteComm(0xc0);
SPI_WriteData(0x2c);
SPI_WriteComm(0xc2);
SPI_WriteData(0x01);
SPI_WriteComm(0xc3);
SPI_WriteData(0x1b); //0B 1B
SPI_WriteComm(0xc4);
SPI_WriteData(0x0D); //20 0d

SPI_WriteComm(0xc6);
SPI_WriteData(0x0f);
SPI_WriteComm(0xd0);
SPI_WriteData(0xa4);
SPI_WriteData(0xa1);
//--------------------------------ST7789V gamma setting---------------------------------------//
SPI_WriteComm(0xe0);
SPI_WriteData(0xd0);
SPI_WriteData(0x05);
SPI_WriteData(0x09);
SPI_WriteData(0x10);
SPI_WriteData(0x08);
SPI_WriteData(0x14);
SPI_WriteData(0x28);
SPI_WriteData(0x33);
SPI_WriteData(0x3f);
SPI_WriteData(0x07);
SPI_WriteData(0x13);
SPI_WriteData(0x14);
SPI_WriteData(0x28);
SPI_WriteData(0x30);

SPI_WriteComm(0xe1);
SPI_WriteData(0xd0);
SPI_WriteData(0x05);
SPI_WriteData(0x09);
SPI_WriteData(0x09);
SPI_WriteData(0x08);
SPI_WriteData(0x03);
SPI_WriteData(0x24);
SPI_WriteData(0x32);
SPI_WriteData(0x3b);
SPI_WriteData(0x38);
SPI_WriteData(0x14);
SPI_WriteData(0x13);
SPI_WriteData(0x28);
SPI_WriteData(0x2f);


SPI_WriteComm(0x21); // flying 反色
SPI_WriteComm(0x29);
//SPI_WriteComm(0x2C);

#else
 SPI_WriteComm(0x11);
     usleep(120*1000);;  //Delay 120ms
//--------------------------------Display and color format setting----------------------------//
  SPI_WriteComm(0xB0);      
	SPI_WriteData(0x12);  //11   
	SPI_WriteData(0xf0); 
	
	SPI_WriteComm(0xb1);   
	SPI_WriteData(0x40); //  
	SPI_WriteData(0x02);  //
	SPI_WriteData(0x14);  //
//-----end RGB interface setting------------ -- 
//--------------------------------ST7789S Frame rate setting----------------------------------//
	SPI_WriteComm(0xb2);
	SPI_WriteData(0x0c);
	SPI_WriteData(0x0c);
	SPI_WriteData(0x00);
	SPI_WriteData(0x33);
	SPI_WriteData(0x33);
	
	SPI_WriteComm(0xb7);
	SPI_WriteData(0x35); //VGH=13.26V, VGL=-10.43V
	
	//---------------------------------ST7789S Power setting--------------------------------------//
	SPI_WriteComm(0xbb);
	SPI_WriteData(0x31); //VCOM=1.325V
	SPI_WriteComm(0xc0);
	SPI_WriteData(0x2c); //Default
	SPI_WriteComm(0xc2);
	SPI_WriteData(0x01); //Default
	SPI_WriteComm(0xc3);
	SPI_WriteData(0x1b); //GVDD=4.9V
	SPI_WriteComm(0xc4);
	SPI_WriteData(0x0D); //VDV=0
	SPI_WriteComm(0xc6);
	SPI_WriteData(0x0f); //60Hz

	SPI_WriteComm(0xd0);
	SPI_WriteData(0xa4); 
	SPI_WriteData(0xa1); //AVDD=6.8V, AVCL=-4.6V
	
	SPI_WriteComm(0x36);
	SPI_WriteData(0x48); 
	
	SPI_WriteComm(0x3a);
	SPI_WriteData(0x05); //64k
	
	//--------------------------------ST7789S gamma setting---------------------------------------//
SPI_WriteComm(0xe0);
SPI_WriteData(0xd0);
SPI_WriteData(0x05);
SPI_WriteData(0x09);
SPI_WriteData(0x10);
SPI_WriteData(0x08);
SPI_WriteData(0x14);
SPI_WriteData(0x28);
SPI_WriteData(0x33);
SPI_WriteData(0x3f);
SPI_WriteData(0x07);
SPI_WriteData(0x13);
SPI_WriteData(0x14);
SPI_WriteData(0x28);
SPI_WriteData(0x30);


SPI_WriteComm(0xe1);
SPI_WriteData(0xd0);
SPI_WriteData(0x05);
SPI_WriteData(0x09);
SPI_WriteData(0x09);
SPI_WriteData(0x08);
SPI_WriteData(0x03);
SPI_WriteData(0x24);
SPI_WriteData(0x32);
SPI_WriteData(0x3b);
SPI_WriteData(0x38);
SPI_WriteData(0x14);
SPI_WriteData(0x13);
SPI_WriteData(0x28);
SPI_WriteData(0x2f);
SPI_WriteComm(0x21);
SPI_WriteComm(0x29);
SPI_WriteComm(0x2C);	 
	
#endif
}
