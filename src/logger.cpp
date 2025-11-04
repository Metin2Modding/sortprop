// ReSharper disable All

#include "logger.h"

#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

/**
 * @brief Logger for system errors.
 */
std::shared_ptr<spdlog::logger> sp;

/**
 * @brief Logger for console output (debug mode).
 */
std::shared_ptr<spdlog::logger> console;

/*
 * Public methods
 */
void
logger::do_init()
{
  if (sp || console)
    return;

  sp = spdlog::basic_logger_mt("sp", get_text_file("sp"), true);
  sp->set_pattern("[%^%l%$] [%H:%M:%S.%e] %v");

  console = spdlog::stdout_color_mt("console");
  console->set_pattern("[%^%l%$] [%H:%M:%S.%e] %v");
}

/*
 * Private methods
 */
bool
logger::is_init()
{
  return !(!sp || !console);
}

void
logger::do_write(int32_t type, std::string message)
{
  if (!is_init())
    assert(is_init());

  std::erase(message, '\n');
  auto runtime = fmt::runtime(message);

  switch (type) {
    case 0:
      sp->info(runtime);
      console->info(runtime);
      break;

    case 1:
      sp->warn(runtime);
      console->warn(runtime);
      break;

    case 2:
      sp->error(runtime);
      console->error(runtime);
      break;
  }

  sp->flush();
  console->flush();
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
