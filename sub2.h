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

#define	BUZZER		0x4B //AO
#define	SET_PWM		0x41 
#define	INIT_ADC	0x42
#define	READ_ADC	0x43
#define	INIT_DIO	0x44
#define	SET_DO_BIT	0x45
#define	SET_DOport	0x46
#define	READ_DI_BIT	0x47
#define	READ_DI		0x48
#define	INIT_LCD	0x49
#define	PUT_LCD		0x4A
#define	BUZZER		0x4B
#define	POKE		0x4C
#define	PEEK		0x4D //AO

/* PICDEM FS USB max packet size is 64-bytes */
const static int reqLen=64;


#ifndef BYTE_DEF
#define BYTE_DEF
typedef char byte;
#endif


/*** declarations for SUBII functions ***/
void picdem_fs_usb_read_version(struct usb_dev_handle * d);
int setPWM (unsigned char, int, struct usb_dev_handle *);
int readADC (unsigned char, struct usb_dev_handle *); 
int writeLCD(char * buffer, byte length, struct usb_dev_handle *); 
int buzz (unsigned char duration, struct usb_dev_handle *);
void picdem_fs_usb_led(struct usb_dev_handle * d, int lednum, int onoff);
void picdem_fs_usb_reset(struct usb_dev_handle * d);

struct usb_dev_handle *usb_picdem_fs_usb_open(void);



//Not sure if the rest should go here or to usblin.h

/* change by Xiaofan */
/* libusb-win32 requires the correct endpoint address
   including the direction bits. This is the most important
   differece between libusb-win32 and libusb.
*/  
//const static int endpoint=1; /* first endpoint for everything */
const static int endpoint_in=0x81; /* endpoint 0x81 address for IN */
const static int endpoint_out=1; /* endpoint 1 address for OUT */


/* PICkit USB values */ /* From Microchip firmware */
const static int vendorID=0x04d8; // Microchip, Inc
const static int productID=0x000c; // PICDEM FS USB demo app
const static int configuration=1; /*  Configuration 1*/
const static int interface=0;	/* Interface 0 */

const static int timeout=2000; /* timeout in ms */
