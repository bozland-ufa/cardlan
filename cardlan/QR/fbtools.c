#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
//#include "page.h"
#include <linux/fb.h>
#include "fbtools.h"
//#include "../gui/RC500.h"
#define HZK16  "hzk16c_ASC.DZK"
#define HZK32  "hzk32c_ASC.DZK"

#ifndef uint8_t
#define uint8_t  	unsigned char
#define uint16_t 	unsigned short
#define uint32_t 	unsigned int
#endif

#if DEBUG
#define DBG_PRINTF					printf
#else
#define DBG_PRINTF(...)  
#endif


void uninit_backgroud(void);

//open & init a frame buffer
unsigned short Mcolor;
unsigned short Textcolor;
unsigned short TextSize;


static char *fbdevice = "/dev/fb0";
static char *hzkascii = NULL;
static char *hzkascii32 = NULL;
static unsigned char *fbuffer = NULL;
static unsigned char *fbuffera = NULL;
static unsigned char *backbuf = NULL;
#define WIDTH  				320
#define HEIGHT  			240


static int fb_fd;

//static int bits_per_pixel = 0;


/*
static const char signal_BMP[] =
{0x70,0x00,0x78,0x00,0x7E,0x00,0x6F,0x00,0x67,0x80,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,
0x67,0x80,0x6F,0x00,0x7E,0x00,0x7C,0x00,0x78,0x00,0x70,0x00,0x00,0x00,0x00,0x7F,
0x00,0x7F,0x00,0x7F,0x00,0x00,0x03,0xFF,0x03,0xFF,0x03,0xFF,0x00,0x00,0x3F,0xFF,
0x3F,0xFF,0x3F,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00};
*/
/*"signal.bmp*/



/*
*************************************************************************************************************
- Function name : static int hzkopen(void)
- Function instruction : Open font file     16*16
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
static int hzkopen(void)
{
	int file_len;
	int file_fd;
	struct stat buf;

	/*Open and calculate the file length*/
	// printf("playsound 01 \n");
	file_fd = open(HZK16, O_RDONLY);
	if(fstat(file_fd,&buf) < 0)
	{
		perror("HZK16 fstat error");
		return -1;
	}
	file_len = buf.st_size;
	hzkascii = (char*)malloc(file_len);
	if(hzkascii == NULL)
	{
		perror("hzkascii malloc error ");
		return -1;
	}
	read(file_fd,hzkascii,file_len);
	close(file_fd);
	return 0;
}



/*
*************************************************************************************************************
- Function name : static void hzkclose32(void)
- Function instruction : Open font file     32*32
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
static int hzkopen32(void)
{
	int file_len;
	int file_fd;
	struct stat buf;

	/*Open and calculate the file length*/
	// printf("playsound 01 \n");
	file_fd = open(HZK32, O_RDONLY);
	if(fstat(file_fd,&buf) < 0)
	{
		perror("HZK32 fstat error");
		return -1;
	}
	file_len = buf.st_size;
	hzkascii32= (char*)malloc(file_len);
	if(hzkascii32== NULL)
	{
		perror("hzkascii32 malloc error ");
		return -1;
	}
	read(file_fd,hzkascii32,file_len);
	close(file_fd);
	return 0;

}

