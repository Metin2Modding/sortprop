#include <print>

#include "loader.h"
#include "logger.h"

int
main()
{
  // std
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

  // logger
  logger::do_init();

  // loader
#define MAP_ARCHIVE "sortprop\\input\\map\\"
#define PROPERTY_ARCHIVE "sortprop\\input\\property\\"

  loader::do_init(MAP_ARCHIVE, PROPERTY_ARCHIVE);
}
