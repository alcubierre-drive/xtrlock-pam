#include "bitmap.h"
#include "awesome_bitmap.h"
#include "awesome_mask_bitmap.h"

#include <X11/Xlib.h>
#include <stddef.h>
#include <stdlib.h>

unsigned char* lock_bits = NULL;
int lock_width = 0,
    lock_height = 0;

int lock_x_hot = 0,
    lock_y_hot = 0;

unsigned char* mask_bits = NULL;
int mask_width = 0,
    mask_height = 0;

static int free_memory = 0;

typedef struct {
    unsigned char i0:1;
    unsigned char i1:1;
    unsigned char i2:1;
    unsigned char i3:1;
    unsigned char i4:1;
    unsigned char i5:1;
    unsigned char i6:1;
    unsigned char i7:1;
} bitwise_pixels;

static void bitwise_pixels_true( bitwise_pixels* pxl, int idx ) {
    if (idx==0) pxl->i0=1;
    if (idx==1) pxl->i1=1;
    if (idx==2) pxl->i2=1;
    if (idx==3) pxl->i3=1;
    if (idx==4) pxl->i4=1;
    if (idx==5) pxl->i5=1;
    if (idx==6) pxl->i6=1;
    if (idx==7) pxl->i7=1;
}

static void bitwise_pixels_false( bitwise_pixels* pxl, int idx ) {
    if (idx==0) pxl->i0=0;
    if (idx==1) pxl->i1=0;
    if (idx==2) pxl->i2=0;
    if (idx==3) pxl->i3=0;
    if (idx==4) pxl->i4=0;
    if (idx==5) pxl->i5=0;
    if (idx==6) pxl->i6=0;
    if (idx==7) pxl->i7=0;
}

static void bitwise_pixels_set( bitwise_pixels* pxl, int idx, int val ) {
    if (val) bitwise_pixels_true( pxl, idx );
    else bitwise_pixels_false( pxl, idx );
}

void xtrlock_bitmap_read( const char* file ) {
    if (file == NULL) {
        lock_bits = _lock_bits;
        lock_width = _lock_width;
        lock_height = _lock_height;
        mask_bits = _mask_bits;
        mask_width = _mask_width;
        mask_height = _mask_height;
        lock_x_hot = _lock_x_hot;
        lock_y_hot = _lock_y_hot;
    } else {
        unsigned int width, height;
        unsigned char *pixels = NULL;
        int x_hot, y_hot;
        int err = XReadBitmapFileData( file, &width, &height, &pixels, &x_hot, &y_hot );

        lock_bits = pixels;
        lock_width = width;
        lock_height = height;
        lock_x_hot = x_hot;
        lock_y_hot = y_hot;

        err += XReadBitmapFileData( file, &width, &height, &pixels, &x_hot, &y_hot );
        mask_bits = pixels;
        mask_width = width;
        mask_height = height;

        int nbytes_per_row = width/8 + (width%8>=1);
        for (int y=0; y<height; ++y) for (int x=0; x<width; ++x) {
            int byte_idx = x/8 + y*nbytes_per_row;
            int bit_idx = x%8;
            int pixel_on = (x==0 || x==width-1 || y==0 || y==height-1);
            bitwise_pixels_set( (bitwise_pixels*)(mask_bits+byte_idx), bit_idx, !pixel_on );
        }

        if (!err) free_memory = 1;
        else xtrlock_bitmap_read(NULL);
    }
}

void xtrlock_bitmap_cleanup( void ) {
    if (free_memory) {
        free( lock_bits );
        free( mask_bits );
    }
}
