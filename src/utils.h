#pragma once
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "types.h"

namespace UTUI {
class Utils {
public:
  static std::string multiplyString(const std::string &str, int times) {
    std::string rstr;
    rstr.reserve(str.length() * times);
    for (int i = 0; i < times; i++) {
      rstr.append(str);
    }

    return rstr;
  }
  static std::vector<std::string> readFile(const std::string &filename,
                                           char delimiter = '\n') {
    std::vector<std::string> lines;
    std::ifstream file(filename);

    if (!file.is_open()) {
      Utils::throwError("Could not open " + filename + "!\n");
      return lines;
    };

    std::string line;
    while (std::getline(file, line, delimiter)) {
      if (delimiter != '\n') {
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
      }

      lines.push_back(line);
    }
    return lines;
  }
  static std::vector<std::string> splitString(const std::string &str,
                                              char delimiter) {
    std::string buffer = "";
    std::vector<std::string> result;

    for (int i = 0; i < str.size(); i++) {
      if ((str[i] == delimiter) && (i > 0 && str[i - 1] != '\\')) {
        result.emplace_back(buffer);
        buffer.clear();
      } else if (str[i] != '\\' || (str[i] == '\\' && i + 1 < str.size() &&
                                    str[i + 1] != delimiter))
        buffer += str[i];
    }

    if (!buffer.empty()) {
      result.emplace_back(buffer);
    }

    return result;
  }
  static std::string mergeString(const std::vector<std::string> &str,
                                 char delimiter) {
    std::string buf;

    for (const std::string &s : str) {
      buf += s + delimiter;
    }

    return buf;
  }
  static bool isInBoundaries(const Vector2 &position, const Vector2 &size,
                             const Vector2 cursorPosition) {
    return cursorPosition.x >= position.x &&
           cursorPosition.x < (position.x + size.x) &&
           cursorPosition.y >= position.y &&
           cursorPosition.y < (position.y + size.y);
  }
  static int getStringWidth(const std::string &str) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    const std::wstring wstr = converter.from_bytes(str);
    int width = wcswidth(wstr.c_str(), wstr.size());

    return width;
  }
  static Color stringToColor(const std::string &str) {

    if (int hashPosition = str.find_first_of('#');
        hashPosition != std::string::npos) {
      return {std::stoi(str.substr(hashPosition + 1, 2), 0, 16),
              std::stoi(str.substr(hashPosition + 3, 2), 0, 16),
              std::stoi(str.substr(hashPosition + 5, 2), 0, 16)};
    }

    std::vector<std::string> values = splitString(str, ',');

    if (values.size() < 3) {
      return {0, 0, 0};
    }
    return {std::stoi(values[0]), std::stoi(values[1]), std::stoi(values[2])};
  }
  static Vector2 stringToVector2(const std::string &str) {
    std::vector<std::string> values = splitString(str, ',');
    if (values.size() < 2) {
      return {0, 0};
    }
    return {std::stoi(values[0]), std::stoi(values[1])};
  }
  static void throwError(const std::string &msg) {
    throw std::runtime_error(msg);
  }
};
} // namespace UTUI
