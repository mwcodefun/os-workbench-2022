#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
int max_v = 10000;

char* itostr(int value,int radix,char *target){
  char *o = target;
  if (value == 0)
  {
    *target = '0';
    *(target + 1) = '\0';
    return o;
  }
  int iq[64];
  int index = 0;

  char *chars = "0123456789abcdef";
  int c = 0;
  int sign = 0;
  if(value < 0){
    sign = 1;
    value = 0 - value;
  }
  while (value != 0)
  {
    int a = value % radix;
    value = value / radix;
    iq[++index] = a;
  }
  if(sign){
   target[c++] = '-';
  }
  while(index > 0){
    target[c++] = chars[iq[index--]];
  }

  target[c] = '\0';
  //reverse 
  return o;
}

void print(const char *value){
  while(*value){
    putch(*value++);
  }
}

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  char out[128];
  vsnprintf(out,128,fmt,ap);
  char *p = out;
  while(*p)
  {
    putch(*p++);
  }
  return strlen(out);
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out,INT32_MAX,fmt,ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  return vsprintf(out,fmt,ap);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  return vsnprintf(out,n,fmt,ap);
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {

  int pre_format = 0;
  size_t count = 0;
  int value = 0;
  char *strValue;
  char tmp_buf[64];
  while(*fmt && n){
    n--;

    if(pre_format){
      pre_format = 0;
      switch (*fmt)
      {
        case 's':
          strValue = va_arg(ap,char *);
          out = strcat(out,strValue);
          out += strlen(strValue);
          break;
        case 'd':
          value = va_arg(ap,int);
          char *dstr = itostr(value,10,tmp_buf);
          out = strcat(out,dstr);
          out += strlen(dstr);
          break;
        case '%':
          *out++ = '%';
          break;
        default:
          break;
      }
      fmt++;
      continue;
    }
    if(*fmt == '%'){
      pre_format = 1;
    }else{
      *out++ = *fmt;
    }

    fmt++;
  }
  *out = '\0';

  return count;
}




#endif