/*
*************************************************************************************************************
- Function name : int open_framebuffer(void)
- Function instruction : Open FrameBuffer
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/


//#include "../gui/InitSystem.h"
void print_fb_info(void)
{
	
	DBG_PRINTF("\n=========fb_fix_screeninfo=============\n");
	DBG_PRINTF("fix.id = %s\n",fix.id);
	DBG_PRINTF("fix.smem_start = %d\n",fix.smem_start);
	DBG_PRINTF("fix.smem_len = %d\n",fix.smem_len);
	DBG_PRINTF("fix.type = %s\n",fix.type);
	DBG_PRINTF("fix.visual = %d\n",fix.visual);
	DBG_PRINTF("fix.line_length = %d\n",fix.line_length);
	DBG_PRINTF("fix.mmio_start = 0x%p\n",fix.mmio_start);
	DBG_PRINTF("fix.mmio_len = %d\n",fix.mmio_len);
	DBG_PRINTF("fix.accel = %d\n",fix.accel);
	DBG_PRINTF("========================================\n");


	DBG_PRINTF("\n=========var_screeninfo===============\n");
	DBG_PRINTF("var.xres = %d\n",var.xres);
	DBG_PRINTF("var.yres = %d\n",var.yres);
	DBG_PRINTF("var.xresvirtual = %d\n",var.xres_virtual);
	DBG_PRINTF("var.yresvirtual = %d\n",var.yres_virtual);
	DBG_PRINTF("var.xoffset = %d\n",var.xoffset);
	DBG_PRINTF("var.yoffset = %d\n",var.yoffset);
	DBG_PRINTF("var.bits_per_pixel = %d\n",var.bits_per_pixel);
	DBG_PRINTF("var.left_margin = %d\n",var.left_margin);
	DBG_PRINTF("var.right_margin = %d\n",var.right_margin);
	DBG_PRINTF("var.upper_margin = %d\n",var.upper_margin);
	DBG_PRINTF("var.lower_margin = %d\n",var.lower_margin);
	DBG_PRINTF("var.hsync_len = %d\n",var.hsync_len);
	DBG_PRINTF("var.vsync_len = %d\n",var.vsync_len);
	DBG_PRINTF("========================================\n");
	/*WIDTH = 320;
	HEIGHT = 240;*/

	
	
}



int open_framebuffer(void)
{
	int result;

	result = hzkopen();
	if (result == -1) {
		perror("open hzk16.DZK error!\n");
		return -1;
	}

	result = hzkopen32();
	if (result == -1) {
		perror("open hzk32.DZK error!\n");
		return -1;
	}

	fb_fd = open(fbdevice, O_RDWR);
	if (fb_fd == -1) {
		perror("open fbdevice");
		close(fb_fd);
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix) < 0) {
		perror("ioctl FBIOGET_FSCREENINFO");
		close(fb_fd);
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &var) < 0) {
		perror("ioctl FBIOGET_VSCREENINFO");
		close(fb_fd);
		return -1;
	}


	//printf("HEIGHT = %d,WIDTH = %d\n",HEIGHT, WIDTH);
	fbuffera = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fb_fd, 0);
	if (fbuffer == (unsigned char *)-1) {
		perror("mmap a framebuffer");
		close(fb_fd);
		return -1;
	}


	//printf("HEIGHT = %d,WIDTH = %d\n",HEIGHT, WIDTH);
	fbuffer = (unsigned char *)malloc(fix.smem_len);
	if (fbuffer == NULL) {
		perror("mmap b framebuffer");
		return -1;
	}

//	print_fb_info();
	//memset(fbuffer,0xff,fix.smem_len);
	return 0;
}

/*void init_dev(void)
{
	FBDEV fr_dev;
	fr_dev.fdfd = -1;
    fr_dev.fdfd = open("/dev/fb0",O_RDWR);
	fr_dev.

    ioctl(fr_dev.fdfd,FBIOSET_FSCREENINFO,&(fr_dev->finfo)); //Get fixed parameters

    ioctl(fr_dev.fdfd,FBIOSET_VSCREENINFO,&(fr_dev->vinfo)); //Get variable parameters

    fr_dev->screensize=fr_dev->vinfo.xres*fr_dev->vinfo.yres*fr_dev->vinfo.bits_per_pixel/8; 

    fr_dev->map_fb=(char *)mmap(NULL,fr_dev->screensize,PROT_READ|PROT_WRITE,MAP_SHARED,fr_dev->fdfd,0);

    printf("init_dev successfully.\n");
}
*/



void UpdataApp(int x1,int y1,int x2,int y2)
{
#if _L7
	int i,n,xa,ya;
	unsigned short *str1,*str2;

	xa = x2 - x1;
	ya = y2 - y1;

	for(i=0; i<ya; i++)
	{
		str1 = ((unsigned short *)fbuffera + (y1+i) * var.xres + x1);
		str2 = ((unsigned short *)fbuffer + (y1+i) * var.xres + x1);
		for(n =0; n<xa; n++)
		{
			if(str2[n] != str1[n])
			{
				str1[n] = str2[n];
			}
		}
	}
#else
	int w = y2 - y1;
	unsigned short *pDest, *pSrc;
	printf("(var.yres - x2 - 1)=%d\n",(var.yres - x2 - 1));
	pDest = (unsigned short *)fbuffera + (var.yres - x2 - 1) * var.xres + y1;
	pSrc = (unsigned short *)fbuffer + (var.yres - x2 - 1) * var.xres + y1;
	
	while (x2-- > x1) {
		memcpy((char *)pDest, (char *)pSrc, w * 2);
		pDest += var.xres;
		pSrc += var.xres;
	}
#endif

}


