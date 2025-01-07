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

#include <string>

class loader
{
public:
  /**
   * @param area_data
   * @param path_archive
   */
  void static initialize(bool area_data, std::string_view path_archive);
};
