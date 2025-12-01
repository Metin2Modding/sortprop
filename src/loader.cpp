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

#include "utils.h"
#include <stringzilla/stringzilla.hpp>

std::unordered_map<uint32_t, uint32_t> hash_container;

inline std::string
utf8(const fs::path& p)
{
  auto s = p.u8string();
  return std::string(reinterpret_cast<const char*>(s.c_str()), s.size());
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
      sz::string_view file_view((file.data()), file.size());
      utils::extract_areadata_ids(file_view, hash_container);
      std::error_code ec;
      fs::path rel = fs::relative(i.path().parent_path(), base, ec);
      if (ec)
        rel = i.path().parent_path().filename();
      fs::path target_dir = out_root / rel;
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

  fs::path out_root = base.parent_path() / "sortprop";
  fs::path property_out_dir = out_root / "property" / "property";
  std::error_code ec;
  fs::create_directories(property_out_dir, ec);

  for (auto& i : fs::recursive_directory_iterator(base)) {
    try {
      if (i.is_directory())
        continue;

      auto file_loader = fast_io::native_file_loader(i.path());
      sz::string_view file_view((file_loader.data()), file_loader.size());

      uint32_t id_val = 0;
      if (!utils::find_YPRT_id(file_view, id_val))
        continue;

      if (!hash_container.contains(id_val))
        hash_container[id_val] = id_val;

      auto type_sv = utils::find_property_type(file_view);
      std::string property_type =
        type_sv.empty() ? "unknown" : std::string(type_sv);

      std::ranges::transform(property_type.begin(),
                             property_type.end(),
                             property_type.begin(),
                             [](unsigned char c) { return std::tolower(c); });

      fs::path property_type_dir = property_out_dir / property_type;
      fs::create_directories(property_type_dir, ec);

      auto ymir_sv = utils::find_ymir_path(file_view);
      if (!ymir_sv.empty()) {
        std::string x = std::string(ymir_sv);
        x = x.substr(3);
        const std::string prefix = "ymir work/";
        auto pos = x.find(prefix);
        if (pos != std::string::npos)
          x.erase(pos, prefix.length());

        // trim left
        while (!x.empty() &&
               std::isspace(static_cast<unsigned char>(x.front())))
          x.erase(x.begin());

        // trim right
        while (!x.empty() && std::isspace(static_cast<unsigned char>(x.back())))
          x.pop_back();

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

          auto hash_value_raw =
#ifdef USE_XXHASH
            XXH32(bb, size, 0);
#elif defined(USE_CRC32)
            GetCaseCRC32(bb, size);
#endif

          uint32_t hash_value = static_cast<uint32_t>(hash_value_raw);
          std::string updated_file;
          utils::replace_id(file_view, id_val, hash_value, updated_file);
          fs::path out_file = property_type_dir / std::to_string(hash_value);

          try {
            auto filexx = fast_io::obuf_file(out_file.string());
            write(filexx, updated_file.begin(), updated_file.end());
            filexx.close();
            hash_container[id_val] = hash_value;
          } catch (...) {
            logger::do_error("Cannot write property file {}",
                             out_file.string());
          }
        } catch (...) {
          logger::do_error("Cannot find referenced file {}",
                           referenced.string());
        }
      }

      auto sound_sv = utils::find_sound_path(file_view);
      if (!sound_sv.empty()) {
        fs::path referenced =
          normalize_path(base / "sound" / fs::path(std::string(sound_sv)));

        try {
          auto aa = fast_io::native_file_loader(referenced);
          auto bb = aa.data();
          auto size = aa.size();

          auto hash_value_raw =
#ifdef USE_XXHASH
            XXH32(bb, size, 0);
#elif defined(USE_CRC32)
            GetCaseCRC32(bb, size);
#endif

          uint32_t hash_value = static_cast<uint32_t>(hash_value_raw);

          std::string updated_file;
          utils::replace_id(file_view, id_val, hash_value, updated_file);

          fs::path out_file = property_type_dir / std::to_string(hash_value);

          try {
            auto filexx = fast_io::obuf_file(out_file.string());
            write(filexx, updated_file.begin(), updated_file.end());
            filexx.close();
            hash_container[id_val] = hash_value;
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

  for (auto& i : fs::recursive_directory_iterator(out_root)) {
    if (i.is_directory())
      continue;

    std::string filename = i.path().filename().string();
    std::string filename_lower = filename;
    std::ranges::transform(filename_lower.begin(),
                           filename_lower.end(),
                           filename_lower.begin(),
                           [](unsigned char c) { return std::tolower(c); });

    if (filename_lower != "areadata.txt" &&
        filename_lower != "areaambiencedata.txt")
      continue;

    auto loader_file = fast_io::native_file_loader(i.path());
    sz::string_view file_view((loader_file.data()), loader_file.size());

    std::string output;
    utils::replace_all_ids_in_file(file_view, hash_container, output);

    try {
      fs::remove(i.path());
      auto filexx = fast_io::obuf_file(i.path().string());
      write(filexx, output.begin(), output.end());
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
