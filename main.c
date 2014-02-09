/* Main game loop */
#include "square.h"
#include "PS2.h"
#include "BMG.h"

volatile int terminate;
CUBE_STATE_T _state, *state=&_state;
char *start, *end;
char **p1_list, **p2_list;
int p1_size = 30;
int p2_size = 30;
struct ps2_controller p1_cont;
struct ps2_controller p2_cont;
int p1_index = 0;
int p2_index = 0;

//draw loop
void redraw_scene(CUBE_STATE_T *state)
{
	int i;
   draw_back(state);
   draw_midline();
   //reset global tint
   glColor4f(1.f, 1.f, 1.f, 1.f);
   glLineWidth(1.0f);
   draw_text_center(end, 1.0, 10, 1.2f);
   glLineWidth(0.2f);
   for (i = 0; i < p1_size; ++i)
   {
   		draw_text(p1_list[i], -17, 5.0f - i * 0.5f, 0.5f);
   }
   for (i = 0; i < p1_size; ++i)
   {
   		draw_text(p1_list[i], 5.0, 5.0f - i * 0.5f, 0.5f);
   }
   glColor4f(1.f, 0.8f, 0.8f, 1.f);
   glLineWidth(2.0f);
   draw_text("oooo", -2, 5.0f - p1_index * 0.5f, 0.5f);
   glColor4f(0.8f, 1.f, 0.8f, 1.f);
   draw_text("oooo", 0, 5.0f - p2_index * 0.5f, 0.5f);
   glColor4f(1.f, 1.f, 1.f, 1.f);
   eglSwapBuffers(state->display, state->surface);
}

char *get_random_link(){
	static char* str;
	FILE *fpipe = (FILE*)popen("python wiki.py -r 1","r");
	static char line[256];
	fgets( line, sizeof(line), fpipe);
	str = (char*)&line;
	pclose(fpipe);
	return str;
}

void get_all_links(char** list, char* link, int size){
	char cmd[50];
	int i = 0;
	sprintf(cmd, "python wiki.py -l \"%s\" -m %d", link, size);
	printf("%s\n", cmd);
	FILE *fpipe = (FILE*)popen(cmd,"r");
	static char line[256];
	for (i = 0; i < size; ++i)
	{
		fgets( line, sizeof(line), fpipe);
		strncpy(list[i], &line, 24);
	}
	pclose(fpipe);
}

int main ()
{
	int i;
	start = get_random_link();
	end = get_random_link();
	p1_list = (char**)malloc(p1_size*sizeof(char*));
	p2_list = (char**)malloc(p1_size*sizeof(char*));
	for (i = 0; i < p1_size; ++i)
	{
		p1_list[i] = (char*)malloc(25*sizeof(char));
		p2_list[i] = (char*)malloc(25*sizeof(char));
	}
	get_all_links(p1_list, start, p1_size);
	for (i = 0; i < p1_size; ++i)
	{
		strcpy(p2_list[i], p1_list[i]);
	}
	int j;
	for (j= 0; j < p1_size; ++j)
	{
		printf("%d: %s\n", j, p1_list[j]);
		printf("%d: %s\n", j, p2_list[j]);
	}
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
	//BMG_player();	
	//system("mpg321 --aggressive -z -B music/  &");
	
	while (!terminate)
	{
		get_ps2_input(handle, &p1_cont, &p2_cont);
		if (p1_cont.rstick_y > 0.5f)
		{
			p1_index++;
			if (p1_index >= p1_size)
			{
				p1_index = 0;
			}
		}
		if (p1_cont.rstick_y < -0.5f)
		{
			p1_index--;
			if (p1_index < 0)
			{
				p1_index = p1_size - 1;
			}
		}
		if (p2_cont.rstick_y > 0.5f)
		{
			p2_index++;
			if (p2_index >= p2_size)
			{
				p2_index = 0;
			}
		}
		if (p2_cont.rstick_y < -0.5f)
		{
			p2_index--;
			if (p2_index < 0)
			{
				p2_index = p2_size - 1;
			}
		}
		if(p1_cont.key == CROSS_BUTTON){
			char buf[25];
			strncpy(buf, p1_list[p1_index], 24);
			p1_list = (char**)malloc(p1_size*sizeof(char*));
			for (i = 0; i < p1_size; ++i)
			{
				p1_list[i] = (char*)malloc(24*sizeof(char));
			}
			get_all_links(p1_list, &buf[0], p1_size);
		}
		if(p2_cont.key == CROSS_BUTTON){
			char buf[25];
			strncpy(buf, p2_list[p2_index], 24);
			p2_list = (char**)malloc(p2_size*sizeof(char*));
			for (i = 0; i < p2_size; ++i)
			{
				p2_list[i] = (char*)malloc(24*sizeof(char));
			}
			get_all_links(p2_list, &buf[0], p2_size);
		}
		redraw_scene(state);
	}
	//deinit
	exit_func(state);
	libusb_free_device_list(devices, 1);
	libusb_exit(context);
	return 0;
}
