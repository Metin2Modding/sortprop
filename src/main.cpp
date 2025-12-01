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

#include <print>

#include "loader.h"
#include "logger.h"

int
main()
{
  std::println(R"(                _                         )");
  std::println(R"( ___  ___  _ __| |_ _ __  _ __ ___  _ __  )");
  std::println(R"(/ __|/ _ \| '__| __| '_ \| '__/ _ \| '_ \ )");
  std::println(R"(\__ \ (_) | |  | |_| |_) | | | (_) | |_) |)");
  std::println(R"(|___/\___/|_|   \__| .__/|_|  \___/| .__/ )");
  std::println(R"(                   |_|             |_|    )");
  std::println(R"(                                          )");
  std::println(R"(Fast and automatic Property files sorter  )");
  std::println(R"(https://github.com/Metin2Modding/sortprop )");
  std::println(R"(                                          )");

  logger::do_init();
  loader::do_init(std::filesystem::current_path());
}
