/* Main game loop */
#include "square.h"
#include "PS2.h"
#include "BMG.h"

volatile int terminate;
CUBE_STATE_T _state, *state=&_state;

//draw loop
void redraw_scene(CUBE_STATE_T *state)
{
   draw_back(state);
   draw_midline();
   //reset global tint
   glColor4f(1.f, 1.f, 1.f, 1.f);
   glLineWidth(1.0f);
   //draw_text("abcdefghijklmnopqrstuvwxyz123", 0, 0, 2);
   eglSwapBuffers(state->display, state->surface);
}

char *get_random_link(){
	static char* str;
	FILE *fpipe = (FILE*)popen("python wiki.py -r 1","r");
	static char line[256];
	fgets( line, sizeof(line), fpipe))
	printf("%s", line);
	str = &line;
	pclose(fpipe);
	return str;
}

int main ()
{
	char *start, *end;
	start = get_random_link();
	end = get_random_link();
	struct libusb_device **devices;
	struct libusb_context *context;
	libusb_init(&context);
	struct libusb_device *ps2 = find_ps2_hub(context, devices);
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
	bcm_host_init();
	// Clear application state
	memset( state, 0, sizeof( *state ) );
	// Start OGLES
	init_game(state);
	// Setup the model world
	init_model_proj(state);
	// initialise the OGLES texture(s)
	init_textures(state);
	BMG_player();	
	
	while (!terminate)
	{
		//get_ps2_input(handle);
		redraw_scene(state);
	}
	//deinit
	exit_func(state);
	libusb_free_device_list(devices, 1);
	libusb_exit(context);
	return 0;
}
