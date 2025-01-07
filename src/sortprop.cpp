#include <print>
#include <regex>
#include <xxhash.h>
#include <filesystem>
#include <unordered_set>
#include <fast_io_device.h>

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
        auto path_regex =
          std::regex(R"(d:/ymir work[^"]+)", std::regex_constants::icase);

        auto path_match = std::smatch();

        if (std::regex_search(file, path_match, path_regex)) {
          auto zzz = path_match[0].str();
          auto x = zzz.substr(3);

          try {
            auto aa = fast_io::native_file_loader(x);
            auto bb = aa.data();
            uint32_t hash_value = XXH32(bb, aa.size(), 0);
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
}
