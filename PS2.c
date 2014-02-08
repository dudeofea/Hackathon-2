#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>
#include <unistd.h>

#define PS2_VENDOR_ID	0x0810
#define PS2_PRODUCT_ID	0x0001

#define PS2_ENDPOINT	0x81

//find the first quckcam express camera attached to usb
struct libusb_device *find_ps2_hub(libusb_context *context, libusb_device **devices){
	
	//Get List of USB devices
	ssize_t cnt = libusb_get_device_list(context, &devices);
	if (cnt < 1)
	{
		printf("No USB Devices Found\n");
	}else{
		printf("Found %d USB devices\n", (int)cnt);
	}
	//Find PS2 Controller Hub
	for
	 (int i = 0; i < cnt; ++i)
	{
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(devices[i], &desc);
		if (desc.idVendor == PS2_VENDOR_ID && desc.idProduct == PS2_PRODUCT_ID)
		{
			printf("Found A PS2 Controller Hub (%d)\n", i);
			return devices[i];
		}
	}
	return NULL;
}

void get_ps2_input(libusb_device_handle* handle){
	unsigned char data[8];
	int trans = 0;
	int err = libusb_interrupt_transfer(handle,
		PS2_ENDPOINT,		//endpoint
		data,				//data buffer
		8,					//length
		&trans,				//bytes transfered
		500);				//timeout
	if(err){
		printf("Transfer Error, %s\n", libusb_error_name(err));
	}else{
		printf("%d Bytes Transfered\n", trans);
		printf("Data: ");
		for (int i = 0; i < 8; ++i)
		{
			printf("%02x", data[i]);
		}
		printf("\n");
	}
}

int main(int argc, const char* argv[])
{
	struct libusb_device **devices;
	struct libusb_context *context;
	libusb_init(&context);
	struct libusb_device *ps2 = find_ps2_hub(context, devices);
	if (ps2)
	{
		libusb_device_handle *handle;
		int err = 0;
		err = libusb_open(ps2, &handle);
		if (err)
			printf("Unable to open device, %s\n", libusb_error_name(err));
		get_ps2_input(handle);
	}
	libusb_free_device_list(devices, 1);
	libusb_exit(context);
	return 0;
}