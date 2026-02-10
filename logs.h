#ifndef LOGS_H
#define LOGS_H
#include <iostream>

enum LogLevel { LOG_NONE, LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_DEBUG };


constexpr LogLevel CURRENT_LOG_LEVEL = LOG_DEBUG;

#define LOG_DEBUG_MSG(x)   if(CURRENT_LOG_LEVEL >= LOG_DEBUG) std::cout << "[DEBUG] - " << x << std::endl
#define LOG_INFO_MSG(x)    if(CURRENT_LOG_LEVEL >= LOG_INFO)  std::cout << "[INFO] - "  << x << std::endl
#define LOG_WARNING_MSG(x) if(CURRENT_LOG_LEVEL >= LOG_WARNING) std::cout << "[WARN] - " << x << std::endl
#define LOG_ERROR_MSG(x)   if(CURRENT_LOG_LEVEL >= LOG_ERROR) std::cout << "[ERROR] - " << x << std::endl
#endif // LOGS_H
