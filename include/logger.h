#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>

#define LOG(sev, col, msg) \
  do { \
    fprintf(stderr, "\033%s%s: %s.\033[0m\n", col, sev, msg); \
  } while (0);

#define ERROR(msg) LOG("ERROR", "[31m", msg)
#define WARNING(msg) LOG("WARNING", "[33m", msg)
#define INFO(msg) LOG("INFO", "[32m", msg)

#endif
