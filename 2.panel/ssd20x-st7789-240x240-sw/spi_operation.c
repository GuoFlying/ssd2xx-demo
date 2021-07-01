
//#include <stddef.h>
#include <unistd.h>

#include "gpio_operation.h"
#include "spi_operation.h"
	

void user_spi_init(void)
{
	_user_export_gpio(SPI_CS_PIN);
	_user_export_gpio(SPI_CS1_PIN);
	_user_export_gpio(SPI_CLK_PIN);
	_user_export_gpio(SPI_DI_PIN);
	_user_export_gpio(PANEL_BL_PIN);
	_user_export_gpio(PANEL_RST_PIN);
	_user_export_gpio(PANEL_ENABLE_PIN);
	
	_user_set_gpio_direction(SPI_CS_PIN, (char*)"out");
	_user_set_gpio_direction(SPI_CS1_PIN, (char*)"out");
	_user_set_gpio_direction(SPI_CLK_PIN, (char*)"out");
	_user_set_gpio_direction(SPI_DI_PIN, (char*)"out");
	_user_set_gpio_direction(PANEL_BL_PIN, (char*)"out");	
	_user_set_gpio_direction(PANEL_RST_PIN, (char*)"out");
	_user_set_gpio_direction(PANEL_ENABLE_PIN, (char*)"out");
	
#if 0
    _user_set_gpio_value(PANEL_ENABLE_PIN, PANEL_ENABLE_ON);
	
    _user_set_gpio_value(SPI_CS_PIN, 1);
    _user_set_gpio_value(SPI_CLK_PIN, 0);
    _user_set_gpio_value(SPI_DI_PIN, 1);
	usleep(120*1000);
	
	_user_set_gpio_value(PANEL_BL_PIN, 1);
#endif
}

void user_spi_cmd(unsigned char value)
{

	unsigned char n;

	_user_set_gpio_value(SPI_CS_PIN,  0);             //SPI_CS_PIN=0;
	_user_set_gpio_value(SPI_CLK_PIN, 0); usleep(4); //SPI_CLK_PIN=0;_nop_();_nop_();_nop_();_nop_();
	_user_set_gpio_value(SPI_CS_PIN,  0);             //SPI_CS_PIN=0;
	_user_set_gpio_value(SPI_DI_PIN,  0); //usleep(1);  //SPI_DI_PIN=0;
	_user_set_gpio_value(SPI_CLK_PIN, 1); usleep(4); //SPI_CLK_PIN=1;_nop_();_nop_();_nop_();_nop_();

	for(n=0; n<8; n++)			
	{  
		if((value & 0x80)!= 0 ) _user_set_gpio_value(SPI_DI_PIN,  1); //SPI_DI_PIN=1;
		else _user_set_gpio_value(SPI_DI_PIN,  0);                    //SPI_DI_PIN=0;
		value <<= 1;

		_user_set_gpio_value(SPI_CLK_PIN, 0); usleep(4);             //SPI_CLK_PIN=0;_nop_();_nop_();_nop_();_nop_();
		_user_set_gpio_value(SPI_CLK_PIN, 1); usleep(4);             //SPI_CLK_PIN=1;_nop_();_nop_();_nop_();_nop_();
	}
	_user_set_gpio_value(SPI_CS_PIN,  1);                             //SPI_CS_PIN=1;
}

void user_spi_data(unsigned char value)
{
	unsigned char n;
	
	
	_user_set_gpio_value(SPI_CLK_PIN, 0); usleep(4); //SPI_CLK_PIN=0;_nop_();_nop_();_nop_();_nop_();
	_user_set_gpio_value(SPI_CS_PIN,  0);             //SPI_CS_PIN=0;
	_user_set_gpio_value(SPI_DI_PIN,  1); //usleep(1);  //SPI_DI_PIN=1;
	_user_set_gpio_value(SPI_CLK_PIN, 1); usleep(4); //SPI_CLK_PIN=1;_nop_();_nop_();_nop_();_nop_();

	for(n=0; n<8; n++)			
	{  
		if((value & 0x80)!= 0 ) _user_set_gpio_value(SPI_DI_PIN,  1); //SPI_DI_PIN=1;
		else _user_set_gpio_value(SPI_DI_PIN,  0);                    //SPI_DI_PIN=0;
		value <<= 1;

		_user_set_gpio_value(SPI_CLK_PIN, 0); usleep(4);             //SPI_CLK_PIN=0;_nop_();_nop_();_nop_();_nop_();
		_user_set_gpio_value(SPI_CLK_PIN, 1); usleep(4);             //SPI_CLK_PIN=1;_nop_();_nop_();_nop_();_nop_();
	}

	_user_set_gpio_value(SPI_CS_PIN,  1);                             //SPI_CS_PIN=1;
}

