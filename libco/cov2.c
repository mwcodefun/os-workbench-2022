#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define STACK_SIZE (1 << 16)

enum co_status
{
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};


struct co
{
  int pool_idx;
  char *name;
  void (*func)(void *); // co_start 指定的入口地址和参数
  void *arg;

  enum co_status status;     // 协程的状态
  struct co *waiter;         // 是否有其他协程在等待当前协程
  jmp_buf context;           // 寄存器现场 (setjmp.h)
  uint8_t stack[STACK_SIZE]; // 协程的堆栈
 };
#define CO_POOL_SIZE  100
struct co *co_pool[CO_POOL_SIZE];

void co_pool_add(struct co *co){
    for(int i = 0;i < CO_POOL_SIZE; i++){
        if(co_pool[i] == NULL){
            co_pool[i] = co;
            co -> pool_idx = i;
            return;
        }
    }
}


static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg)
{
  asm volatile(
#if __x86_64__
      "movq %0, %%rsp; movq %2, %%rdi; jmp *%1"
      :
      : "b"((uintptr_t)sp), "d"(entry), "a"(arg)
      : "memory"
#else
      "movl %0, %%esp; movl %2, 4(%0); jmp *%1"
      :
      : "b"((uintptr_t)sp - 8), "d"(entry), "a"(arg)
      : "memory"
#endif
  );
}


struct co *co_start(const char *name, void (*func)(void *), void *arg)
{
  struct co *co = (struct co *)malloc(sizeof(struct co));
  co->name = (char *)malloc(sizeof(name));
	strcpy(co->name, name);
  
  co->func = func;
  co->arg = arg;
  
  co->status = CO_NEW;
  co -> waiter = NULL;
  co_pool_add(co);
  return co;
}

struct co *waiter;

void co_wait(struct co *co)
{
    assert(co != NULL);


}

void co_yield(){

}

static struct co *current;