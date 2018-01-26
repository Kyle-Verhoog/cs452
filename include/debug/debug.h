#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define SANITY() \
  bwputstr(LOG_COM, "\033[33mSANITY "__FILE__":"STR(__LINE__)"\033[0m\r\n")
#else
#define SANITY() {}
#endif

#ifdef DEBUG
//Debug functionality goes here
#define DBLOG(fmt, ...) \
  for (_i = 0; _i < _indent; _i++) \
    bwputstr(LOG_COM, " "); \
  bwprintf(LOG_COM, fmt, __VA_ARGS__);
#else
#define DBLOG() {}
#endif

#ifdef DEBUG
#define DBLOG_S()              \
  bwputstr(LOG_COM, " \033[32m✓\033[0m\r\n"); \
  _indent -= 2;
#else
#define DBLOG_S() {}
#endif

#ifdef DEBUG
#define DBLOG_F()              \
  bwputstr(LOG_COM, " \033[31m✗\033[0m\r\n");
#else
#define DBLOG_F() {}
#endif

#ifdef DEBUG
#define DBLOG_N()             \
  bwputstr(LOG_COM, "\r\n");
#else
#define DBLOG_N() {}
#endif

#ifdef DEBUG
#define DBLOG_INIT(fmt, ...) \
  int _i;               \
  int _indent = 0;      \
  DBLOG(fmt, __VA_ARGS__);           \
  DBLOG_N();            \
  _indent += 2;
#else
#define DBLOG_INIT(fmt, ...) {}
#endif

#ifdef DEBUG
#define DBLOG_START(fmt, ...) \
  DBLOG(fmt, __VA_ARGS__);            \
  _indent += 2;
#else
#define DBLOG_START(fmt, ...) {}
#endif


#ifdef DEBUG
#define IS_VALID_USER_P(tid, sp) (sp >= ((USER_STACK_BASE - ((tid & 0xffff)+1)*USER_STACK_SIZE)) && sp <= (USER_STACK_BASE - ((tid & 0xffff)*USER_STACK_SIZE)))
#else
#define IS_VALID_USER_P(tid, sp)
#endif

#ifdef DEBUG
#define IS_VALID_ID(tid) (tid >= 0 && tid < MAX_TASK)
#else
#define IS_VALID_ID(tid)
#endif


#endif /* DEBUG_H */
