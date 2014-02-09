
// Test app for VG font library.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "bcm_host.h"
#include "vgfont.h"

int32_t render_text(GRAPHICS_RESOURCE_HANDLE img, const char *text, const uint32_t text_size, const uint32_t y_offset)
{
  uint32_t text_length = strlen(text);
  uint32_t width = 0, height = 0;
  uint32_t img_w, img_h;
  int32_t s = 0;
  
  graphics_get_resource_size(img, &img_w, &img_h);

  if (text_length == 0) return 0;

  s = graphics_resource_text_dimensions_ext(img, text, text_length, &width, &height, text_size);
  
  if (s != 0) return s;
}

int main(void)
{
   GRAPHICS_RESOURCE_HANDLE img;
   uint32_t width, height;
   int LAYER=1;
   bcm_host_init();
   int s;

   s = gx_graphics_init(".");
   assert(s == 0);

   s = graphics_get_display_size(0, &width, &height);
   assert(s == 0);

   s = gx_create_window(0, width, height, GRAPHICS_RESOURCE_RGBA32, &img);
   assert(s == 0);

   // transparent before display to avoid screen flash
   graphics_resource_fill(img, 0, 0, width, height, GRAPHICS_RGBA32(0,0,0,0x00));

   graphics_display_resource(img, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 1);

   uint32_t text_size = 10;
   while (1) {
      const char *text = "The quick brown fox jumps over the lazy dog";
      uint32_t y_offset = height-60+text_size/2;
      graphics_resource_fill(img, 0, 0, width, height, GRAPHICS_RGBA32(0,0,0,0x00));
      // blue, at the top (y=40)
      graphics_resource_fill(img, 0, 40, width, 1, GRAPHICS_RGBA32(0,0,0xff,0xff));

      // green, at the bottom (y=height-40)
      graphics_resource_fill(img, 0, height-40, width, 1, GRAPHICS_RGBA32(0,0xff,0,0xff));

      // draw the subtitle text
      graphics_update_displayed_resource(img, 0, 0, 0, 0);
      text_size += 1;
      if (text_size > 50)
         text_size = 10;
   }

   graphics_display_resource(img, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 0);
   graphics_delete_resource(img);

   return 0;
}
