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

GLfloat triangle[] = {
  -0.25f, -0.25f, 0.0f,
  -0.25f, -0.25f, 0.0f,
  -0.25f, -0.25f, 0.0f,
};

GLfloat square[] = {
  -0.25f, -0.25f,
  -0.25f, -0.25f,
  -0.25f, -0.25f,
  -0.25f, -0.25f,
};

GLfloat colors[] = {
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
};

static void init() 
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

static void display() 
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  //Triangle
  glShadeModel(GL_SMOOTH);
  
  glVertexPointer(3, GL_FLOAT, 0, triangle);
  glColorPointer(4, GL_FLOAT, 0,colors);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  
  glPushMatrix();

  glTranslate(0.25f, 0.5f, 0.0f);
  glTranslate(0.25f, 0.5f, 0.0f);
  glScalef(0.5f, 0.5f, 0.5f);
  
  glDrawArrays(GL_TRIANGLES, 0, 3);
  
  glPopMatrix();

  glDisableClientState(GL_COLOR_ARRAY);

  //Square
  glShadeModel(GL_FLAT);
  glVertexPointer(2, GL_FLOAT, 0, square);
  
  glColor4f(0.25f, 0.25f, 0.75f, 1.0f);

  glPushMatrix();
  
  glTranslatef(0.75f, 0.5f, 0.0f);
  glScalef(0.5f, 0.5f, 0.5f);
  
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glPopMatrix();

  glFlush();
  glutSwapBuffers();
}

int  main (char * args) 
{
  init();
  while(1)
    {
      display();
    }
  return 0;
}