/*
*************************************************************************************************************
- Function name :void close_framebuffer(void)
- Function instruction : Close FrameBuffer
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void close_framebuffer(void)
{
	munmap(fbuffera, fix.smem_len);
	close(fb_fd);
	free(hzkascii);
	free(hzkascii32);
	free(fbuffer);
	uninit_backgroud();

}

/*
*************************************************************************************************************
- Function name :static int pixel(int x,int y,unsigned short color)
- Function instruction ://Display one pixel
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
static int pixel(int x,int y,unsigned short color)
{
	int offset;
	/*if((x >= HEIGHT)||(y >= WIDTH))*/
	if((x >= WIDTH)||(y >= HEIGHT))
		return (-1);
	
#if _L7
	offset = y * WIDTH + x;
#else
	offset = (WIDTH - 1 - x ) * HEIGHT + y;
#endif
	unsigned short *dst = ((unsigned short *)fbuffer + offset);
	*dst = color;
	return (0);
}



/*
*************************************************************************************************************
- Function name : void line (int x1, int y1, int x2, int y2, unsigned short colidx)
- Function instruction ://Line function
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void line (int x1, int y1, int x2, int y2, unsigned short colidx)
{
	int tmp;
	int dx = x2 - x1;
	int dy = y2 - y1;

	if (abs (dx) < abs (dy)) {
		if (y1 > y2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = -dx;
			dy = -dy;
		}
		x1 <<= 16;
		/* dy is apriori >0 */
		dx = (dx << 16) / dy;
		while (y1 <= y2) {
			pixel (x1 >> 16, y1, colidx);
			x1 += dx;
			y1++;
		}
	} else {
		if (x1 > x2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = -dx;
			dy = -dy;
		}
		y1 <<= 16;
		dy = dx ? (dy << 16) / dx : 0;
		while (x1 <= x2) {
			pixel (x1, y1 >> 16, colidx);
			y1 += dy;
			x1++;
		}
	}
}

/*
*************************************************************************************************************
- Function name : void rect (int x1, int y1, int x2, int y2, unsigned short colidx)
- Function instruction ://Rectangle
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void rect (int x1, int y1, int x2, int y2, unsigned short colidx)
{
	line (x1, y1, x2, y1, colidx);
	line (x2, y1, x2, y2, colidx);
	line (x2, y2, x1, y2, colidx);
	line (x1, y2, x1, y1, colidx);
}

/*
*************************************************************************************************************
- Function name : void fillrect (int x1, int y1, int x2, int y2, unsigned colidx)
- Function instruction ://Rectangular surface
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void fillrectcoli (int x1, int y1, int x2, int y2, unsigned short colidx,unsigned char *fbs)
{

	int i,n,xa,ya;

#if !_L7
	int w = y2 - y1;
	unsigned short *pDest;
	
	pDest = (unsigned short *)fbs + (var.yres - x2 - 1) * var.xres + y1;
	
	while (x2-- > x1) {
		for (i = 0; i < w; ++i) {
			/**(pDest + i) = backbuf[i];*/
			*(pDest + i) = colidx;
		}

		pDest += var.xres;
	}

#else
	unsigned short *str2;
	xa = x2 - x1;
	ya = y2 - y1;

	for(i=0; i<ya; i++)
	{
		str2 = ((unsigned short *)fbs + (y1+i) * var.xres + x1);
		for(n=0; n<xa; n++)
		{
			str2[n] = colidx;
		}
	}
#endif

}


/*
*************************************************************************************************************
- Function name : void fillrect (int x1, int y1, int x2, int y2, unsigned colidx)
- Function instruction ://Rectangular surface
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void fillrect (int x1, int y1, int x2, int y2, unsigned colidx)
{
	int tmp,i;
	/* Clipping and sanity checking */
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if (x1 < 0) x1 = 0;
	if ((__u32)x1 >= var.xres) x1 = var.xres - 1;
	if (x2 < 0) x2 = 0;
	if ((__u32)x2 >= var.xres) x2 = var.xres - 1;
	if (y1 < 0) y1 = 0;
	if ((__u32)y1 >= HEIGHT) y1 = HEIGHT - 1;
	if (y2 < 0) y2 = 0;
	if ((__u32)y2 >= HEIGHT) y2 = HEIGHT - 1;

	if ((x1 > x2) || (y1 > y2))
		return;
	tmp = y2 - y1;
	for(i = 0; i < tmp; i++)
	{
		line (x1, y1+i, x2, y1+i, colidx);
	}

}

