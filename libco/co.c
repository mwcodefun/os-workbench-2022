#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
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


enum co_status
{
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};

typedef void (*co_handler_t)(void *arg);

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

void (*co_handler[CO_DEAD + 1]) (struct co *co);
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
static void co_wrapper(void *arg);
void co_switch_new(struct co *co){
  current = co;
  co -> status = CO_RUNNING;
  stack_switch_call(co -> stack + STACK_SIZE,co_wrapper,(uintptr_t)co);
}

void co_switch_running(struct co* co){
  current = co;
  longjmp(co -> context,1);
}

void co_switch_waiting(struct co* co){
   assert(0);
}


static void *co_switch[] = {
  [CO_NEW] = co_switch_new,
  [CO_RUNNING] = co_switch_running,
  [CO_WAITING] = co_switch_waiting,
  [CO_DEAD] = switch_from_dead_co,
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
  int id = rand() % co_pool_size; 
  for (int i = id; i <= co_pool_size + id; i++)
  {
    if (co_pool[i % co_pool_size] != NULL)
    {
      struct co *co = co_pool[i % co_pool_size];
      if(co == NULL) continue;
      if(co == current) continue;
      if(co -> status == CO_DEAD) continue;
      if(co -> status == CO_WAITING) continue;
      return co;
    }
  }
  return 0;
}


void co_free(struct co *co) {
	assert((co != NULL));

	co_pool[co->pool_idx] = NULL;
	co_pool_size -- ;

	if (!co->name) {
		free(co->name);
	}
	free(co);
}


void switch_from_dead_co(struct co *co);
static void co_wrapper(void *arg){
  struct co *co = (struct co*)arg;
  co->func(co -> arg);
  co -> status = CO_DEAD;
  ((co_handler_t)co_handler[CO_DEAD])(co);
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


void switch_to(struct co *co){
  ((co_handler_t)co_handler[co -> status])(co);
}

//wait on main
//so current is main

void co_wait(struct co *co)
{
  int jmp_return = setjmp(current -> context);
  if(jmp_return == 0){
    current -> status = CO_WAITING;
    co -> waiter = current;
    
    ((co_handler_t)co_handler[CO_DEAD])(co);

  }else{
    co_free(co);
		current->status = CO_RUNNING;
  }
}

void co_yield ()
{
  // when yield need change to another co
  // record current context and switch to another
  int jmp_return = setjmp(current->context);
  if (jmp_return == 0)
  {
    struct co *next = pool_next_co();
    printf("switch to:%s\n",next->name);
    if(next == NULL){
      longjmp(current -> context,1);
    }
    assert(next -> status <= CO_RUNNING);
    ((co_handler_t)co_handler[next -> status])(next);
  }else{
    //jmp from longjmp()
  }
}

__attribute__((destructor)) static void co_pool_free(){
  co_free(co_pool[0]);
}


__attribute__((constructor)) static void co_main_init(){
  for(int i = 0;i < co_pool_size;i++){
    co_pool[i] = NULL;
  }

  struct co *main = co_start("main",NULL,NULL);
  main -> status = CO_RUNNING;
  current = main;
}

