#include "radhard2flash.h"

//-----------------------------------------------------------------------------
//
//  Defines
//
//-----------------------------------------------------------------------------
#define MAX_BIT_INDEX     7
#define MAX_BULK_SIZE     512
#define EP_PROG_FPGA      2
#define ID_VENDOR         0x0547
#define ID_PRODUCT_MASTER 0x1002
#define ID_PRODUCT_SLAVE  0x1003


unsigned int check_bit_header(FILE * infile) {
      char kar,str[256];
  const char header[9] = {(char)0x0f, (char)0xf0, (char)0x0f,(char)0xf0,
			  (char)0x0f, (char)0xf0, (char)0x0f, (char)0xf0, 0};
  unsigned int length, loop;

  length = fgetc(infile) * 256 + fgetc(infile);
  if (length != 9)
  {
    fprintf(stderr, "\nField 1 length error, is this a valid bit-file?\n\n");
    return 0;
  }
  for (loop = 0; loop < length; loop ++)
  {
    kar = fgetc(infile);
    if (kar != header[loop])
    {
     fprintf(stderr,
	     "\nThe header data does not match, is this a valid bit-file?\n\n");
      return 0;
    }
  }
  length = fgetc(infile) * 256 + fgetc(infile);
  if (length != 1)
  {
    fprintf(stderr, "\nField 2 length error, is this a valid bit-file?\n\n");
    return 0;
  }
  kar = fgetc(infile);
  if (kar != 'a')
  {
    fprintf(stderr,
	    "\nThe field 2 data does not match, is this a valid bit-file?\n\n");
    return 0;
  }
  length = fgetc(infile) * 256 + fgetc(infile);
  for (loop = 0; loop < length; loop ++)
    str[loop] = fgetc(infile);
  printf("Design Name : \"%s\"\n" , str);
  kar = fgetc(infile);
  if (kar != 'b')
  {
    fprintf(stderr,
	    "\nThe field 4 data does not match, is this a valid bit-file?\n\n");
    return 0;
  }
  length = fgetc(infile) * 256 + fgetc(infile);
  for (loop = 0 ;loop < length; loop ++)
    str[loop] = fgetc(infile);
  printf("Device Name : \"%s\"\n" , str);
  kar = fgetc(infile);
  if (kar != 'c')
  {
    fprintf(stderr,
	    "\nThe field 4 data does not match, is this a valid bit-file?\n\n");
    return 0;
  }
  length = fgetc(infile) * 256 + fgetc(infile);
  for (loop = 0; loop < length; loop ++)
    str[loop] = fgetc(infile);
  printf("Creation Date : \"%s\"\n" , str);
  kar = fgetc(infile);
  if (kar != 'd')
  {
    fprintf(stderr,
	    "\nThe field 5 data does not match, is this a valid bit-file?\n\n");
    return 0;
  }
  length = fgetc( infile ) * 256 + fgetc(infile);
  for (loop = 0; loop < length; loop ++)
    str[loop] = fgetc(infile);
  printf("Creation Time : \"%s\"\n" , str);
  kar = fgetc(infile);
  if (kar != 'e')
  {
    fprintf(stderr,
	    "\nThe field 5 data does not match, is this a valid bit-file?\n\n");
    return 0;
  }
  length = (fgetc(infile) <<24) + (fgetc(infile)<<16) +
	   (fgetc(infile) <<8)  +  fgetc(infile);
  printf("Stream length = 0x%08X Bytes\n", length);
  return length;
}






int flash_bitfile(char * infilename, char* message, char fpga) {
  // usb related variables
  int                rv;
  int                flag;
  struct usb_bus    *bus;
  struct usb_device *device;
  usb_dev_handle    *h_device;

  // bitstream related variables
  unsigned int       flen;
  unsigned int       id_product;
  FILE              *infile;
  char              *buffer;
  int                i, j;
  char               byte, inv_byte;

  if ((fpga=='m')||(fpga='M'))
     id_product = ID_PRODUCT_MASTER;
  else
     id_product = ID_PRODUCT_SLAVE;

  // board detection
  usb_init();
  usb_find_busses();
  usb_find_devices();

  flag = 0;

  for (bus = usb_busses; bus; bus = bus->next)
  {
    for (device = bus->devices; device; device = device->next)
    {
      if ((device->descriptor.idVendor  == ID_VENDOR) &&
	       (device->descriptor.idProduct == id_product))
      {
	flag = 1;
	break;
      }
    }
    if (flag)
      break;
  }
  if (!device)
  {
    sprintf(message, "No board detected!");
    return 0;
  }

  // bitstream file handling
  infile = fopen(infilename, "r");
  if (!infile)
  {
    sprintf(message, "Error opening file \"%s\" for reading!", infilename);
    return 0;
  }
  printf("\n");
  flen = check_bit_header(infile);
  if (!flen) {
    sprintf(message, "Error checking header of bitfile \"%s\".", infilename);
    return 0;
  }
  buffer = (char*)malloc(flen * sizeof(char));
  if (!buffer)
  {
    sprintf(message, "Memory allocation failed!");
    return 0;
  }
  for (i = 0; i < flen; i++)
  {
    byte     = fgetc(infile);
    inv_byte = 0x00;
    for (j = 0; j <= MAX_BIT_INDEX; j++)
    {
      if (byte & (1 << j))
	inv_byte |= (1 << (MAX_BIT_INDEX - j));
    }
    buffer[i] = inv_byte;
  }
  fclose(infile);

  // usb transfer
  h_device = usb_open(device);
  if (usb_set_configuration(h_device,
			    device->config->bConfigurationValue))
  {
    sprintf(message, "Could not set the USB configuration 0x%08X" , device->config->bConfigurationValue );
    free(buffer);
    usb_close(h_device);
    return 0;
  }
  //fprintf(stderr, "0x%08X\n" , device->config->bConfigurationValue );
  while (usb_claim_interface(h_device,
			     device->config->interface->
			     altsetting->bInterfaceNumber));
  rv = usb_bulk_write(h_device, EP_PROG_FPGA, buffer, flen, 2048);
  usb_release_interface(h_device,
			device->config->interface->
			altsetting->bInterfaceNumber);
  usb_close(h_device);
  free(buffer);
  if (rv < 0)
  {
    sprintf(message, "Failed to send the bitstream!");
    return 0;
  }
  else
    sprintf(message, "Sent: 0x%.8x Bytes", rv);

  return 1;
}