/*
*************************************************************************************************************
- Function name : void put_hz(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
- Function instruction ://Start displaying a 16x16 Chinese character from x1, y1
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void put_hz(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
{
	int i,j,n;
	unsigned char list;
	n = 0;
	for(i = 0; i < 16; i++,n++)
	{
		list = dispBuf[n];
		for(j=0; j<8; j++)
		{
			if(list&(1<<(7-j)))
			{
				pixel(y1+j,x1+i, color);
			}

		}
		n++;
		list = dispBuf[n];
		for(j=0; j<8; j++)
		{
			if(list&(1<<(7-j)))
			{
				pixel(y1+8+j,x1+i,color);
			}
		}
	}
}

/*
*************************************************************************************************************
- Function name : void put_char(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
- Function instruction ://Start displaying a ASCII_8x16 from x1, y1
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void put_char(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
{
	int i,j,n;
	unsigned char list;
	n = 0;
	for(i = 0; i < 16; i++,n++)
	{
		list = dispBuf[n];
		for(j=0; j<8; j++)
		{
			if(list&(1<<(7-j)))
			{
				pixel(y1+j,x1+i, color);
			}

		}
	}
}

/*
*************************************************************************************************************
- Function name : static void _GetPixel(unsigned char QM, unsigned char WM, unsigned char *pxBuff)
- Function instruction :Return to the font data according to the location code
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
static void _GetPixel(unsigned char QM, unsigned char WM, unsigned char *pxBuff)
{
	unsigned int  Loff;
//unsigned char i;
//unsigned char loopt;

	if (WM == 0 )
	{
		Loff = (unsigned int)(QM) * (unsigned int)16 +(unsigned int)0x45080;
		memcpy(pxBuff,hzkascii+Loff,16);

		//fseek(hzk,Loff,SEEK_SET);
		//fread(pxBuff,16,1,hzk);

	}
	else
	{
		Loff = ( (unsigned int)(QM-0xa1) * (unsigned int)94 + (unsigned int)(WM-0xa1) ) * (unsigned int)32;
		//fseek(hzk,Loff,SEEK_SET);
		//fread(pxBuff,32,1,hzk);
		memcpy(pxBuff,hzkascii+Loff,32);
	}
}


/*
*************************************************************************************************************
- Function name :void put_string(int x1, int y1, char *pzText, unsigned color)
- Function instruction ://Display a string of characters 16 * 16 at x,y 
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void put_string(int x1, int y1, char *pzText, unsigned color)
{
	unsigned char i,QM,WM;
	unsigned char hzkbuf[32];
	unsigned int x,y;
	x = x1;
	y = y1;
	i = 0;
	while ( *(pzText+i) > 0 )
	{
	  if(x>=WIDTH)
		{y=y+16;x=0;}
		QM = *(pzText+i);
		if ( QM <= 0x80 )
		{	
		
			if((y < HEIGHT)&&(x < WIDTH))
			{
				_GetPixel(QM,0,hzkbuf);
				put_char(hzkbuf,y,x,color);
				x = x + 8;
			}
			i++;
		}
		else
		{
			if (i+1)
			{
				WM = *(pzText+i+1);
				if (WM > 0 )
				{
									
					if((y < HEIGHT)&&(x < WIDTH))
					{	
						_GetPixel(QM,WM,hzkbuf);
						put_hz(hzkbuf,y,x,color);
						x = x + 16;
					}						
					i++;
				}
				i++;
			}
			else i++;
		}
	
	}
}


/*
*************************************************************************************************************
- Function name :void put_string_center(int x, int y, char *s, unsigned colidx)
- Function instruction :
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void put_string_center(int x, int y, char *s, unsigned colidx)
{
	size_t sl = strlen (s);
	//put_string (x - (sl / 2) * font_vga_8x8.width,
	//            y - font_vga_8x8.height / 2, s, colidx);
	put_string (x - (sl / 2) * 8,
	            y - 16 / 2, s, colidx);

}

/*
******************************put_char32*******************************************************************************
- Function name :void _draw_model(char *pdata, int w, int h, int x, int y, int color)
- Function instruction ://Display a character at x, y
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void _draw_model(char *pdata, int w, int h, int x, int y, int color)
{
	int     i;    /* control line */
	int     j;    /* Controls 8 points in a row */
	int     k;    /* The first few "8 points" in a line */
	int     nc;   /* to the first few bytes of dot matrix data */
	int     cols; /* control column */

	w = (w + 7) / 8 * 8; /* Recalculate w */
	

	nc = 0;
	for (i=0; i< h; i++)
	{
		cols = 0;
		for (k=0; k<w/8; k++)
		{
			for (j=0; j<8; j++)
			{
				if(pdata[nc]&(1<<(7-j)))
				{
					pixel(x+cols, y+i, color);
				}
				cols++;
			}
			nc++;
		}
	}
}

