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
static void exit_func(void);
