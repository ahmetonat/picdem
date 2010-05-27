
#include <usb.h> /* libusb header */
#include <unistd.h> /* for geteuid */
#include <stdio.h>

//typedef unsigned char byte;
typedef char byte;

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

extern void picdem_fs_usb_read_version(struct usb_dev_handle * d);

//static 
//void send_usb(struct usb_dev_handle * d, int len, const char * src);
void send_usb(struct usb_dev_handle * d, int len, byte * src);
//static 
void recv_usb(struct usb_dev_handle * d, int len, byte * dest);
void bad(const char *why);

/****************** Internal I/O Commands *****************/

/** Send this binary string command. */
//static 
//void send_usb(struct usb_dev_handle * d, int len, const char * src)
void send_usb(struct usb_dev_handle * d, int len, byte * src)
{
   int r = usb_bulk_write(d, endpoint_out, (char *)src, len, timeout);
   //DIKKAT: bunun usb_interrupt olmasi gerekebilir. Degisik MCHPFSUSB surumleri icin degismis galiba.
//   if( r != reqLen )
   if( r < 0 )
   {
	  perror("usb PICDEM FS USB write"); 
	  bad("USB write failed"); 
   }
}

/** Read this many bytes from this device */
//static 
void recv_usb(struct usb_dev_handle * d, int len, byte * dest)
{
//   int i;
   int r = usb_bulk_read(d, endpoint_in, dest, len, timeout);
   if( r != len )
   {
	  perror("usb PICDEM FS USB read"); 
	  bad("USB read failed"); 
   }
}


void bad(const char *why) {
	fprintf(stderr,"Fatal error> %s\n",why);
	exit(17);
}



/* debugging: enable debugging error messages in libusb */
extern int usb_debug;

/* Find the first USB device with this vendor and product.
   Exits on errors, like if the device couldn't be found.
*/
struct usb_dev_handle *usb_picdem_fs_usb_open(void)
{
  struct usb_device * device;
  struct usb_bus * bus;

#ifndef WIN32  
  if( geteuid()!=0 ){
    bad("This program must be run as root, or made setuid root");
  }
#endif  
#ifdef USB_DEBUG
  usb_debug=4; 
#endif

  //printf("Searching for SUBOARDII (vendor ID=0x%04x/product ID=0x%04x)\n", vendorID, productID);
  printf("Searching for SUBOARDII...\n");
  /* (libusb setup code stolen from John Fremlin's cool "usb-robot") */

  // added the two debug lines  
  usb_set_debug(0); //WAS: 255 AO
  //printf("setting USB debug on by adding usb_set_debug(255) \n");
  //End of added codes

  usb_init();
  usb_find_busses();
  usb_find_devices();

/* change by Xiaofan */  
/* libusb-win32: not using global variable like usb_busses*/
/*  for (bus=usb_busses;bus!=NULL;bus=bus->next) */  
  for (bus=usb_get_busses();bus!=NULL;bus=bus->next) 
  {
	 struct usb_device * usb_devices = bus->devices;
	 for( device=usb_devices; device!=NULL; device=device->next )
	 {
		if( device->descriptor.idVendor == vendorID &&
			device->descriptor.idProduct == productID )
		{
		   struct usb_dev_handle *d;
		   printf("Found SUBOARDII as device %s on USB bus %s\n",
				   device->filename,
				   device->bus->dirname);
		   d = usb_open(device);
		   if( d )
		   { /* This is our device-- claim it */
//			  byte retData[reqLen];
			  if( usb_set_configuration(d, configuration) ) 
			  {
				 bad("Error setting USB configuration.\n");
			  }
			  if( usb_claim_interface(d, interface) ) 
			  {
				 bad("Claim failed-- the USB PICDEM FS USB is in use by another driver.\n");
			  }
			  printf("Communication established.\n");
			  picdem_fs_usb_read_version(d);
			  printf("\n");
			  return d;
		   }
		   else 
			  bad("Open failed for USB device");
		}
		/* else some other vendor's device-- keep looking... */
	 }
  }
  bad("Could not find USB PICDEM FS USB Demo Board--\n"
      "you might try lsusb to see if it's actually there.");
  return NULL;
}

/* Change by Xiaofan */
/* Please also refer to README */
