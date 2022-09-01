#include "co.h"
#include <stdlib.h>

struct co {
};

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  return NULL;
}

void co_wait(struct co *co) {
}

void co_yield() {
}

int main(){
  return 0;
}