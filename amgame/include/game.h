#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>


uint32_t snake_length = 1;

//记录🐍身体的位置 X，Y坐标
uint32_t position[63355][2];

//当前运动的方向
uint32_t oritation;


void splash();
void print_key();
static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}