/*
*************************************************************************************************************
- Function name : void _draw_char(char *hz, int x, int y, int w, int h, int color)
- Function instruction ://Display a character at x, y
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void _draw_char(char hz, int x, int y, int w, int h, int color)
{
	char fontbuf[128];   /* Large enough buffer, can also be dynamically allocated */
	/* Calculated offset */
	long offset = (long)(hz*64 + 0x114200);

// fseek(hzk32, offset, SEEK_SET); /* Addressing */
// fread(fontbuf,1,64, hzk32);   /* Read dot matrix data */
	memcpy(fontbuf,hzkascii32+offset,64);
	_draw_model(fontbuf, w, h, x, y, color);    /* Drawing a font */
}




/*
*************************************************************************************************************
- Function name : void _draw_hz(char *hz, int x, int y, int w, int h, int color)
- Function instruction ://Display a character at x, y
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void _draw_hz(char *hz, int x, int y, int w, int h, int color)
{

	char fontbuf[128];   /* Large enough buffer, can also be dynamically allocated */
	int ch0 = hz[0]-0xA0; /* Area code */
	int ch1 = hz[1]-0xA0; /* Bit code */

	/* Calculated offset */
	long offset = (long)(128 * ((ch0 - 1) * 94 + ch1 - 1));
//fseek(hzk32, offset, SEEK_SET);              /* Addressing */
//fread(fontbuf,1,(w + 7) / 8 * h, hzk32);   /* Read dot matrix data */
	memcpy(fontbuf,hzkascii32+offset,(w + 7) / 8 * h);
	_draw_model(fontbuf, w, h, x, y, color);    /* Drawing a font */

}

