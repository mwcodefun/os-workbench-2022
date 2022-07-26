#include <game.h>
#include <klib.h>

#define SIDE 16
static int w, h;

uint32_t background_color = 0x000000;
uint32_t color = 0xffffff;

static void init() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  printf("AM_GPU_CONFIG,w=%d,h=%d\n",info.width,info.height);
  w = info.width;
  h = info.height;

  snake_length = 1;
  position[0][0] = w/2;
  position[0][1] = h / 2;

}




static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void draw_main(){
  for(int i = 0;i < snake_length; i++){
    draw_tile(position[i][0],position[i][1],SIDE,SIDE,color);
  }
}

void splash() {
  init();
  draw_main();
  // for (int x = 0; x * SIDE <= w; x ++) {
  //   for (int y = 0; y * SIDE <= h; y++) {
  //     if ((x & 1) ^ (y & 1)) {
  //       draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
  //     }
  //   }
  // }
}
