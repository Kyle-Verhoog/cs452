#include <task.h>

void ktd_create(volatile TaskDescriptor *td, uint32_t tid, volatile void *task, int priority,
                TaskStatus status, volatile TaskDescriptor *parent);
