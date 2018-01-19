#ifndef DEBUG_H
#define DEBUG_H

//Debug functionality goes here
#define DBLOG(fmt) \
  for (_i = 0; _i < _indent; _i++) \
    bwputstr(LOG_COM, " "); \
  bwprintf(LOG_COM, fmt) \

#define DBLOG_S()              \
  bwputstr(LOG_COM, " \033[32m✓\033[0m\r\n"); \
  _indent -= 2;                \

#define DBLOG_F()              \
  bwputstr(LOG_COM, " \033[31m✗\033[0m\r\n"); \

#define DBLOG_N()             \
  bwputstr(LOG_COM, "\r\n");  \

#define DBLOG_INIT(fmt) \
  int _i;               \
  int _indent = 0;      \
  DBLOG(fmt);           \
  DBLOG_N();            \
  _indent += 2          \

#define DBLOG_START(fmt) \
  DBLOG(fmt);            \
  _indent += 2;          \


#endif /* DEBUG_H */
