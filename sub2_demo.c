#include <usb.h> /* libusb header */
#include <stdio.h>
#include <string.h>
#include "sub2.h"


int main(int argc, char ** argv) 
{
  int PWMval;
  int i;
  char sendLCD[60];//Make sure this is long enough to keep all characters!!! 

  struct usb_dev_handle * picdem_fs_usb = usb_picdem_fs_usb_open();
  
  PWMval=500;
  printf ("Set PWM=%d for motor1.\n",PWMval); 
  setPWM (1,PWMval,picdem_fs_usb); 
  printf ("Set PWM=%d for motor2.\n",PWMval); 
  setPWM (2,PWMval,picdem_fs_usb);
  
  
  buzz(15,picdem_fs_usb);
  picdem_fs_usb_led(picdem_fs_usb, 3, 1);
  
  i=readADC(0,picdem_fs_usb);
  printf("ADC CH0=%d\n",i);
  i=readADC(1,picdem_fs_usb);
  printf("ADC CH1=%d\n",i);
  i=readADC(2,picdem_fs_usb);
  printf("ADC CH2=%d\n",i);
  i=readADC(7,picdem_fs_usb);
  printf("ADC CH7=%d\n",i);
  i=(unsigned int)readADC(4,picdem_fs_usb);
  printf("ADC CH4=%d\n",i);

  printf("Sending string to LCD\n");
  i=0;
  sendLCD[0]=0x01; ++i;     
  //sendLCD[1]=0x0C; ++i;   //This appears as a character due to 
  // insufficiency of LCD print queue driver on SUBII (AO.'s fault)
  strcpy(sendLCD+i,"Sabanci Univ."); i=i+13;
  sendLCD[i]=0xC0;  ++i;
  strcpy(sendLCD+i,"SUBOARD 2.1"); i=i+11;
    writeLCD(sendLCD,i,picdem_fs_usb);  
  
  usb_close(picdem_fs_usb);
  return 0;
}
