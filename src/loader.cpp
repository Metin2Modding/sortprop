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
#include "logger.h"

#include <print>
#include <regex>
#include <fstream>
#include <xxhash.h>
#include <unordered_set>
#include <fast_io_device.h>

std::unordered_set<std::string> crc_container;

void
loader::do_map(const fs::path& archive)
{
  logger::do_info("{} called", __FUNCTION__);
  logger::do_info("{} preloading areadata files", __FUNCTION__);

  auto find_match = std::smatch();
  auto find_regex =
    std::regex(R"(Start\s+Object\d+\s+[-\d.]+\s+[-\d.]+\s+[-\d.]+\s+(\d+)\s*)");

  for (auto& i : fs::recursive_directory_iterator(archive)) {
    if (i.is_directory())
      continue;

    auto path = i.path().string();
    std::ranges::transform(path, path.begin(), tolower);

    if (!path.contains("areadata.txt"))
      continue;

    /*
     * Map file and convert to std::string
     */
    auto file = fast_io::native_file_loader(i.path());
    auto file_view = std::string(file.data());

    /*
     * Create a regular expression to find object CRC
     */
    auto find_start = file_view.cbegin();
    auto find_stop = file_view.cend();

    while (std::regex_search(find_start, find_stop, find_match, find_regex)) {
      crc_container.emplace(find_match[1]);
      find_start = find_match.suffix().first;
    }
  }
}

#define NEW_PROPERTY_ARCHIVE "sortprop\\output\\property\\"

void
loader::do_prp(const fs::path& archive)
{
  logger::do_info("{} called", __FUNCTION__);
  logger::do_info("{} preloading property objects", __FUNCTION__);

  auto match = std::smatch();
  auto regex = std::regex(R"(YPRT\s*(\d+))");

  for (auto& i : fs::recursive_directory_iterator(archive)) {
    if (i.is_directory())
      continue;

    auto file = std::string(fast_io::native_file_loader(i.path()).data());

    if (!std::regex_search(file, match, regex))
      continue;

    if (crc_container.contains(match[1])) {
      auto path_regex =
        std::regex(R"(d:/ymir work[^"]+)", std::regex_constants::icase);
      auto path_match = std::smatch();
      if (std::regex_search(file, path_match, path_regex)) {
        auto zzz = path_match[0].str();
        auto x = zzz.substr(3);
        try {
          auto aa = fast_io::native_file_loader("sortprop/input/" + x);
          auto bb = aa.data();
          auto hash_value = XXH32(bb, aa.size(), 0);
          std::string updated_file = std::regex_replace(
            file, std::regex(match[1].str()), std::to_string(hash_value));
          auto filexx = fast_io::obuf_file(NEW_PROPERTY_ARCHIVE +
                                           std::to_string(hash_value));
          write(filexx, updated_file.begin(), updated_file.end());
          filexx.close();
        } catch (fast_io::error a) {
          std::println("{}", x);
        }
      }
    }
  }
}

void
loader::do_init(const fs::path& map_archive, const fs::path& prp_archive)
{
  logger::do_info("{} called", __FUNCTION__);

  if (!exists(map_archive) || !exists(prp_archive)) {
    logger::do_error(
      "{} - {} does not exist", __FUNCTION__, map_archive.string());
    return;
  }

  do_map(map_archive);
  do_prp(prp_archive);
}
