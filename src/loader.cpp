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

#define USE_CRC32
// #define USE_XXHASH

#if defined(USE_CRC32) && defined(USE_XXHASH)
#error "Cannot use both USE_CRC32 and USE_XXHASH"
#endif

#include "loader.h"
#include "logger.h"

#include <fast_io_device.h>
#include <regex>
#include <unordered_set>

#ifdef USE_CRC32
#include "crc32.h"
#endif

#ifdef USE_XXHASH
#include <xxhash.h>
#endif

std::unordered_map<std::string, std::string> hash_container;

inline std::string
utf8(const fs::path& p)
{
  auto s = p.u8string();
  return std::string(reinterpret_cast<const char*>(s.c_str()), s.size());
}

static fs::path
make_relative_or_full(const fs::path& p, const fs::path& base)
{
  std::error_code ec;
  fs::path rel = p.lexically_relative(base);
  if (rel.empty() || rel.string().find("..") == 0) {
    return p;
  }
  return rel;
}

inline fs::path
normalize_path(const fs::path& p)
{
  std::string s = utf8(p);
  std::replace(s.begin(), s.end(), '\\', '/');
  return fs::path(s);
}

void
loader::do_map(const fs::path& base)
{
  logger::do_info("{} called", __FUNCTION__);
  logger::do_info("{} preloading areadata files", __FUNCTION__);

  auto find_match = std::smatch();
  auto find_regex =
    std::regex(R"(Start\s+Object\d+\s+[-\d.]+\s+[-\d.]+\s+[-\d.]+\s+(\d+)\s*)",
               std::regex_constants::icase);

  fs::path out_root = base.parent_path() / "sortprop";
  for (auto& i : fs::recursive_directory_iterator(base)) {
    try {
      if (i.is_directory())
        continue;

      std::string filename_lower = utf8(i.path().filename());
      std::ranges::transform(filename_lower.begin(),
                             filename_lower.end(),
                             filename_lower.begin(),
                             [](unsigned char c) { return std::tolower(c); });

      if (filename_lower.find("areadata.txt") == std::string::npos &&
          filename_lower.find("areaambiencedata.txt") == std::string::npos)
        continue;

      auto file = fast_io::native_file_loader(i.path());
      auto file_view = std::string(file.data(), file.size());

      auto find_start = file_view.cbegin();
      auto find_stop = file_view.cend();

      while (std::regex_search(find_start, find_stop, find_match, find_regex)) {
        hash_container.emplace(find_match[1], find_match[1]);
        find_start = find_match.suffix().first;
      }

      fs::path rel = make_relative_or_full(i.path().parent_path(), base);
      fs::path target_dir = out_root / rel;
      std::error_code ec;
      fs::create_directories(target_dir, ec);
      if (ec) {
        logger::do_error(
          "Failed to create directory {} : {}", utf8(target_dir), ec.message());
        continue;
      }

      fs::path target_file = target_dir / i.path().filename();
      try {
        auto filexx = fast_io::obuf_file(utf8(target_file));
        write(filexx, file_view.begin(), file_view.end());
        filexx.close();
      } catch (...) {
        logger::do_error("Cannot write file {}", utf8(target_file));
      }
    } catch (const std::exception& e) {
      logger::do_error("{}: {}", utf8(i.path()), e.what());
    }
  }
}

