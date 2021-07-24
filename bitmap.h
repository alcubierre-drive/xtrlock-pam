#ifndef XTRLOCK_BITMAP_READER_H
#define XTRLOCK_BITMAP_READER_H

extern unsigned char* lock_bits;
extern int lock_width, lock_height;

extern int lock_x_hot, lock_y_hot;

extern unsigned char* mask_bits;
extern int mask_width, mask_height;

void xtrlock_bitmap_read( const char* file, int edgewidth );
void xtrlock_bitmap_cleanup( void );

#endif // XTRLOCK_BITMAP_READER_H
