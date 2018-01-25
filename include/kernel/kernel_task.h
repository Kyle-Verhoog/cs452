#include <task.h>

void ktd_create( TaskDescriptor* volatile td, uint32_t tid, void *task,
                 int priority,
                 TaskStatus status, TaskDescriptor *parent);
