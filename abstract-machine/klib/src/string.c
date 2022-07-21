#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *str) {
  const char *s;
	for (s = str; *s; ++s)
		;
	return (s - str);
}

char *strcpy(char *dst, const char *src) {
  return memcpy(dst,src,strlen(src) + 1);
}

char *strncpy(char *dst, const char *src, size_t n) {
  return memcpy(dst,src,n);
}

char *strcat(char *dst, const char *src) {
  strcpy(dst + strlen(dst),src);
  return dst;
}

int strcmp(const char *s1, const char *s2) {

  char c1;
  char c2;
  do
  {
    c1 = *s1++;
    c2 = *s2++;
    if(c1 == '\0'){
      return c1 - c2;
    }
  } while (c1 == c2);
  return c1 - c2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  char c1;
  char c2;
  do
  {
    c1 = *s1++;
    c2 = *s2++;
    if(c1 == '\0'){
      return c1 - c2;
    }
  } while (c1 == c2 && n--);
  return c1 - c2;
}

void *memset(void *s, int c, size_t n) {
  char *cp = (char *)s;
  while (n--)
  {
    *cp++ = c;
  }
  return s;
}

void *memmove(void *dest, const void *src, size_t len) {
  char *d = dest;
  const char *s = src;
  if (d < s)
    while (len--)
      *d++ = *s++;
  else
    {
      const char *lasts = s + (len-1);
      char *lastd = d + (len-1);
      while (len--)
        *lastd-- = *lasts--;
    }
  return dest;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *d = out;
  const char *src = in;
  while (n--)
  {
    *d++ = *src++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  register const char *c1 = s1;
  register const char *c2 = s2;

  while (n--)
  {
    if(*c1++ != *c2++){
      return c1[-1] > c2[-1] ? 1 : -1;
    }
  }
  return 0;
}

#endif
