#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include <stddef.h>
#include <execinfo.h>

#include "sstardisp.h"
#include "st7789_lcd.h"

#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)

static MI_DISP_PubAttr_t stDispPubAttr;

int fb_test()
{
	int fbfd, fbsize, i;
	int red, green, blue;
	unsigned char *fbbuf, *fbP;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo; 

	/* Open video memory */
	if ((fbfd = open("/dev/fb0", O_RDWR)) < 0) {
		exit(1);
	} 

	/* Get variable display parameters */
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Bad vscreeninfo ioctl/n");
		exit(2);
	}

	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1){
		printf("Bad fscreeninfo ioctl/n");
		exit(2);
	}

	/* Size of frame buffer */
	fbsize = vinfo.xres*vinfo.yres*(vinfo.bits_per_pixel/8);  //finfo.smem_len;

	/* Print fb information */
	printf("********Fb info Start*******\n");
	printf("xres  :%d\n",vinfo.xres);
	printf("yres  :%d\n",vinfo.yres);
	printf("bpp   :%d\n",vinfo.bits_per_pixel);
	printf("fbsize:%d\n",finfo.smem_len);
	printf("*********Fb info End********\n");

	/* Map video memory */
	if ((fbP = (unsigned char *)mmap(0, fbsize, PROT_READ | PROT_WRITE,
		MAP_SHARED, fbfd, 0)) == (void *) -1) {
		exit(3);
	}

	fbbuf = fbP;

	unsigned short *pfbshort = (unsigned short *)fbbuf;

	for(i = 0; i<(fbsize/2); i++) {
		if(i< vinfo.xres*vinfo.yres/3)
		{
			*pfbshort = 0xf800;
			pfbshort ++;
		}
		else  if(i< vinfo.xres*vinfo.yres*2/3 && (i>vinfo.xres*vinfo.yres/3 || i==vinfo.xres*vinfo.yres/3))
		{
			*pfbshort = 0x07e0;
			pfbshort ++; 
		}
		else
		{
			*pfbshort = 0x001f;
			pfbshort ++; 
		}
	}
	fflush(stdout);
	sleep(3);


	return 0;
}



int main(int argc, const char *argv[]) 
{  
    stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;
    stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    stDispPubAttr.u32BgColor = YUYV_BLACK;
	lcd_spi_init();

	sstar_disp_init(&stDispPubAttr);
    fb_test();
	
	lcd_spi_deinit();
	do{
		sleep(-1);
	}while(1);

	//sstar_disp_Deinit(&stDispPubAttr);

    return 0;
}
