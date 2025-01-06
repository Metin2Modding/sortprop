#include <filesystem>
#include <fstream>
#include <print>
#include <regex>
#include <unordered_set>

#include "fast_io/fast_io.h"
#include "fast_io/fast_io_device.h"
#include "xxhash/xxhash.h"

#define MAP_ARCHIVE "map\\"
#define OLD_PROPERTY_ARCHIVE "property\\"
#define NEW_PROPERTY_ARCHIVE "property_sortprop\\"

int
main()
{
  std::unordered_set<std::string> crc_container;

  /*
   * S T E P    F I R S T
   * We need to search CRCs inside the AreaData files
   */
  {
    for (auto& i : std::filesystem::recursive_directory_iterator(MAP_ARCHIVE)) {
      /*
       * F I L E
       */
      auto& path = i.path();

      if (path.filename() != "areadata.txt")
        continue;

      auto file = std::string(fast_io::native_file_loader(path).data());

      /*
       * R E G E X
       */
      auto regex = std::regex(
        R"(Start\s+Object\d+\s+[-\d.]+\s+[-\d.]+\s+[-\d.]+\s+(\d+)\s*)");

      /*
       * M A T C H    R E G E X
       */
      auto match = std::smatch();
      auto start = file.cbegin();

      while (std::regex_search(start, file.cend(), match, regex)) {
        crc_container.emplace(match[1]);
        start = match.suffix().first;
      }
    }
  }

  std::ofstream out("abc.txt");

  /*
   * S T E P    S E C O N D
   * We need to search founded CRCs inside the old Property archive
   */
  {
    for (auto i :
         std::filesystem::recursive_directory_iterator(OLD_PROPERTY_ARCHIVE)) {
      /*
       * F I L E
       */
      if (i.is_directory())
        continue;

      auto file = std::string(fast_io::native_file_loader(i.path()).data());

      /*
       * R E G E X
       */
      auto regex = std::regex(R"(YPRT\s*(\d+))");

      /*
       * M A T C H    R E G E X
       */
      auto match = std::smatch();

      if (!std::regex_search(file, match, regex))
        continue;

      if (crc_container.contains(match[1])) {
        // std::println("matched regex: {}", match[1].str());

        // New regex to capture the file path
        auto path_regex =
          std::regex(R"(d:/ymir work[^"]+)", std::regex_constants::icase);

        // Match the path
        auto path_match = std::smatch();
        if (std::regex_search(file, path_match, path_regex)) {
          // std::println("Found path: {}", path_match[0].str());
          // path_match[0].str());
          auto zzz = path_match[0].str();
          auto x = zzz.substr(3);
          out << std::format("{} - {}\n", match[1].str(), x);

          try {
            auto aa = fast_io::native_file_loader(x);
            auto bb = aa.data();

            /*
             * HASH PREPARED REPLACE PROPERTY FILE AND AREADATA
             */
            uint32_t hash_value = XXH32(bb, aa.size(), 0);
            // std::println(
            //     "Original CRC: {} -> New Hash: {}", match[1].str(),
            //     hash_value);

            /*
             * R E P L A C E   T H E   C R C
             * We will now replace the found CRC with the calculated hash
             * (XXH32). Regex replace the matched CRC in the file content.
             */
            std::string updated_file = std::regex_replace(
              file, std::regex(match[1].str()), std::to_string(hash_value));

            // std::print("{}", updated_file);

            // Optionally, save the updated content to a new file or process
            // further For example, writing to a new file:
            // std::filesystem::create_directory(NEW_PROPERTY_ARCHIVE);
            // std::filesystem::create_directories(NEW_PROPERTY_ARCHIVE + '\\' +
            // i.path().string());
            auto filexx = fast_io::obuf_file(NEW_PROPERTY_ARCHIVE +
                                             std::to_string(hash_value));
            write(filexx, updated_file.begin(), updated_file.end());
            filexx.close();

            // std::ofstream updated_file_out(NEW_PROPERTY_ARCHIVE +
            //                                std::to_string(hash_value),
            //                                std::ios::out | std::ios::);
            // updated_file_out << updated_file;
          } catch (fast_io::error a) {
            std::println("{}", x);
          }
        }
      }
    }
  }
}
