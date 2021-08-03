#ifndef __JPEG_H__
#define __JPEG_H__

#include<stdio.h> 
#include "../logo.h"

#include <jerror.h>
#include "jpeglib.h"
#include <setjmp.h>
struct my_error_mgr {
    struct jpeg_error_mgr pub;	/* "public" fields */

    jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr *my_error_ptr;
int load_logo_jpeg(char* frameBuffer,BITMAP* fb,FILE* fp);

#endif