/*
*************************************************************************************************************
- Function name : void put_string32(int x1, int y1, char *pzText, unsigned color)
- Function instruction ://Display a character at x, y
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void put_string32(int x1, int y1, char *pzText, unsigned color)
{
	unsigned char i,QM,WM;
//unsigned char hzkbuf[128];
	unsigned int x,y;
	x = x1;
	y = y1;
	i = 0;
	while ( *(pzText+i) > 0 )
	{
		if(x>=WIDTH)
		  {y=y+32;x=0;}
		QM = *(pzText+i);
		if ( QM <= 0x80 )
		{
			if((y < HEIGHT)&&(x < WIDTH))
			{
				_draw_char(QM,x,y,16,32,color);
				x = x + 16;
			}
			i++;
		}
		else
		{
			if(i+1)
			{
				WM = *(pzText+i+1);
				if (WM > 0 )
				{
					if((y < HEIGHT)&&(x < WIDTH))
					{
						char buff[2];
						buff[0] = QM;
						buff[1] = WM;
						//put_hz32(hzkbuf,y,x,color);
						_draw_hz(buff,x,y,32,32,color);
						x = x + 32;
					}
					i++;
				}
				i++;
			}
			else i++;
		}
	}
}

long chartolong( char * string, int length )
{
	long number;

	if (length <= 4)
	{
		memset( &number, 0x00, sizeof(long) );
		memcpy( &number, string, length );
	}

	return( number );
}


/*
*************************************************************************************************************
- Function name : void InitDisplay(void)
- Function instruction :
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/

int load_backm(int x, int y,uint8_t *buf)
{
	FILE *fp;
	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	int rc;
	int ciBitCount, ciWidth, ciHeight;
	int line_x, line_y;
	long int location = 0;

#if !_F26
	if((x >= var.xres)||(y >= var.yres))
		return (-1);
#endif
	/* Open bitmap file */
	fp = fopen("/var/run/back.bmp", "rb" );
	if (fp == NULL)
	{
		return( -1 );
	}

	/* Read bitmap file header */
	rc = fread( &FileHead, sizeof(BITMAPFILEHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read header error!\n");
		fclose( fp );
		return( -2 );
	}

	/* Determine the type of bitmap */
	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose( fp );
		return( -3 );
	}

	/* Read bitmap information header */
	rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read infoheader error!\n");
		fclose( fp );
		return( -4 );
	}

	ciWidth    = (int) chartolong( InfoHead.ciWidth,	4 );
	ciHeight   = (int) chartolong( InfoHead.ciHeight,	4 );
	ciBitCount = (int) chartolong( InfoHead.ciBitCount, 4 );

	fseek(fp, (int)chartolong(FileHead.cfoffBits, 4), SEEK_SET);


	/*printf("width=%d, height=%d, bitCount=%d, offset=%d\n", ciWidth, ciHeight, ciBitCount, (int)chartolong(FileHead.cfoffBits, 4));
*/
	line_x = line_y = 0;
	while(!feof(fp))
	{
		PIXEL pix;
		unsigned short int tmp;
		rc = fread( (char *)&pix, 1, sizeof(unsigned short int), fp );
		if (rc != sizeof(unsigned short int) )
		{
			break;
		}
		location = line_x * var.bits_per_pixel / 8 + (ciHeight - line_y - 1) * var.xres * var.bits_per_pixel / 8;

		tmp=pix.red<<11 | pix.green<<6 | pix.blue;
		*((unsigned short int*)(buf + y * var.xres + x + location)) = tmp;

		line_x++;
		if (line_x == ciWidth )
		{
			line_x = 0;
			line_y++;

			if(line_y==ciHeight-1)
			{
				break;
			}
		}
	}
	fclose( fp );
	DBG_PRINTF("show bmp.\n");
	/*UpdataApp(var.xres-x,var.yres-y,100,y+32);*/
	return( 0 );

}
void InitDisplay(void)
{

	open_framebuffer();
	SetColor(Color_black);
	Mcolor = Color_blue;
	Textcolor = Color_white;
	TextSize = 16;
	init_backgroud();
}

void init_backgroud(void)
{
	backbuf = (unsigned char *)malloc(fix.smem_len);
	if (backbuf == NULL) {
		perror("mmap b backbuf error");
		return ;
	}
	if (load_backm(0,0,backbuf) == 0) {
		memcpy((char *)fbuffera, (char *)backbuf,fix.smem_len);
	} else {
		uninit_backgroud();
	}
}

void uninit_backgroud(void)
{
	if (backbuf) {
		free(backbuf);
		backbuf = NULL;
	}
}
/*
*************************************************************************************************************
- Function name : void CloseDisplay(void)
- Function instruction :
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void CloseDisplay(void)
{
	close_framebuffer();
}



/*
*************************************************************************************************************
- Function name : void TextOut(int x1, int y1, char *pzText)
- Function instruction :
- input paramater : null
- output parameter: null

//put_string(50,100,"�����Ƽ����޹�˾",0);
//put_string32(50,100,"�����Ƽ����޹�˾",0);
*************************************************************************************************************
*/


void _TextOut(int x1, int y1, char *pzText)
{
	int lenx;
	lenx = strlen(pzText);


	if(TextSize <= 16)
	{
		lenx = lenx*8;
		if (backbuf)
			memcpy((char *)fbuffer, (char *)backbuf,fix.smem_len);
		else
		fillrectcoli(x1,y1,x1+lenx,y1+HEIGHT,Mcolor,fbuffer);
		put_string(x1,y1,pzText,Textcolor);
		UpdataApp(x1,y1,x1+lenx,y1+HEIGHT);
	}
	else
	{
		lenx = lenx*16;
		//printf("screen size=%d\n",fix.smem_len);
		if (backbuf)
			memcpy((char *)fbuffer, (char *)backbuf,fix.smem_len);
		else		
		{
		lenx=20*16;
		fillrectcoli(x1,y1, x1+lenx,y1+HEIGHT,Mcolor,fbuffer);
		}
		/*fillrectcoli(x1,y1,x1+lenx,y1+32,Mcolor,fbuffer);*/
		put_string32(x1,y1,pzText,Textcolor);
		UpdataApp(x1,y1,x1+lenx,y1+HEIGHT);
	

	}
}