void
loader::do_prp(const fs::path& base)
{
  logger::do_info("{} called", __FUNCTION__);
  logger::do_info("{} preloading property objects", __FUNCTION__);

  auto match = std::smatch();
  auto regex = std::regex(R"(YPRT\s*(\d+))", std::regex_constants::icase);

  fs::path out_root = base.parent_path() / "sortprop";
  fs::path property_out_dir = out_root / "property" / "property";
  std::error_code ec;
  fs::create_directories(property_out_dir, ec);

  std::regex type_regex("propertytype[ \t]*\"([^\"]+)\"",
                        std::regex_constants::icase);

  for (auto& i : fs::recursive_directory_iterator(base)) {
    try {
      if (i.is_directory())
        continue;

      auto file_loader = fast_io::native_file_loader(i.path());
      auto file = std::string(file_loader.data(), file_loader.size());

      if (!std::regex_search(file, match, regex))
        continue;

      auto id = match[1].str();
      if (!hash_container.contains(id))
        continue;

      std::smatch type_match;
      std::string property_type = "unknown";
      if (std::regex_search(file, type_match, type_regex)) {
        property_type = type_match[1].str();
        std::ranges::transform(property_type,
                               property_type.begin(),
                               [](unsigned char c) { return std::tolower(c); });
      }
      fs::path property_type_dir = property_out_dir / property_type;
      fs::create_directories(property_type_dir, ec);

      auto path_regex =
        std::regex(R"(d:/ymir work[^"]+)", std::regex_constants::icase);
      auto path_match = std::smatch();
      if (std::regex_search(file, path_match, path_regex)) {
        auto zzz = path_match[0].str();
        auto x = zzz.substr(3);
        const std::string prefix = "ymir work/";
        auto pos = x.find(prefix);
        if (pos != std::string::npos)
          x.erase(pos, prefix.length());
        x.erase(0, x.find_first_not_of(" \t\r\n"));
        x.erase(x.find_last_not_of(" \t\r\n") + 1);
        auto slash_pos = x.find('/');
        if (slash_pos != std::string::npos) {
          std::string first_segment = x.substr(0, slash_pos);
          x.insert(slash_pos + 1, "ymir work/" + first_segment + "/");
        }
        fs::path referenced = normalize_path(base / fs::path(x));
        try {
          auto aa = fast_io::native_file_loader(referenced);
          auto bb = aa.data();
          auto size = aa.size();
          auto hash_value =
#ifdef USE_XXHASH
            XXH32(bb, size, 0);
#elif defined(USE_CRC32)
            GetCaseCRC32(bb, size);
#endif
          std::string updated_file = std::regex_replace(
            file, std::regex(id), std::to_string(hash_value));
          fs::path out_file = property_type_dir / std::to_string(hash_value);
          try {
            auto filexx = fast_io::obuf_file(out_file.string());
            write(filexx, updated_file.begin(), updated_file.end());
            filexx.close();
            hash_container[id] = std::to_string(hash_value);
          } catch (...) {
            logger::do_error("Cannot write property file {}",
                             out_file.string());
          }
        } catch (...) {
          logger::do_error("Cannot find referenced file {}",
                           referenced.string());
        }
      }

      auto path_regex2 =
        std::regex(R"(sound/ambience/[^"]+)", std::regex_constants::icase);
      if (std::regex_search(file, path_match, path_regex2)) {
        auto zzz = path_match[0].str();
        fs::path referenced = normalize_path(base / "sound" / fs::path(zzz));
        try {
          auto aa = fast_io::native_file_loader(referenced);
          auto bb = aa.data();
          auto size = aa.size();
          auto hash_value =
#ifdef USE_XXHASH
            XXH32(bb, size, 0);
#elif defined(USE_CRC32)
            GetCaseCRC32(bb, size);
#endif
          std::string updated_file = std::regex_replace(
            file, std::regex(id), std::to_string(hash_value));
          fs::path out_file = property_type_dir / std::to_string(hash_value);
          try {
            auto filexx = fast_io::obuf_file(out_file.string());
            write(filexx, updated_file.begin(), updated_file.end());
            filexx.close();
            hash_container[id] = std::to_string(hash_value);
          } catch (...) {
            logger::do_error("Cannot write property file {}",
                             out_file.string());
          }
        } catch (...) {
          logger::do_error("Cannot find referenced file {}",
                           referenced.string());
        }
      }
    } catch (const fast_io::error& e) {
      logger::do_error("{}", utf8(i.path()));
    }
  }

  auto do_replace =
    [](std::string& str, const std::string& from, const std::string& to) {
      size_t start_pos = 0;
      while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        bool left_ok =
          (start_pos == 0 ||
           !std::isalnum(static_cast<unsigned char>(str[start_pos - 1])));
        bool right_ok = (start_pos + from.length() == str.length() ||
                         !std::isalnum(static_cast<unsigned char>(
                           str[start_pos + from.length()])));
        if (left_ok && right_ok) {
          str.replace(start_pos, from.length(), to);
          start_pos += to.length();
        } else {
          start_pos += from.length();
        }
      }
    };

  for (auto& i : fs::recursive_directory_iterator(out_root)) {
    if (i.is_directory())
      continue;

    std::string filename = i.path().filename().string();
    std::string filename_lower = filename;
    std::ranges::transform(filename_lower,
                           filename_lower.begin(),
                           [](unsigned char c) { return std::tolower(c); });

    if (filename_lower != "areadata.txt" &&
        filename_lower != "areaambiencedata.txt")
      continue;

    auto loader_file = fast_io::native_file_loader(i.path());
    std::string file = std::string(loader_file.data(), loader_file.size());

    for (auto& [fst, snd] : hash_container) {
      do_replace(file, "    " + fst, "    " + snd);
    }

    try {
      fs::remove(i.path());
      auto filexx = fast_io::obuf_file(i.path().string());
      write(filexx, file.begin(), file.end());
      filexx.close();
    } catch (...) {
      logger::do_error("Cannot write replaced file {}", i.path().string());
    }
  }
}

void
loader::do_init(const fs::path& base)
{
  logger::do_info("{} called", __FUNCTION__);

  if (!fs::exists(base)) {
    logger::do_error("{} - {} does not exist", __FUNCTION__, base.string());
    return;
  }

  do_map(base);
  do_prp(base);
}
