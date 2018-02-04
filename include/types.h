#ifndef TYPES_H
#define TYPES_H

typedef int bool;
typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef int tid_t;
#define TID_ID(tid) tid & 0xffff
typedef short int tid_id_t;
typedef short int tid_ver_t;

#endif /* TYPES_H */
