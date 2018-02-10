#include <kernel/handlers/nameserver.h>


void ns_register(TaskDescriptor *td) {
  // int t;
  KASSERT(IS_VALID_TID(td->tid));
  KASSERT(IS_VALID_USER_P(td->tid, td->sp));
  // asm("ldr %0, [%1, #4];":"=r"(t):"r"(td->sp));
  // KASSERT(IS_VALID_ID(t));
  nameserver_tid = td->tid;
}

void ns_get(TaskDescriptor *td) {
  KASSERT(td != NULL);
  KASSERT(td->status != ZOMBIE && td->status != UNINIT);
  td->ret = nameserver_tid;
}
