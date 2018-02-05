## IRQ Context Switch 

### Kernel To User Task

When activating a user task, the kernel now checks the TaskDescriptor interrupt flag to determine if it is reloading a SWI or a hardware interrupted task. When reloading an IRQ, all registers are restored and ```SUBS pc, lr, #4``` is called.

### User Task to Kernel

The user state is first copied onto the irq stack and then migrated to the user stack. The kernel state is then restored, and the SPSR is read from irq_mode. A special flag is set in the user's TaskDescriptor to note that it has hardware interrupted and should do a full restore (reload r0).

## Interrupt Handling

### Interrupt Handler

The Interrupt Handler supports the syscall ```AwaitEvent``` which user tasks can invoke to block waiting for an interrupt and be awoken from the Interrupt Matrix. The kernel will run the ```event_register``` command to register the task on the interrupt specified by the user task's r0 (subsequently saved to the user stack after context switch). 

When a hardware interrupt occurs, the kernel enters via IRQ_mode, notes itself as a hardware interrupt and then runs ```event_wake``` which does a lookup in the Interrupt Matrix and wakes up all tasks blocking on the highest priority, non-empty interrupt. Any active interrupts with higher priority (which have no waiting tasks) will also be cleared to avoid multiple context switch overhead. 

Interrupt initialization and cleanup are handled at the beginning and end of the kernel. Interrupt init creates sets up the Interrupt Matrix, sets the IRQ Mode stack base, and enables interrupts.

Interrupt cleanup diables all interrupts (so we don't have to reset the box after every session). 

```c
void init_irq( interrupt_matrix *im );
void cleanup_irq();
void event_register( interrupt_matrix *im, TaskDescriptor *td);
void event_wake(interrupt_matrix *im);
```

### Interrupt Matrix

The Interrupt Matrix is a data strcture which holds all tasks blocked waiting on an interrupt. The matrix is a ```struct``` including circular buffers for each interrupt the kernel handles (improve as we go). Currently, the Interrupt Matrix only supports storing and loading tasks for the T3CUI interrupt. 

Insertion and head deletion for the task both take O(1) time. However, when waking all tasks for a corresponding interrupt requires O(n) time, for n being the number of tasks waiting on the interrupt (rarely large).

```c
void im_init(interrupt_matrix *im);
int im_push(interrupt_matrix *im, TaskDescriptor *task, InterruptEvent ie);
TaskDescriptor *im_top(interrupt_matrix *im, InterruptEvent ie);
int im_pop(interrupt_matrix *im, InterruptEvent ie);
unsigned int im_eventsize(interrupt_matrix *im, InterruptEvent ie);
```
 
