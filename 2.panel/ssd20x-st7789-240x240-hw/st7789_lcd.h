#ifndef _ST7789_LCD_H
#define _ST7789_LCD_H

#define  SPI_CS_PIN                  (85) /*屏幕片选*/
#define  SPI_CS1_PIN				 (86) /*MCU片选*/

#define  PANEL_RST_PIN               (42) /*屏幕复位*/
#define  PANEL_ENABLE_PIN            (0)  /*背光使能*/
#define  PANEL_BL_PIN				 (4)  /*背光PWM*/


void lcd_spi_init(void);
void lcd_spi_deinit(void);

#endif