/*
*************************************************************************************************************
- Function name : void SetTextSize(unsigned short size)
- Function instruction :
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void SetTextSize(unsigned short size)
{
	TextSize = size;
}

/*
*************************************************************************************************************
- Function name : void SetTextColor(unsigned short color)
- Function instruction :
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void SetTextColor(unsigned short color)
{
	Textcolor = color;
}
#if _F26
void set_text_white(void)
{
	SetTextColor(Color_white);
}

#endif

/*
*************************************************************************************************************
- Function name : unsigned short RGB565(unsigned char r,unsigned g,unsigned char b)
- Function instruction ://Color function
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
unsigned short RGB565(unsigned char r,unsigned g,unsigned char b)
{
//RGB---------565
	unsigned short rgb;
	rgb = (unsigned short)(((r&0x1f) << 11)|((g&0x3f) << 5)|(b&0x1f));
	return rgb;
}

/*
*************************************************************************************************************
- Function name : void SetColor(unsigned short color)
- Function instruction ://Control the color function of the overall screen
- input paramater : null
- output parameter: null
*************************************************************************************************************
*/
void SetColor(unsigned short color)
{
	unsigned short *cor;
	unsigned int i,ch;

#if 0
	i = 0;
	ch = fix.smem_len / 2;
	cor = (unsigned short *)fbuffera;
	do
	{
		*cor++ = color;
		i++;
	} while(i < ch);
#else
	if (backbuf) 
		memcpy((char *)fbuffera, (char *)backbuf,fix.smem_len);
	else {
		i = 0;
		ch = fix.smem_len / 2;
		cor = (unsigned short *)fbuffera;
		do
		{
			*cor++ = color;
			i++;
		} while(i < ch);
	}

#endif
}



/******************************************************************************
*
******************************************************************************/

/******************************************************************************
*
******************************************************************************/

int Show_BMP(int x,int y,char *bmpfile)
{
	FILE *fp;
	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	int rc;
	int ciBitCount, ciWidth, ciHeight;
	int line_x, line_y;
	long int location = 0;

#if !_F26
	if((x >= var.xres)||(y >= var.yres))
		return (-1);
#endif
	/* Open bitmap file */
	fp = fopen( bmpfile, "rb" );
	if (fp == NULL)
	{
		return( -1 );
	}

	/* Read bitmap file header */
	rc = fread( &FileHead, sizeof(BITMAPFILEHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read header error!\n");
		fclose( fp );
		return( -2 );
	}

	/* Determine the type of bitmap */
	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose( fp );
		return( -3 );
	}

	/* Read bitmap information header */
	rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read infoheader error!\n");
		fclose( fp );
		return( -4 );
	}

	ciWidth    = (int) chartolong( InfoHead.ciWidth,	4 );
	ciHeight   = (int) chartolong( InfoHead.ciHeight,	4 );
	ciBitCount = (int) chartolong( InfoHead.ciBitCount, 4 );

	fseek(fp, (int)chartolong(FileHead.cfoffBits, 4), SEEK_SET);


	//printf("width=%d, height=%d, bitCount=%d, offset=%d\n", ciWidth, ciHeight, ciBitCount, (int)chartolong(FileHead.cfoffBits, 4));

	line_x = line_y = 0;
	while(!feof(fp))
	{
		PIXEL pix;
		unsigned short int tmp;
		rc = fread( (char *)&pix, 1, sizeof(unsigned short int), fp );
		if (rc != sizeof(unsigned short int) )
		{
			break;
		}
		location = line_x * var.bits_per_pixel / 8 + (ciHeight - line_y - 1) * var.xres * var.bits_per_pixel / 8;

		tmp=pix.red<<11 | pix.green<<6 | pix.blue;
		*((unsigned short int*)(fbuffera + y * var.xres + x + location)) = tmp;
		/**((unsigned short int*)(fbuffera + y * var.xres + x + location)) = tmp;*/

		line_x++;
		if (line_x == ciWidth )
		{
			line_x = 0;
			line_y++;

			if(line_y==ciHeight-1)
			{
				break;
			}
		}
	}
	fclose( fp );
	/*DBG_PRINTF("show bmp.\n");*/
	/*UpdataApp(var.xres-x,var.yres-y,100,y+32);*/
	return( 0 );
}

