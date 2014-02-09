#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>
#include <unistd.h>

#define PS2_VENDOR_ID		0x0810
#define PS2_PRODUCT_ID		0x0001

#define PS2_ENDPOINT		0x81

#define NEUTRAL				0x0f0000
#define CROSS_BUTTON		0x400000 | NEUTRAL
#define SQUARE_BUTTON		0x800000 | NEUTRAL
#define TRIANGLE_BUTTON		0x100000 | NEUTRAL
#define CIRCLE_BUTTON		0x200000 | NEUTRAL

#define bool char
#define false				0
#define true				1

struct ps2_controller
{
	int key;
	float lstick_x;
	float lstick_y;
	float rstick_x;
	float rstick_y;
};

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

bool is_not_neutral(struct ps2_controller *cont){
	if(cont->key != 0)
		return true;
	if(cont->lstick_x != 0.0)
		return true;
	if(cont->lstick_y != 0.0)
		return true;
	if(cont->rstick_x != 0.0)
		return true;
	if(cont->rstick_y != 0.0)
		return true;
	return false;
}

int get_ps2_input(libusb_device_handle* handle){
	unsigned char data[8];
	int trans = 0;
	struct ps2_controller p1;
	struct ps2_controller p2;
	//loop until data is caught
	while(is_not_neutral(&p1) || is_not_neutral(&p2)){
		int err = libusb_interrupt_transfer(handle,
			PS2_ENDPOINT,		//endpoint
			data,				//data buffer
			8,					//length
			&trans,				//bytes transfered
			500);				//timeout
		if(err < 0 && err != LIBUSB_ERROR_TIMEOUT){
			return -1;
		}
		if(trans == -1)
			return -1;
		for (int i = 0; i < trans; ++i)
		{
			printf("%02x ", data[i]);
		}
		printf("\n");
		if(data[0] == 1){
			p1.key = data[5] << 16 | data[6] << 8 | data[7];
			p1.lstick_x = (data[2] - 128) / 128.0;
			p1.lstick_y = (data[1] - 128) / 128.0;
		}else if(data[0] == 2){
			p2.key = data[5] << 16 | data[6] << 8 | data[7];
			p2.lstick_x = (data[2] - 128) / 128.0;
			p2.lstick_y = (data[1] - 128) / 128.0;
		}
	}
	if (p1.key != NEUTRAL)
	{
		printf("Controller 1: 0x%x ", p1.key);
		switch(p1.key){
			case CROSS_BUTTON:		printf("CROSS"); break;
			case SQUARE_BUTTON:		printf("SQUARE"); break;
			case TRIANGLE_BUTTON:	printf("TRIANGLE"); break;
			case CIRCLE_BUTTON:		printf("CIRCLE"); break;
		}
		printf("\n");
	}
	if (p2.key != NEUTRAL)
	{
		printf("Controller 2: 0x%x ", p2.key);
		switch(p2.key){
			case CROSS_BUTTON:		printf("CROSS"); break;
			case SQUARE_BUTTON:		printf("SQUARE"); break;
			case TRIANGLE_BUTTON:	printf("TRIANGLE"); break;
			case CIRCLE_BUTTON:		printf("CIRCLE"); break;
		}
		printf("\n");
	}
	return 0;
}

int main(int argc, const char* argv[])
{
	struct libusb_device **devices;
	struct libusb_context *context;
	libusb_init(&context);
	struct libusb_device *ps2 = find_ps2_hub(context, devices);
	if (ps2)
	{
		//open usb device
		libusb_device_handle *handle;
		int err = 0;
		err = libusb_open(ps2, &handle);
		if(err)
			printf("Unable to open device, %s\n", libusb_error_name(err));
		//detach from kernel
		if (libusb_kernel_driver_active(handle, 0)){
			printf("Device busy...detaching...\n"); 
			libusb_detach_kernel_driver(handle,0);
		}
		else printf("Device free from kernel\n");
		//set interface
		err = libusb_claim_interface(handle, 0);
		if(err)
			printf("Unable to claim interface, %s\n", libusb_error_name(err));
		while(1)
			get_ps2_input(handle);
	}
	libusb_free_device_list(devices, 1);
	libusb_exit(context);
	return 0;
}