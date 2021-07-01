#include <unistd.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "gpio_operation.h"
#include "st7789_lcd.h"


static const char *device = "/dev/spidev0.0";
static uint8_t mode = 0;            /* SPI通信使用全双工，设置CPOL＝0，CPHA＝0。 */
static uint8_t bits = 9;            /* ８ｂiｔｓ读写，MSB first。*/
static uint32_t speed = 60 * 1000 * 1000; /* 设置传输速度 */
static uint16_t delay = 0;
static int g_SPI_Fd = 0;

static void pabort(const char *s)
{
    perror(s);
    abort();
}

int spi_open(void)
{
    int fd;
    int ret = 0;

    if (g_SPI_Fd != 0) /* 设备已打开 */
        return 0xF1;

    fd = open(device, O_RDWR);
    if (fd < 0)
        pabort("can't open device\n");
    else
        printf("SPI - Open Succeed. Start Init SPI...\n");

    g_SPI_Fd = fd;
    /*
    * spi mode
    */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode\n");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode\n");

    /*
    * bits per word
    */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word\n");
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word\n");

    /*
    * max speed hz
    */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz\n");

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz\n");

    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d KHz (%d MHz)\n", speed / 1000, speed / 1000 / 1000);

    return ret;
}

int spi_write(uint8_t *TxBuf, int len)
{
    int ret;

    ret = write(g_SPI_Fd, TxBuf, len);
    if (ret < 0)
        perror("SPI Write error\n");

    return ret;
}

static void spi_write_cmd(uint8_t cmd)
{
    _user_set_gpio_value(SPI_CS_PIN,  0);
    uint8_t tx[2];
    tx[0] = 0;
    tx[1] = cmd;
    spi_write(tx,2);
    _user_set_gpio_value(SPI_CS_PIN,  1);
}

static void spi_write_data(uint8_t data)
{
    _user_set_gpio_value(SPI_CS_PIN,  0);
    uint8_t tx[2];
    tx[0] = 1;
    tx[1] = data;
    spi_write(tx,2);
    _user_set_gpio_value(SPI_CS_PIN,  1);
}


void lcd_spi_init(void)
{
    _user_export_gpio(SPI_CS_PIN);
	_user_export_gpio(SPI_CS1_PIN);
	_user_export_gpio(PANEL_BL_PIN);
	_user_export_gpio(PANEL_RST_PIN);
	_user_export_gpio(PANEL_ENABLE_PIN);
	
	_user_set_gpio_direction(SPI_CS_PIN, (char*)"out");
	_user_set_gpio_direction(SPI_CS1_PIN, (char*)"out");
	_user_set_gpio_direction(PANEL_BL_PIN, (char*)"out");	
	_user_set_gpio_direction(PANEL_RST_PIN, (char*)"out");
	_user_set_gpio_direction(PANEL_ENABLE_PIN, (char*)"out");
    _user_set_gpio_value(PANEL_ENABLE_PIN, 1);
    _user_set_gpio_value(PANEL_BL_PIN, 1);

    _user_set_gpio_value(SPI_CS1_PIN, 1);
    _user_set_gpio_value(SPI_CS_PIN, 1); //SPI_CS_PIN=0;

    _user_set_gpio_value(PANEL_RST_PIN, 0); //SPI_RES=0;
    usleep(50 * 1000);
    _user_set_gpio_value(PANEL_RST_PIN, 1); //SPI_RES=1;


    spi_open();

    /////////////Start Initial Sequence /////////////////

    //---------------------------------------------------------------------------------------------------//
    spi_write_cmd(0x11);
    usleep(120 * 1000);
     //Delay 120ms
    //------------------------------display and color format setting--------------------------------//
    spi_write_cmd(0x36);
    spi_write_data(0x48); //00 48
    spi_write_cmd(0x3a);
    spi_write_data(0x55);

    spi_write_cmd(0xB0); //Interface Control
    spi_write_data(0x11); //11 12	//RGB interface
    spi_write_data(0X00); //00 F0 	//0XF4	/RIM=1:6 BIT BUS WIDTH flying 小端优先

    spi_write_cmd(0xB1); //RGB Interface Control
    spi_write_data(0xE0); // without RAM=C0 , With RAM = 40h , HV mode=60h
    spi_write_data(0x03); // V Bproch by HV mode= MCU porch
    spi_write_data(0x14); // H Bproch by HV mode= MCU porch

    spi_write_cmd(0xB2);
    spi_write_data(0x0C);
    spi_write_data(0x0C);
    spi_write_data(0x00);
    spi_write_data(0x33);
    spi_write_data(0x33);

    // spi_write_cmd(0xF6);  //Interface Control
    // spi_write_data(0x01);
    // spi_write_data(0x13);
    // spi_write_data(0x07);  //RIM: Specify the RGB interface mode when the RGB interface is used.

    //--------------------------------ST7789V Frame rate setting -OK ----------------------------------//
    spi_write_cmd(0xb2);
    spi_write_data(0x0c);
    spi_write_data(0x0c);
    spi_write_data(0x00);
    spi_write_data(0x33);
    spi_write_data(0x33);
    spi_write_cmd(0xb7);
    spi_write_data(0x35);
    //---------------------------------ST7789V Power setting-OK--------------------------------------//
    spi_write_cmd(0xbb);
    spi_write_data(0x31); //28 31
    spi_write_cmd(0xc0);
    spi_write_data(0x2c);
    spi_write_cmd(0xc2);
    spi_write_data(0x01);
    spi_write_cmd(0xc3);
    spi_write_data(0x1b); //0B 1B
    spi_write_cmd(0xc4);
    spi_write_data(0x0D); //20 0d

    spi_write_cmd(0xc6);
    spi_write_data(0x0f);
    spi_write_cmd(0xd0);
    spi_write_data(0xa4);
    spi_write_data(0xa1);
    //--------------------------------ST7789V gamma setting---------------------------------------//
    spi_write_cmd(0xe0);
    spi_write_data(0xd0);
    spi_write_data(0x05);
    spi_write_data(0x09);
    spi_write_data(0x10);
    spi_write_data(0x08);
    spi_write_data(0x14);
    spi_write_data(0x28);
    spi_write_data(0x33);
    spi_write_data(0x3f);
    spi_write_data(0x07);
    spi_write_data(0x13);
    spi_write_data(0x14);
    spi_write_data(0x28);
    spi_write_data(0x30);

    spi_write_cmd(0xe1);
    spi_write_data(0xd0);
    spi_write_data(0x05);
    spi_write_data(0x09);
    spi_write_data(0x09);
    spi_write_data(0x08);
    spi_write_data(0x03);
    spi_write_data(0x24);
    spi_write_data(0x32);
    spi_write_data(0x3b);
    spi_write_data(0x38);
    spi_write_data(0x14);
    spi_write_data(0x13);
    spi_write_data(0x28);
    spi_write_data(0x2f);

    spi_write_cmd(0x21); // flying 反色
    spi_write_cmd(0x29);
    //spi_write_cmd(0x2C);
}
void lcd_spi_deinit(void)
{
    close(g_SPI_Fd);
    _user_set_gpio_value(SPI_CS1_PIN, 1);
    _user_set_gpio_value(SPI_CS_PIN, 1); //SPI_CS_PIN=0;
}