#ifndef SYSCALLS_H
#define SYSCALLS_H

void Pass();

void Block();

int Create(int priority, void (*code)());

int MyTid();

int MyParentTid();

void Exit();

#endif /* SYSCALLS_H */
