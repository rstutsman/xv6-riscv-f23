#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include "kernel/riscv.h" // for PGSIZE
#define NCPU 0           // proc.h needs these
struct spinlock {};
#define NOFILE 0
#include "kernel/proc.h" // for struct context

#define assert(x) if (!(x)) { printf("assertion failed: %s\n", #x); exit(1); }

extern void swtch(struct context *prev, struct context *next);

struct context parent_ctxt;
struct context child_ctxt;

void parent_main() {
  while (1) {
    printf("parent_main\n");
    swtch(&parent_ctxt, &child_ctxt);
    sleep(1);
  }
}

void child_main() {
  while (1) {
    printf("child_main\n");
    swtch(&child_ctxt, &parent_ctxt);
    sleep(1);
  }
}

void thread_create(struct context *ctxt, void (*entry)(void)) {
  void* child_stack = malloc(PGSIZE);
  assert(child_stack);
  memset(ctxt, 0, sizeof(*ctxt));
  memset(child_stack, 0, PGSIZE);
  child_ctxt.sp = (uint64)child_stack + PGSIZE;
  child_ctxt.ra = (uint64)entry;
}

int main(int argc, char* argv[]) {
  thread_create(&child_ctxt, child_main);
  parent_main();
  return 0;
}

/*
struct context *current = &parent_ctxt;

void yield() {
  if (current == &parent_ctxt) {
    current = &child_ctxt;
    swtch(&parent_ctxt, &child_ctxt);
  } else {
    current = &parent_ctxt;
    swtch(&child_ctxt, &parent_ctxt);
  }
}
*/
