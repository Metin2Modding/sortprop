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

#include "loader.h"

#include <fast_io.h>
#include <filesystem>
#include <print>
#include <regex>

void
loader::initialize(bool area_data, std::string_view path_archive)
{
  namespace fs = std::filesystem;

  if (area_data) {
    for (auto& i : fs::recursive_directory_iterator(path_archive)) {
      if (i.path().filename() != "areadata.txt") {
        /*
         * Filename may be PascalCase
         * so we need to std::transform
         */
        continue;
      }

      /*
       * Map file and convert to std::string
       */
      auto file = fast_io::native_file_loader(i.path());
      auto file_view = std::string(file.data());

      /*
       * Create a regular expression to find object CRC
       */
      auto find_regex = std::regex(
        R"(Start\s+Object\d+\s+[-\d.]+\s+[-\d.]+\s+[-\d.]+\s+(\d+)\s*)");
      auto find_match = std::smatch();
      auto find_start = file_view.cbegin();

      while (std::regex_search(
        find_start, file_view.cend(), find_match, find_regex)) {
        /*
         * Found any match, great!
         */
        std::println("{} - {}", find_match[1].str(), i.path().string());
        find_start = find_match.suffix().first;
      }
    }
  }
}
