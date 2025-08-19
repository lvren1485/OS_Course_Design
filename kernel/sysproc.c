#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h" // 添加这一行

// 在文件顶部添加extern声明，告诉编译器函数在其他文件中定义
extern uint64 kfreemem(void);
extern uint64 nproc(void);

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;

  // 从用户空间获取第一个参数（mask）
  if(argint(0, &mask) < 0)
    return -1;
  
  // 将 mask 赋值给当前进程的 trace_mask 字段
  myproc()->trace_mask = mask;
  return 0;
}

uint64
sys_sysinfo(void)
{
  uint64 addr; // user pointer to struct sysinfo
  struct sysinfo info;

  // 从用户空间获取参数（指向sysinfo结构体的指针）
  if(argaddr(0, &addr) < 0)
    return -1;

  // 填充sysinfo结构体
  info.freemem = kfreemem();
  info.nproc = nproc();

  // 使用copyout将内核中的info结构体复制到用户空间地址addr处
  if(copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}
