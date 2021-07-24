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

static int bitwise_pixels_get( const bitwise_pixels* pxl, int idx ) {
    if (idx==0) return pxl->i0;
    if (idx==1) return pxl->i1;
    if (idx==2) return pxl->i2;
    if (idx==3) return pxl->i3;
    if (idx==4) return pxl->i4;
    if (idx==5) return pxl->i5;
    if (idx==6) return pxl->i6;
    if (idx==7) return pxl->i7;
    return 0;
}

static void bitwise_pixels_toggle_xy( bitwise_pixels* pxl, int x, int y, unsigned int width, unsigned int height ) {
    int nbytes_per_row = width/8 + (width%8>=1);
    int byte_idx = x/8 + y*nbytes_per_row;
    int bit_idx = x%8;
    bitwise_pixels_set( pxl + byte_idx, bit_idx, !bitwise_pixels_get( pxl + byte_idx, bit_idx ) );
}

static void bitwise_pixels_set_xy( bitwise_pixels* pxl, int x, int y, unsigned int width, unsigned int height, int val ) {
    int nbytes_per_row = width/8 + (width%8>=1);
    int byte_idx = x/8 + y*nbytes_per_row;
    int bit_idx = x%8;
    bitwise_pixels_set( pxl+byte_idx, bit_idx, val );
}

static int bitwise_pixels_get_xy( const bitwise_pixels* pxl, int x, int y, unsigned int width, unsigned int height ) {
    int nbytes_per_row = width/8 + (width%8>=1);
    int byte_idx = x/8 + y*nbytes_per_row;
    int bit_idx = x%8;
    return bitwise_pixels_get( pxl+byte_idx, bit_idx );
}

static void bitwise_pixels_edges_set( const bitwise_pixels* lock,
        bitwise_pixels* mask, unsigned int width, unsigned int height,
        int edgewidth ) {
    for (int x=0; x<width; ++x) for (int y=0; y<height; ++y) {
        int colored = bitwise_pixels_get_xy( lock, x, y, width, height );
        if (!colored) {
            for (int i=-edgewidth; i<=edgewidth; ++i) for (int j=-edgewidth; j<=edgewidth; ++j)
                bitwise_pixels_set_xy( mask, (x+i+width)%width, (y+j+height)%height, width, height, 0 );
        }
    }
}

static void bitwise_pixels_toggle( bitwise_pixels* pxl, unsigned int width, unsigned int height ) {
    for (int x=0; x<width; ++x) for (int y=0; y<height; ++y)
        bitwise_pixels_toggle_xy( pxl, x, y, width, height );
}

void xtrlock_bitmap_read( const char* file, int edgewidth ) {
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

        bitwise_pixels_edges_set( (const bitwise_pixels*)lock_bits,
                (bitwise_pixels*)mask_bits, width, height, edgewidth );
        bitwise_pixels_toggle( (bitwise_pixels*) mask_bits, width, height );

        if (!err) free_memory = 1;
        else xtrlock_bitmap_read(NULL,0);
    }
}

void xtrlock_bitmap_cleanup( void ) {
    if (free_memory) {
        free( lock_bits );
        free( mask_bits );
    }
}
