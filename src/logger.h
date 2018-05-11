#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>

/**
 * @class Logger
 * @brief Реализует единый механизм логгирования.
 */
class Logger
{
public:
    static void trace(const std::string& message);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

};

/**
 * @brief writeValuesToCsv - записывает значения values в csv-файл с именем fileName.
 * @param fileName - имя выходного файла.
 * @param titles - список заголовков столбцов данных.
 * @param linesCount - количество записываемых строк данных.
 * @param columns - список данных для записи (по столбцам).
 */
void writeValuesToCsv(const std::string& fileName,
                      const std::vector<std::string>& titles,
                      const size_t linesCount,
                      const std::vector<std::vector<double>>& columns);

#endif // LOGGER_H
