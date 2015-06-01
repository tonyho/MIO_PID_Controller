#ifndef LCDCONF_H
#define LCDCONF_H


#define LCD_XSIZE          (400)		/* 320 -> 400 */
#define LCD_YSIZE          (240)		/* 240 */
#define LCD_CONTROLLER     (9320)		/* 9320 -> 5420 */
#define LCD_BITSPERPIXEL   (16)
#define LCD_FIXEDPALETTE   (565)
#define LCD_SWAP_RB        (1)
//#define LCD_SWAP_XY        (1)
//LCD_MIRROR_X
#define LCD_INIT_CONTROLLER()  ili9320_Initializtion();
#endif /* LCDCONF_H */

