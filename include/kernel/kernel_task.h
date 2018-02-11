#include <task/task.h>

void ktd_create( TaskDescriptor* volatile td, tid_t tid, void *task,
                 int priority,
                 TaskStatus status, TaskDescriptor *parent);
