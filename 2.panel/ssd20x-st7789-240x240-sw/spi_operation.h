#ifndef __SPI_OPERATION__H__
#define __SPI_OPERATION__H__

#include "mi_common_datatype.h"

#define  SPI_CS_PIN                  (85) /*屏幕片选*/
#define  SPI_CS1_PIN				 (86) /*MCU片选*/
#define  SPI_CLK_PIN                 (9)
#define  SPI_DI_PIN                  (10)

#define  PANEL_BL_PIN				 (4)
#define  PANEL_RST_PIN               (42)
#define  PANEL_ENABLE_PIN            (0)

void user_spi_init(void);
void user_spi_cmd(unsigned char value);
void user_spi_data(unsigned char value);

#endif