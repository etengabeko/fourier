#include "logger.h"

#include <chrono>
#include <cstring>
#include <ctime>
#include <fstream>
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

void writeValuesToCsv(const std::string& fileName,
                      const std::vector<std::string>& titles,
                      const size_t linesCount,
                      const std::vector<std::vector<double>>& columns)
{
    std::ofstream out(fileName);
    if (!out.good())
    {
        Logger::error(strerror(errno));
        return;
    }

    bool isFirstColumn = true;
    for (const std::string& eachTitle : titles)
    {
        if (!isFirstColumn)
            out << ", ";
        else
            isFirstColumn = false;

        out << eachTitle;
    }
    out << std::endl;

    for (size_t i = 0; i < linesCount; ++i)
    {
        if (!out)
        {
            Logger::error(strerror(errno));
            return;
        }

        isFirstColumn = true;
        for (const std::vector<double>& eachColumn : columns)
        {
            if (!isFirstColumn)
                out << ", ";
            else
                isFirstColumn = false;

            out << (eachColumn.size() > i ? std::to_string(eachColumn.at(i)) : "");
        }
        out << std::endl;
    }

    Logger::info("Writed " + fileName);
}
