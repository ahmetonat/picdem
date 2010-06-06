

#ifndef BYTE_DEF
#define BYTE_DEF
typedef char byte;
#endif

/*** Declarations of USB comm functions ***/
void bad(const char *why);
// void send_usb(struct usb_dev_handle *, int, const char *);
void send_usb(struct usb_dev_handle *, int, byte *);
void recv_usb(struct usb_dev_handle * d, int len, byte * dest);

//Bazi denemeler:
//static 
//void send_usb(struct usb_dev_handle * d, int len, const char * src);
/*void send_usb(struct usb_dev_handle * d, int len, byte * src);
//static 
void recv_usb(struct usb_dev_handle * d, int len, byte * dest);
void bad(const char *why);
*/
