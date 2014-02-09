#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define PATH "./"

#define IMAGE_SIZE 128

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

#ifndef M_PI
#define M_PI 3.141592654
#endif

typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;

static const GLbyte quadx[6*1*3] = {
   /* FRONT */
   -18, -12, 10,
   18, -12, 10,
   -18, 12, 10,
   18, 12, 10,
};

GLfloat linex[6] = {
   0.5,-12,10,
   0.5,12,10,
};

GLfloat alphabet[3*10*26]={
   //A
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //B
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
   //C
   0,0,10,
   0.8f,0,10,
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   //D
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //E
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
   //F
   0,0,10,
   0.8f,0,10,
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   //G
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //H
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
   //I
   0,0,10,
   0.8f,0,10,
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   //J
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //K
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
   //L
   0,0,10,
   0.8f,0,10,
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   //M
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //N
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
   //O
   0,0,10,
   0.8f,0,10,
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   //P
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //Q
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
   //R
   0,0,10,
   0.8f,0,10,
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   //S
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //T
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
   //U
   0,0,10,
   0.8f,0,10,
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   //V
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //W
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
   //X
   0,0,10,
   0.8f,0,10,
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   0.8f,1,10,
   //Y
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   0.8f,0,10,
   //Z
   0,0,10,
   0,1,10,
   0.8f,1,10,
   0.8f,0.5f,10,
   0,0.5f,10,
   0.8f,0.5f,10,
   0.8f,0,10,
   0,0,10,
   0,0,10,
   0,0,10,
};

/** Texture coordinates for the quad. */
static const GLfloat texCoords[6 * 1 * 2] = {
   0.f, 0.f,
   1.f, 0.f,
   0.f, 1.f,
   1.f, 1.f,
};

typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
// OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   GLuint tex[2];
// model rotation vector and direction
   GLfloat rot_angle_x_inc;
   GLfloat rot_angle_y_inc;
   GLfloat rot_angle_z_inc;
// current model rotation angles
   GLfloat rot_angle_x;
   GLfloat rot_angle_y;
   GLfloat rot_angle_z;
// current distance from camera
   GLfloat distance;
   GLfloat distance_inc;
// pointers to texture buffers
   char *tex_buf1;
   char *tex_buf2;
   char *tex_buf3;
} CUBE_STATE_T;

static void init_game(CUBE_STATE_T *state);
static void init_model_proj(CUBE_STATE_T *state);
static void reset_model(CUBE_STATE_T *state);
static void redraw_scene(CUBE_STATE_T *state);
static void init_textures(CUBE_STATE_T *state);
static GLuint load_tex_from_BMP(const char * imagepath);
static void draw_text(const char *str, int x, int y, int s);
static void exit_func(void);
