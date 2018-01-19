#ifndef DEBUG_H
#define DEBUG_H

#define SANITY() \
  bwputstr(LOG_COM, "\033[33mSANITY "__FILE__":"STR(__LINE__)"\033[0m\r\n") \

//Debug functionality goes here
#define DBLOG(fmt, ...) \
  for (_i = 0; _i < _indent; _i++) \
    bwputstr(LOG_COM, " "); \
  bwprintf(LOG_COM, fmt, __VA_ARGS__); \

#define DBLOG_S()              \
  bwputstr(LOG_COM, " \033[32m✓\033[0m\r\n"); \
  _indent -= 2;                \

#define DBLOG_F()              \
  bwputstr(LOG_COM, " \033[31m✗\033[0m\r\n"); \

#define DBLOG_N()             \
  bwputstr(LOG_COM, "\r\n");  \

#define DBLOG_INIT(fmt, ...) \
  int _i;               \
  int _indent = 0;      \
  DBLOG(fmt, __VA_ARGS__);           \
  DBLOG_N();            \
  _indent += 2;          \

#define DBLOG_START(fmt, ...) \
  DBLOG(fmt, __VA_ARGS__);            \
  _indent += 2;                       \


#endif /* DEBUG_H */
