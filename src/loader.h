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

#pragma once

#include <filesystem>
namespace fs = std::filesystem;

class loader
{
  void static do_prp(const fs::path& archive);
  void static do_map(const fs::path& archive);

public:
  void static do_init(const fs::path& map_archive, const fs::path& prp_archive);
};
