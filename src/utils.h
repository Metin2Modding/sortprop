#pragma once

#include <string>
#include <unordered_map>

#include <stringzilla/stringzilla.hpp>
namespace sz = ashvardanian::stringzilla;

class utils
{
public:
  static bool find_YPRT_id(sz::string_view sv, uint32_t& id_out);
  static sz::string_view find_property_type(sz::string_view sv);
  static sz::string_view find_ymir_path(sz::string_view sv);
  static sz::string_view find_sound_path(sz::string_view sv);
  static void extract_areadata_ids(sz::string_view sv,
                                   std::unordered_map<uint32_t, uint32_t>& map);
  static void replace_id(sz::string_view input,
                         uint32_t from,
                         uint32_t to,
                         std::string& out);
  static void replace_all_ids_in_file(
    sz::string_view sv,
    const std::unordered_map<uint32_t, uint32_t>& map,
    std::string& out);
};
