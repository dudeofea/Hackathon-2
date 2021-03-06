#include "square.h"

static volatile int terminate;
static CUBE_STATE_T _state, *state=&_state;

//init the game
static void init_game(CUBE_STATE_T *state)
{
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };
   
   EGLConfig config;

   // get an EGL display connection
   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, NULL);
   assert(state->context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
   assert( success >= 0 );

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state->screen_width;
   dst_rect.height = state->screen_height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = state->screen_width << 16;
   src_rect.height = state->screen_height << 16;

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = state->screen_width;
   nativewindow.height = state->screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);

   // Set background color and clear buffers
   glClearColor(0.15f, 0.25f, 0.35f, 1.0f);

   // Enable back face culling.
   glEnable(GL_CULL_FACE);
   glMatrixMode(GL_MODELVIEW);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//init projections
static void init_model_proj(CUBE_STATE_T *state)
{
   float nearp = 1.0f;
   float farp = 500.0f;
   float hht;
   float hwd;

   glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

   glViewport(0, 0, (GLsizei)state->screen_width, (GLsizei)state->screen_height);
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   hht = nearp * (float)tan(45.0 / 2.0 / 180.0 * M_PI);
   hwd = hht * (float)state->screen_width / (float)state->screen_height;

   glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);
   
   glEnableClientState( GL_VERTEX_ARRAY );
   glVertexPointer( 3, GL_BYTE, 0, quadx );

   reset_model(state);
}

//reset initial state
static void reset_model(CUBE_STATE_T *state)
{
   // reset model position
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.f, 0.f, -40.f);

   // reset model rotation
   state->rot_angle_x = 45.f; state->rot_angle_y = 30.f; state->rot_angle_z = 0.f;
   state->rot_angle_x_inc = 0.5f; state->rot_angle_y_inc = 0.5f; state->rot_angle_z_inc = 0.f;
   state->distance = 40.f;
}

//must take in all lowercase!!
static void draw_char(const char c, int x, int y, int s){
   int i = c - 97;
   glTranslatef(x, y, 0.0f);
   glVertexPointer( 3, GL_FLOAT, 0, alphabet);
   glDrawArrays(GL_LINE_STRIP, i * 10, 10);
   glTranslatef(-x, -y, 0.0f);
}

static void draw_text(const char *str, int x, int y, int s){
   int i;
   for (i = 0; i < strlen(str); i++){
      draw_char(str[i], x + i*s, y, s);
   }
}

//draw loop
static void redraw_scene(CUBE_STATE_T *state)
{
   // Start with a clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   glEnable(GL_TEXTURE_2D);
   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state->tex[0]);
   // Need to rotate textures - do this by rotating each cube face
   //glRotatef(180.f, 0.f, 0.f, 1.f ); // front face normal along z axis
   // draw first 4 vertices
   glVertexPointer( 3, GL_BYTE, 0, quadx );
   glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);
   glDisable(GL_TEXTURE_2D);

   //draw line
   glColor4f(0.f, 0.f, 0.f, 1.f);
   glLineWidth(5.0f);
   glVertexPointer( 3, GL_FLOAT, 0, linex );
   glEnableClientState(GL_VERTEX_ARRAY);
   glDrawArrays(GL_LINES, 0, 2);
   //reset global tint
   glColor4f(1.f, 1.f, 1.f, 1.f);
   glLineWidth(3.0f);
   //draw_char('c', 0, 0, 1);
   draw_text("hey", 0, 0, 1);
   eglSwapBuffers(state->display, state->surface);
}

/***********************************************************
* Name: init_textures
*
* Arguments:
* CUBE_STATE_T *state - holds OGLES model info
*
* Description: Initialise OGL|ES texture surfaces to use image
* buffers
*
* Returns: void
*
***********************************************************/
static void init_textures(CUBE_STATE_T *state)
{
   // load three texture buffers but use them on six OGL|ES texture surfaces
   //load_tex_images(state);
   state->tex[0] = load_tex_from_BMP("./background.bmp");

   // setup overall texture environment
   glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   
   glEnable(GL_TEXTURE_2D);
}

GLuint load_tex_from_BMP(const char * imagepath){
   // Data read from the header of the BMP file
   unsigned char header[54]; // Each BMP file begins by a 54-bytes header
   unsigned int dataPos;     // Position in the file where the actual data begins
   unsigned int width, height;
   unsigned int imageSize;   // = width*height*3
   // Actual RGB data
   unsigned char * data;

   // Open the file
   FILE * file = fopen(imagepath,"rb");
   if (!file){
      printf("Image could not be opened\n"); return 0;
   }
   if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
      printf("Not a correct BMP file\n");
      return 0;
   }
   if ( header[0]!='B' || header[1]!='M' ){
    printf("Not a correct BMP file\n");
    return 0;
   }
   // Read ints from the byte array
   dataPos    = *(int*)&(header[0x0A]);
   imageSize  = *(int*)&(header[0x22]);
   width      = *(int*)&(header[0x12]);
   height     = *(int*)&(header[0x16]);
   // Some BMP files are misformatted, guess missing information
   if (imageSize==0)
      imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
   if (dataPos==0)
      dataPos=54; // The BMP header is done that way
   // Create a buffer
   data = malloc(imageSize);
    
   // Read the actual data from the file into the buffer
   fread(data,1,imageSize,file);
    
   //Everything is in memory now, the file can be closed
   fclose(file);
   // Create one OpenGL texture
   GLuint textureID;
   glGenTextures(1, &textureID);
    
   // "Bind" the newly created texture : all future texture functions will modify this texture
   glBindTexture(GL_TEXTURE_2D, textureID);
    
   // Give the image to OpenGL
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   return textureID;
}

//------------------------------------------------------------------------------

static void exit_func(void)
// Function to be passed to atexit().
{
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(state->display, state->surface);

   // Release OpenGL resources
   eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( state->display, state->surface );
   eglDestroyContext( state->display, state->context );
   eglTerminate( state->display );

   // release texture buffers
   free(state->tex_buf1);
   free(state->tex_buf2);
   free(state->tex_buf3);

   printf("\ncube closed\n");
} // exit_func()

//==============================================================================

int main ()
{
   bcm_host_init();
   // Clear application state
   memset( state, 0, sizeof( *state ) );
   // Start OGLES
   init_game(state);

   // Setup the model world
   init_model_proj(state);

   // initialise the OGLES texture(s)
   init_textures(state);

   while (!terminate)
   {
      redraw_scene(state);
   }
   exit_func();
   return 0;
}
