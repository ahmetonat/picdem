/**
** This file is part of fsusb_demo
**
** fsusb_demo is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of the
** License, or (at your option) any later version.
**
** fsusb_demo is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with fsusb_picdem; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301, USA
*/

/**
A USB interface to the Microchip PICDEM FS USB Demo board

Manuel Bessler, m.bessler AT gmx.net, 20050619

Based on usb_pickit.c :
Orion Sky Lawlor, olawlor@acm.org, 2003/8/3
Mark Rages, markrages@gmail.com, 2005/4/1

Simple modification by Xiaofan Chen, 20050811
Email: xiaofan AT sg dot pepperl-fuchs dot com
Note: I am in the process of trying to port it to Windows 
using libusb_win32 and MinGW.
By the way, I do not know where I got the original source.

Notes by Xiaofan Chen, 20050812
Okay I got the reply from the author. The code comes from here:
http://www.varxec.net/picdem_fs_usb/ 
Now the code works with Win32. Tested on Windows XP SP2.

*/
#include <usb.h> /* libusb header */
#include <stdio.h>
#include <string.h>

#define READ_VERSION     0x00
#define READ_VERSION_LEN 0x02

#define ID_BOARD         0x31
#define UPDATE_LED       0x32
#define SET_TEMP_REAL    0x33
#define RD_TEMP          0x34
#define SET_TEMP_LOGGING 0x35
#define RD_TEMP_LOGGING  0x36
#define RD_POT           0x37
#define RESET            0xFF


/* PICDEM FS USB max packet size is 64-bytes */
const static int reqLen=64;
//typedef unsigned char byte;
typedef char byte;

/*** Extern declarations of USB comm functions ***/
extern void bad(const char *why);
extern void send_usb(struct usb_dev_handle *, int, const char *);
extern void recv_usb(struct usb_dev_handle * d, int len, byte * dest);
struct usb_dev_handle *usb_picdem_fs_usb_open(void);
/************************************************/

void picdem_fs_usb_read_version(struct usb_dev_handle * d)
{
   byte resp[reqLen];
   byte query[reqLen];
   query[0] = READ_VERSION;
   query[1] = READ_VERSION_LEN;
   send_usb(d, 2, query);
   recv_usb(d, 2+READ_VERSION_LEN, resp);
   if( (int)resp[0] == READ_VERSION && (int)resp[1] == READ_VERSION_LEN )
    printf("Onboard firmware version is %d.%d\n",(int)resp[3],(int)resp[2]);
}

void picdem_fs_usb_led(struct usb_dev_handle * d, int lednum, int onoff)
{
   if( lednum < 3 || lednum > 4 )
	  return;
   byte answer[reqLen];
   byte question[reqLen];
   question[0] = UPDATE_LED;
   question[1] = lednum;
   question[2] = (onoff==0) ? 0 : 1;
   send_usb(d, 3, question);
   recv_usb(d, 1, answer);
   printf("LED #%i is now %s\n", lednum, (onoff==0) ? "off" : "on");
}

void picdem_fs_usb_readtemp(struct usb_dev_handle * d)
{
   byte answer[reqLen];
   byte question[reqLen];
   int rawval,rawtemp;
   float degCtemp;
   question[0] = RD_TEMP;
   send_usb(d, 1, question);
   recv_usb(d, 3, answer);
   rawval = answer[1] + (answer[2]<<8);
   
   /* improved temperature conversion by Bill Freeman */
   if( rawval & (1 << 15) )
         rawval -= 1 << 16;
			 
/*   if( (rawval>>15) & 0x01 )
	  rawval = (~rawval) + 1; */
   rawtemp = (rawval)>>3;
   degCtemp = rawtemp * 0.0625;
   printf("Temperature now is %f degC (raw: %i, rawval: %i)\n", degCtemp, rawtemp, rawval);
}

void picdem_fs_usb_readpot(struct usb_dev_handle * d)
{
   byte answer[reqLen];
   byte question[reqLen];
   int rawval;
   question[0] = RD_POT;
   send_usb(d, 1, question);
   recv_usb(d, 3, answer);
   rawval = answer[1] + (answer[2]<<8);
   printf("Potentiometer now reads %i\n", rawval);
}

void picdem_fs_usb_reset(struct usb_dev_handle * d)
{
//   byte answer[reqLen];
   byte question[reqLen];
   question[0] = RESET;
   send_usb(d, 1, question);
//   recv_usb(d, 3, answer);
   printf("Board resetted\n");
}


void show_usage(void)
{
  printf("fsusb_demo: Software for PICDEM Full Speed USB demo board\n");
  printf("fsusb_demo --ledon n, n=3 or 4: ON LED D3 or D4\n");
  printf("fsusb_demo --ledff n, n=3 or 4: Off LED D3 or D4\n");
  printf("fsusb_demo --readtemp: read out temperature\n");
  printf("fsusb_demo --readpot: read out poti value\n");
  printf("fsusb_demo --reset: to reset the PICDEM FS USB demo board\n");
}

int main(int argc, char ** argv) 
{
   struct usb_dev_handle * picdem_fs_usb = usb_picdem_fs_usb_open();
   if(argc < 2 || argc > 3) {
    show_usage();
    exit(1);
  } 
  if(argc == 3){
	  if( 0 == strcmp(argv[1],"--ledon") )
	  {
		 picdem_fs_usb_led(picdem_fs_usb, atoi(argv[2]), 1);
	  }
	  else if( 0 == strcmp(argv[1],"--ledoff") )
	  {
		 picdem_fs_usb_led(picdem_fs_usb, atoi(argv[2]), 0);	 
	  }
	 }
   
   if( argc == 2 ){
	  if(  0 == strcmp(argv[1],"--readtemp") )
	  {
		 picdem_fs_usb_readtemp(picdem_fs_usb);
	  }
	  else if(  0 == strcmp(argv[1],"--readpot") )
	  {
		 picdem_fs_usb_readpot(picdem_fs_usb);
	  }
	  else if(  0 == strcmp(argv[1],"--reset") )
	  {
		 picdem_fs_usb_reset(picdem_fs_usb);
	  }
	}
   usb_close(picdem_fs_usb);
   return 0;
}
