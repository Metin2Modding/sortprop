/*
 *                 _
 *  ___  ___  _ __| |_ _ __  _ __ ___  _ __
 * / __|/ _ \| '__| __| '_ \| '__/ _ \| '_ \
 * \__ \ (_) | |  | |_| |_) | | | (_) | |_) |
 * |___/\___/|_|   \__| .__/|_|  \___/| .__/
 *                    |_|             |_|
 *
 * Fast and automatic Property files sorter
 * https://github.com/Metin2Modding/sortprop
 */

// ReSharper disable All

#pragma once

#include <spdlog/logger.h>

/**
 * @class Logger
 * @brief Provides logging functionality.
 */
class logger
{
public:
  /**
   * @brief Initializes the loggers.
   */
  static void do_init();

  /**
   * @brief Logs an informational message.
   * @param message The message format string.
   * @param arguments Additional arguments for the format string.
   */
  template<typename... T>
  static void do_info(std::string message, T&&... arguments)
  {
    do_write(0,
             fmt::format(fmt::runtime(message), std::forward<T>(arguments)...));
  }

  /**
   * @brief Logs an warning message.
   * @param message The message format string.
   * @param arguments Additional arguments for the format string.
   */
  template<typename... T>
  static void do_warn(std::string message, T&&... arguments)
  {
    do_write(1,
             fmt::format(fmt::runtime(message), std::forward<T>(arguments)...));
  }

  /**
   * @brief Logs an error message.
   * @param message The message format string.
   * @param arguments Additional arguments for the format string.
   */
  template<typename... T>
  static void do_error(std::string message, T&&... arguments)
  {
    do_write(2,
             fmt::format(fmt::runtime(message), std::forward<T>(arguments)...));
  }

  /**
   * @brief Gets the current time as a string.
   * @return A string representing the current time in "YYYYMMDD_HHMMSS" format.
   */
  static std::string get_current_time();

private:
  /**
   * @brief Checks if loggers are initialized.
   * @return True if loggers are initialized, false otherwise.
   */
  static bool is_init();

  /**
   * @brief Writes a log message.
   * @param info True for informational messages, false for error messages.
   * @param message The message format string.
   * @param arguments The formatted message string.
   */
  static void do_write(int32_t info, std::string message);

  /**
   * @brief Generates a filename for a text file with the given name and the
   * current timestamp.
   * @param name A string representing the base name for the file.
   * @return A string representing the complete filename in the format
   * "log/<name>_<timestamp>.txt", where <name> is the provided base name and
   * <timestamp> is the current time in "YYYYMMDD_HHMMSS" format.
   */
  static std::string get_text_file(std::string name);
};
