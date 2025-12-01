#pragma once

#include <string>
#include <unordered_map>

class utils
{
public:
  static bool find_YPRT_id(std::string_view sv, uint32_t& id_out);
  static std::string_view find_property_type(std::string_view sv);
  static std::string_view find_ymir_path(std::string_view sv);
  static std::string_view find_sound_path(std::string_view sv);
  static void extract_areadata_ids(std::string_view sv,
                                   std::unordered_map<uint32_t, uint32_t>& map);
  static void replace_id(std::string_view input,
                         uint32_t from,
                         uint32_t to,
                         std::string& out);
  static void replace_all_ids_in_file(
    std::string_view sv,
    const std::unordered_map<uint32_t, uint32_t>& map,
    std::string& out);
};
