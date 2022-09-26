#ifndef __FBTOOLS_H
#define __FBTOOLS_H

//#define  Mcolor   RGB565(50,50,60)

#define Color_black        0       // Black
/*#define Color_blue    RGB565(79,192,245)  //blue*/
#define Color_blue    RGB565(0,0,255)  //blue
#define Color_red     RGB565(255,0,0)  //red
#define Color_green   RGB565(0,255,0)  //green
#define Color_purple  RGB565(255,0,255)  //purple
#define Color_yellow  RGB565(255,255,0)  //yellow
#define Color_white   RGB565(255,255,255) //white

//14byte
typedef struct
{
  char cfType[2];         /* file type,must be"BM" (0x4D42)*/
  char cfSize[4];         /* size of the file(byte) */
  char cfReserved[4];     /* Reserve must be o */
  char cfoffBits[4];      /* The offset of the bitmap array relative to the file header (bytes)*/
}__attribute__((packed)) BITMAPFILEHEADER;       /* File header structure */

//40byte
typedef struct
{
  char ciSize[4];         /* size of BITMAPINFOHEADER */
  char ciWidth[4];        /* Bitmap width (pixels) */
  char ciHeight[4];       /* Bitmap height (pixels) */
  char ciPlanes[2];       /* The number of bit planes of the target device must be set to 1 */
  char ciBitCount[2];     /* Number of bits per pixel, 1, 4, 8 or 24 */
  char ciCompress[4];     /* Bitmap array compression method, 0=no compression */
  char ciSizeImage[4];    /* Image size (bytes) */
  char ciXPelsPerMeter[4];/* Target device level per pixel */
  char ciYPelsPerMeter[4];/* Target device vertical pixels per meter */
  char ciClrUsed[4];      /* The number of colors in the color table actually used by the bitmap */
  char ciClrImportant[4]; /* Number of important color indexes */
}__attribute__((packed)) BITMAPINFOHEADER;       /* Bitmap information header structure */



typedef struct
{
  unsigned short blue:5;
  unsigned short green:5;
  unsigned short red:5;
  unsigned short rev:1;
}__attribute__((packed)) PIXEL;



typedef struct MYBMPFILE {
	int ciWidth;
 	int ciHeight;
    char *bmpfile;
}MYBMPFILE;


//extern int Lond_BMP(char *bmpfile,MYBMPFILE*fpb);
//extern int reLond_BMP(MYBMPFILE *fpb);
//extern int Show_bmp_file(int x,int y,MYBMPFILE *fpb);



extern unsigned short Mcolor;
extern unsigned short Textcolor;
extern unsigned short TextSize;

 static struct fb_fix_screeninfo fix;
 static struct fb_var_screeninfo var;


extern int open_framebuffer(void);
extern void close_framebuffer(void);
extern void line (int x1, int y1, int x2, int y2, unsigned short colidx);
extern void rect (int x1, int y1, int x2, int y2, unsigned short colidx);
extern void fillrect (int x1, int y1, int x2, int y2, unsigned colidx);
extern void put_hz(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color);
extern void put_char(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color);
extern void put_string(int x1, int y1, char *pzText, unsigned color);
extern void put_string_center(int x, int y, char *s, unsigned colidx);
extern unsigned short RGB565(unsigned char r,unsigned g,unsigned char b);
extern void SetColor(unsigned short color);


extern void _draw_model(char *pdata, int w, int h, int x, int y, int color);
extern void _draw_char(char hz, int x, int y, int w, int h, int color);
extern void _draw_hz(char *hz, int x, int y, int w, int h, int color);
extern void put_string32(int x1, int y1, char *pzText, unsigned color);

extern void InitDisplay(void);
extern void CloseDisplay(void);
extern void ClearAll(void);
extern void ClearWin(int x,int y,int x1,int y1);
extern void SetTextColor(unsigned short color);
extern void SetTextSize(unsigned short size);
extern void _TextOut(int x1, int y1, char *pzText);
extern int Show_BMP(int x,int y,char *bmpfile);
extern void init_backgroud(void);
extern void print_fb_info(void);
#define TextOut(x,y,t) 			_TextOut((x),(y),(t))


#endif
