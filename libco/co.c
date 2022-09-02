#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <assert.h>

#include <string.h>
#define STACK_SIZE (1 << 16)

// Switch stack and jmp to entry
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
#define POOL_SIZE 100
static struct co *current;
static int co_pool_size = 0;

static struct co *co_pool[POOL_SIZE];
static int pool_index;



enum co_status
{
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};
struct co
{
  char *name;
  void (*func)(void *); // co_start 指定的入口地址和参数
  void *arg;

  enum co_status status;     // 协程的状态
  struct co *waiter;         // 是否有其他协程在等待当前协程
  jmp_buf context;           // 寄存器现场 (setjmp.h)
  uint8_t stack[STACK_SIZE]; // 协程的堆栈
  int pool_idx;
};

static int insert_pool(struct co *co)
{
  if(co_pool_size >= POOL_SIZE){
    return 0;
  }
  for (int i = 0; i < POOL_SIZE; i++)
  {
    if (co_pool[i] == NULL)
    {
      co_pool[i] = co;
      co_pool_size++;
      co -> pool_idx = i;
      return 1;
    }
  }
  return 0;
}

static struct co *pool_next_co()
{
  for (int i = 0; i < co_pool_size; i++)
  {
    if (co_pool[i] != NULL)
    {
      return co_pool[i];
    }
  }
  return 0;
}

// static void co_free(struct co *co){
//   co_pool[co -> pool_idx] = NULL;
//   co_pool_size--;
//   free(co);
// }
void switch_from_dead_co(struct co *co);
static void co_wrapper(void *arg){
  struct co *co = (struct co*)arg;
  co->func(co -> arg);
  co -> status = CO_DEAD;
  switch_from_dead_co(co);
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
  insert_pool(co);
  return co;
}

void switch_from_dead_co(struct co *co) {
  assert(co -> status == CO_DEAD);

  if(co -> waiter != NULL) {
    if(co -> waiter -> context != NULL){
      longjmp(co -> waiter -> context,1);
    }
  }else{
    co_yield();
  }
}
void switch_to(struct co *co){
  if(co -> status == CO_NEW){
    current = co;
    co -> status = CO_RUNNING;
    stack_switch_call(co -> stack + STACK_SIZE,co_wrapper,(uintptr_t)co);
  }else{
    longjmp(co -> context,1);
  }
}

//wait on main
//so current is main
void co_wait(struct co *co)
{
  int jmp_return = setjmp(current -> context);
  if(jmp_return == 0){
    current -> status = CO_WAITING;
    co -> waiter = current;
    switch_to(co);
  }else{
    //jmp from set jmp
    // if(co -> status == CO_DEAD){
    //   co_free(co);
    // }
  }
}
//0x5555555597c0
void co_yield ()
{
  // when yield need change to another co
  // record current context and switch to another

  current->status = CO_RUNNING;
  int jmp_return = setjmp(current->context);
  if (jmp_return == 0)
  {
    struct co *next = pool_next_co();

    if (next->status == CO_NEW)
    {
      stack_switch_call(next->stack + STACK_SIZE, co_wrapper, (uintptr_t)next);
    }
    if (next->status == CO_RUNNING)
    {
      longjmp(next->context, 1);
    }
    if (next->status == CO_DEAD)
    {
      switch_from_dead_co(next);
    }
  }else{
    //jmp from longjmp()
  }
}

__attribute__((constructor)) static void co_main_init(){
  struct co *main = co_start("main",NULL,NULL);
  main -> status = CO_RUNNING;
  current = main;
}

