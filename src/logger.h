#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger
{
public:
    static void trace(const std::string& message);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

};

#endif // LOGGER_H
