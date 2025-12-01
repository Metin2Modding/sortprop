#include "utils.h"

bool
utils::find_YPRT_id(sz::string_view sv, uint32_t& id_out)
{
  size_t pos = sv.find("YPRT");
  if (pos == sz::string_view::npos)
    return false;
  size_t line_end = sv.find('\n', pos);
  if (line_end == sz::string_view::npos)
    return false;
  size_t id_start = line_end + 1;
  while (id_start < sv.size() && (sv[id_start] == ' ' || sv[id_start] == '\t' ||
                                  sv[id_start] == '\r' || sv[id_start] == '\n'))
    id_start++;
  size_t id_end = id_start;
  while (id_end < sv.size() &&
         std::isdigit(static_cast<unsigned char>(sv[id_end])))
    id_end++;
  if (id_start == id_end)
    return false;
  try {
    id_out = static_cast<uint32_t>(
      std::stoul(std::string(sv.substr(id_start, id_end - id_start))));
    return true;
  } catch (...) {
    return false;
  }
}

sz::string_view
utils::find_property_type(sz::string_view sv)
{
  const char* key = "propertytype";
  size_t pos = sv.find(key);
  if (pos == sz::string_view::npos)
    return {};
  pos += strlen(key);
  while (pos < sv.size() && (sv[pos] == ' ' || sv[pos] == '\t'))
    pos++;
  if (pos >= sv.size() || sv[pos] != '"')
    return {};
  size_t start = ++pos;
  size_t end = sv.find('"', start);
  if (end == sz::string_view::npos)
    return {};
  return sv.substr(start, end - start);
}

sz::string_view
utils::find_ymir_path(sz::string_view sv)
{
  const char* key = "d:/ymir work";
  size_t pos = sv.find(key);
  if (pos == sz::string_view::npos)
    return {};
  size_t start = pos;
  size_t end = sv.find('"', start);
  if (end == sz::string_view::npos)
    end = sv.size();
  return sv.substr(start, end - start);
}

sz::string_view
utils::find_sound_path(sz::string_view sv)
{
  const char* key = "sound/ambience/";
  size_t pos = sv.find(key);
  if (pos == sz::string_view::npos)
    return {};
  size_t start = pos;
  size_t end = sv.find('"', start);
  if (end == sz::string_view::npos)
    end = sv.size();
  return sv.substr(start, end - start);
}

void
utils::extract_areadata_ids(sz::string_view sv,
                            std::unordered_map<uint32_t, uint32_t>& map)
{
  size_t pos = 0;
  while (true) {
    pos = sv.find("Start Object", pos);
    if (pos == sz::string_view::npos)
      break;
    size_t line_end = sv.find('\n', pos);
    if (line_end == sz::string_view::npos)
      line_end = sv.size();
    size_t last_space = sv.rfind(' ', line_end);
    if (last_space != sz::string_view::npos && last_space > pos) {
      auto token = sv.substr(last_space + 1, line_end - (last_space + 1));
      try {
        uint32_t id = static_cast<uint32_t>(std::stoul(std::string(token)));
        map.emplace(id, id);
      } catch (...) {
      }
    }
    pos = line_end + 1;
  }
}

void
utils::replace_id(sz::string_view input,
                  uint32_t from,
                  uint32_t to,
                  std::string& out)
{
  out.clear();
  out.reserve(input.size());
  std::string from_str = std::to_string(from);
  std::string to_str = std::to_string(to);
  size_t pos = 0;
  while (true) {
    size_t found = input.find(from_str, pos);
    if (found == sz::string_view::npos) {
      out.append(input.substr(pos));
      break;
    }
    bool left_ok =
      found == 0 || !std::isalnum(static_cast<unsigned char>(input[found - 1]));
    bool right_ok =
      found + from_str.size() >= input.size() ||
      !std::isalnum(static_cast<unsigned char>(input[found + from_str.size()]));
    if (left_ok && right_ok) {
      out.append(input.substr(pos, found - pos));
      out.append(to_str);
      pos = found + from_str.size();
    } else {
      pos = found + from_str.size();
    }
  }
}

void
utils::replace_all_ids_in_file(
  sz::string_view sv,
  const std::unordered_map<uint32_t, uint32_t>& map,
  std::string& out)
{
  out = std::string(sv);
  for (auto& [old_id, new_id] : map) {
    std::string tmp;
    replace_id(out, old_id, new_id, tmp);
    out.swap(tmp);
  }
}
