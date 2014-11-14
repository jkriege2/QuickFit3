#include "radhard2flash.h"
#include "qftools.h"
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


unsigned int check_bit_header(FILE * infile, char* message) {
      char kar,str[256];
      char designName[1024], designName2[1024], creationDate[256], creationTime[256];
  const char header[9] = {(char)0x0f, (char)0xf0, (char)0x0f,(char)0xf0,
			  (char)0x0f, (char)0xf0, (char)0x0f, (char)0xf0, 0};
  unsigned int length, loop;

  length = fgetc(infile) * 256 + fgetc(infile);
  if (length != 9)
  {
    sprintf(message, "\nField 1 length error, is this a valid bit-file?\n\n");
    fprintf(stderr, "%s\n", message);
    return 0;
  }
  for (loop = 0; loop < length; loop ++)
  {
    kar = fgetc(infile);
    if (kar != header[loop])
    {
      sprintf(message, "\nThe header data does not match, is this a valid bit-file?\n\n");
      fprintf(stderr, "%s\n", message);
      return 0;
    }
  }
  length = fgetc(infile) * 256 + fgetc(infile);
  if (length != 1)
  {
    sprintf(message, "\nField 2 length error, is this a valid bit-file?\n\n");
    fprintf(stderr, "%s\n", message);
    return 0;
  }
  kar = fgetc(infile);
  if (kar != 'a')
  {
    sprintf(message,
	    "\nThe field 2 data does not match, is this a valid bit-file?\n\n");
    fprintf(stderr, "%s\n", message);
    return 0;
  }
  length = fgetc(infile) * 256 + fgetc(infile);
  for (loop = 0; loop < length; loop ++)
    str[loop] = fgetc(infile);
  printf("Design Name : \"%s\"\n" , str);
  strcpy(designName, str);
  kar = fgetc(infile);
  if (kar != 'b')
  {
    sprintf(message,
	    "\nThe field 4 data does not match, is this a valid bit-file?\n\n");
    return 0;
  }
  length = fgetc(infile) * 256 + fgetc(infile);
  for (loop = 0 ;loop < length; loop ++)
    str[loop] = fgetc(infile);
  printf("Device Name : \"%s\"\n" , str);
  strcpy(designName2, str);
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
  strcpy(creationDate, str);
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
  strcpy(creationTime, str);
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

  sprintf(message, "Design Name:  '%s'\n              '%s'\nCreation Date: %s\nCreation Time: %s\nStream Length: %u = 0x%08X Bytes\n", designName, designName2, creationDate, creationTime, length, length);

  return length;
}





#define TEMPSIZE 1024
int flash_bitfile(char * infilename, char* message, char fpga) {
  char tmpMessage[TEMPSIZE];
  for (int i=0; i<TEMPSIZE; i++) tmpMessage[i]=0;

  // usb related variables
  int                rv;
  int                flag;
  struct usb_bus    *bus;
  struct usb_device *device=NULL;
  usb_dev_handle    *h_device;

  // bitstream related variables
  unsigned int       flen;
  unsigned int       id_product;
  FILE              *infile;
  char              *buffer;
  int                i, j;
  char               byte, inv_byte;

  if ((fpga=='m')||(fpga=='M')) {
     id_product = ID_PRODUCT_MASTER;
     sprintf(message, "programming master FPGA");
  } else {
     id_product = ID_PRODUCT_SLAVE;
     sprintf(message, "programming slave FPGA");
  }

  // board detection
  usb_init();
  usb_find_busses();
  usb_find_devices();

  flag = 0;

  for (bus = usb_busses; bus; bus = bus->next)
  {
    for (device = bus->devices; device; device = device->next)
    {
      //fprintf(stderr, "device: vender=%X  product=%X  (Radhrad2: %X / %X)\n", device->descriptor.idVendor, device->descriptor.idProduct, ID_VENDOR, id_product);
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
    memset(tmpMessage, 0, TEMPSIZE);
    sprintf(tmpMessage, "\n\nNo board detected!");
    strcat(message, tmpMessage);
    return 0;
  }

  // bitstream file handling
  infile = fopen(infilename, "r");
  if (!infile)
  {
    memset(tmpMessage, 0, TEMPSIZE);
    sprintf(tmpMessage, "\n\nError opening file \"%s\" for reading!", infilename);
    strcat(message, tmpMessage);
    return 0;
  }
  printf("\n");
  flen = check_bit_header(infile, tmpMessage);
  strcat(message, tmpMessage);
  if (!flen) {
    memset(tmpMessage, 0, TEMPSIZE);
    sprintf(tmpMessage, "\n\nError checking header of bitfile \"%s\".", infilename);
    strcat(message, tmpMessage);
    return 0;
  }
  buffer = (char*)qfMalloc(flen * sizeof(char));
  if (!buffer)
  {
    memset(tmpMessage, 0, TEMPSIZE);
    sprintf(tmpMessage, "\n\nMemory allocation failed!");
    strcat(message, tmpMessage);
    return 0;
  }
  for (i = 0; i < (long)flen; i++)
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
    memset(tmpMessage, 0, TEMPSIZE);
    sprintf(tmpMessage, "\n\nCould not set the USB configuration 0x%08X" , device->config->bConfigurationValue );
    strcat(message, tmpMessage);
    qfFree(buffer);
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
  qfFree(buffer);
  if (rv < 0)
  {
    memset(tmpMessage, 0, TEMPSIZE);
    sprintf(tmpMessage, "\n\nFailed to send the bitstream!");
    strcat(message, tmpMessage);
    return 0;
  }
  else {
    memset(tmpMessage, 0, TEMPSIZE);
    sprintf(tmpMessage, "\n\nSent: 0x%.8x Bytes", rv);
    strcat(message, tmpMessage);
  }

  return 1;
}
