#pragma once
#include "types.h"

namespace UTUI {
class Utils {
 public:
  static std::string multiplyString(const std::string& str, int times) {
    std::string rstr;
    rstr.reserve(str.length() * times);
    for (int i = 0; i < times; i++) {
      rstr.append(str);
    }

    return rstr;
  }

  static bool isInBoundaries(const Vector2& position, const Vector2& size,
                             const Vector2 cursorPosition) {
    return cursorPosition.x >= position.x &&
           cursorPosition.x < (position.x + size.x) &&
           cursorPosition.y >= position.y &&
           cursorPosition.y < (position.y + size.y);
  }
  static int getStringWidth(const std::string& str) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    const std::wstring wstr = converter.from_bytes(str);
    int width = wcswidth(wstr.c_str(), wstr.size());

    return width;
  }
};
}  // namespace UTUI
