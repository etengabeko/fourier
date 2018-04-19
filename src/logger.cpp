#include "logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace
{

const std::tm currentTime()
{
    using namespace std::chrono;

    const time_t now = system_clock::to_time_t(system_clock::now());
    std::tm result = *std::localtime(&now);

    return result;
}

}

void Logger::trace(const std::string& message)
{
    const std::tm tm = ::currentTime();
    std::cout << std::put_time(&tm, "%H:%M:%S") << " TRACE: " << message << std::endl;
}

void Logger::debug(const std::string& message)
{
    const std::tm tm = ::currentTime();
    std::cout << std::put_time(&tm, "%H:%M:%S") << " DEBUG: " << message << std::endl;
}

void Logger::info(const std::string& message)
{
    const std::tm tm = ::currentTime();
    std::cout << std::put_time(&tm, "%H:%M:%S") << " INFO:  " << message << std::endl;
}

void Logger::warning(const std::string& message)
{
    const std::tm tm = ::currentTime();
    std::cout << std::put_time(&tm, "%H:%M:%S") << " WARN:  " << message << std::endl;
}

void Logger::error(const std::string& message)
{
    const std::tm tm = ::currentTime();
    std::cout << std::put_time(&tm, "%H:%M:%S") << " ERROR: " << message << std::endl;
}
