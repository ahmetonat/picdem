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
#include "usblin.h"  //Functions for USB Linux
#include "sub2.h"    //Functions for SUB2


void picdem_fs_usb_read_version(struct usb_dev_handle * d)
{
   byte resp[reqLen];
   byte query[reqLen];
   query[0] = READ_VERSION;
   query[1] = READ_VERSION_LEN;
   send_usb(d, 2, query);
   recv_usb(d, 2+READ_VERSION_LEN, resp);
   if( (int)resp[0] == READ_VERSION && (int)resp[1] == READ_VERSION_LEN )
    printf("USB firmware stack version is %d.%d\n",(int)resp[3],(int)resp[2]);
}  //END: Read version.
/****************************************************************************/ 


int setPWM (unsigned char motor_no, int PWM_value, struct usb_dev_handle * USB_handle) 
{ 
    int RecvLength; 
    int SendLength; 
    //DWORD Comm_OK=0; 
    byte send_buf[64],receive_buf[64]; 
    int retval=0; 
    int errchk; 
 
    send_buf[0]=SET_PWM;              //Set main command. 
     
    if (motor_no==1)                  //Insert motor number in the packet. 
       send_buf[1]=1; 
    else if (motor_no==2) 
       send_buf[1]=2; 
    else { 
       printf("setPWM: Motor number is out of range: %d\n", motor_no); 
       send_buf[1]=0; 
       retval=1; 
       } 
        
     
    if (PWM_value>1023){            //Check for overflow in PWM value. 
       printf("setPWM: PWM value is out of range: %d\n", PWM_value); 
       PWM_value =1023; 
       retval=1; 
       } 
    if (PWM_value < -1023){ 
       printf("setPWM: PWM value is out of range: %d\n", PWM_value); 
       PWM_value =-1023; 
       retval=1; 
       }     
        
    if (PWM_value <0){              //Insert direction in the packet. 
       send_buf[2]=1;   //Negative 
       PWM_value = PWM_value*-1;     //Set PWM_value as a positive number. 
       } 
    else 
       send_buf[2]=0; 
        
    send_buf[3]=(byte)(PWM_value%256); //index 3 is low value. 
    send_buf[4]=(byte)(PWM_value/256); //index 4 is high value. 

    
    /*
    printf ("Send buffer value 0= %d\n",(unsigned char)send_buf[0]);
    printf ("Send buffer value 1= %d\n",(unsigned char)send_buf[1]);
    printf ("Send buffer value 2= %d\n",(unsigned char)send_buf[2]);
    printf ("Send buffer value 3= %d\n",(unsigned char)send_buf[3]);
    printf ("Send buffer value 4= %d\n",(unsigned char)send_buf[4]);
    */    
    
    SendLength=5;                    //5 bytes in the outbound packet. 
    RecvLength=3; 
    
    send_usb(USB_handle, SendLength, send_buf);
    recv_usb(USB_handle, RecvLength, receive_buf);
    
    /*
    printf ("Buffer value 0= %d\n",(unsigned char)receive_buf[0]);
    printf ("Buffer value 1= %d\n",(unsigned char)receive_buf[1]);
    printf ("Buffer value 2= %d\n",(unsigned char)receive_buf[2]);
    */
    
    errchk=(unsigned char)receive_buf[1]+256*(unsigned char)receive_buf[2]; 
    if (PWM_value != errchk){ 
      printf("setPWM: Wrong PWM value returned: Requested:%d Returned:%d\n", PWM_value,errchk); 
      retval=1; 
    } 
    return (retval);        
} //END: set_PWM
/****************************************************************************/ 


int readADC (unsigned char channel, struct usb_dev_handle * USB_handle) 
{
    int RecvLength;
    int SendLength;
    //int Comm_OK=0;
    byte send_buf[64],receive_buf[64];
    int retval=0;
    int errchk;

    send_buf[0]=READ_ADC;              //Set main command.
    
    send_buf[1]= channel;
    
    SendLength=2;                    //5 bytes in the outbound packet.
    RecvLength=4;

    send_usb(USB_handle, SendLength, send_buf);
    recv_usb(USB_handle, RecvLength, receive_buf);
    
    errchk=receive_buf[3];
    if (channel != errchk){
       printf("readADC: Wrong ADC channel returned: Requested:%d Returned:%d\n", channel, errchk);
       //retval=1;
       }
    retval=(unsigned char)receive_buf[1]*256+(unsigned char)receive_buf[2];
    return (retval);          
} //END: readADC
/****************************************************************************/


int writeLCD(char * buffer, byte length, struct usb_dev_handle * USB_handle) 
{
    int RecvLength;
    int SendLength;
    //int Comm_OK=0;
    byte send_buf[64],receive_buf[64];
    int retval=0;
    int errchk;
    int i;
    send_buf[0]=PUT_LCD;              //Set main command.
    
    send_buf[1]= (byte)length;
    
    for (i=0;i<length; ++i){          //Copy print buffer into send buffer
        send_buf[i+2]=(buffer[i]);
        }

    SendLength=2+length;                    // Outbound packet length
    RecvLength=2;                           // [1]token, [2]length

    send_usb(USB_handle, SendLength, send_buf);
    recv_usb(USB_handle, RecvLength, receive_buf);
    
    errchk=receive_buf[1];
    if (length != errchk){
       printf("writeLCD: Wrong string length returned: Requested:%d Returned:%d\n", length, errchk);
       retval=1;
       }
    return (retval);
} //END: writeLCD
/****************************************************************************/ 
 
 
int buzz (unsigned char duration, struct usb_dev_handle * USB_handle) 
{ 
    int RecvLength; 
    int SendLength; 
    //DWORD Comm_OK=0; 
    byte send_buf[64],receive_buf[64]; 
    int retval=0; 
    int errchk; 
 
    send_buf[0]=BUZZER;              //Set main command. 
     
    send_buf[1]= duration; 
     
    SendLength=2;                    //5 bytes in the outbound packet. 
    RecvLength=2; 
 
    send_usb(USB_handle, SendLength, send_buf);
    recv_usb(USB_handle, RecvLength, receive_buf);
 
    errchk=receive_buf[1]; 
    if (duration != errchk){ 
      printf("BUZZER: Wrong duration value returned: Requested:%d Returned:%d\n", duration, errchk); 
      retval=1; 
    } 
    return (retval);           
} //END: buzz
/****************************************************************************/ 

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
} //END: picdem_fs_usb_led
/****************************************************************************/ 


void picdem_fs_usb_reset(struct usb_dev_handle * d)
{
//   byte answer[reqLen];
   byte question[reqLen];
   question[0] = RESET;
   send_usb(d, 1, question);
//   recv_usb(d, 3, answer);
   printf("Board resetted\n");
} //END: picdem_fs_usb_reset
/****************************************************************************/ 

