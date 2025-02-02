// ReSharper disable All

#include "logger.h"

#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>

#ifdef _DEBUG
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

/**
 * @brief Logger for system errors.
 */
std::shared_ptr<spdlog::logger> sp;

#ifdef _DEBUG
/**
 * @brief Logger for console output (debug mode).
 */
std::shared_ptr<spdlog::logger> console;
#endif

/*
 * Public methods
 */
void
logger::do_init()
{
#ifndef _DEBUG
  if (sp)
#else
  if (sp || console)
#endif
    return;

  sp = spdlog::basic_logger_mt("sp", get_text_file("sp"), true);
  sp->set_pattern("[%^%l%$] [%H:%M:%S.%e] %v");

#ifdef _DEBUG
  console = spdlog::stdout_color_mt("console");
  console->set_pattern("[%^%l%$] [%H:%M:%S.%e] %v");
#endif
}

/*
 * Private methods
 */
bool
logger::is_init()
{
#ifndef _DEBUG
  return !(!sp);
#else
  return !(!sp || !console);
#endif
}

void
logger::do_write(bool info, std::string message)
{
  if (!is_init())
    assert(is_init());

  std::erase(message, '\n');
  auto runtime = fmt::runtime(message);

  if (!info) {
    sp->error(runtime);
    sp->flush();

#ifdef _DEBUG
    console->error(runtime);
    console->flush();
#endif
  } else {
#ifdef _DEBUG
    sp->info(runtime);
    sp->flush();

    console->info(runtime);
    console->flush();
#endif
  }
}

std::string
logger::get_text_file(std::string name)
{
  return fmt::format("log/{}_{}.txt", name, get_current_time());
}

std::string
logger::get_current_time()
{
  // Get time
  const auto time = std::time(nullptr);

  // Create stream
  std::ostringstream time_fmt;
  time_fmt << std::put_time(std::localtime(&time), "%H%M%S_%Y%m%d");

  // Return stream
  return time_fmt.str();
}
