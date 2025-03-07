#pragma once
#include "utils.h"

namespace UTUI {
class ANSI {
 public:
  static std::string setCursorPosition(const Vector2& position) {
    char buffer[bufferSize];
    std::snprintf(buffer, bufferSize, "\033[%d;%dH", position.y, position.x);
    return std::string(buffer);
  }
  static std::string setFgColor(const Color& color) {
    char buffer[bufferSize];
    std::snprintf(buffer, bufferSize, "\033[38;2;%d;%d;%dm", color.r, color.g,
                  color.b);
    return std::string(buffer);
  }
  static std::string setBgColor(const Color& color) {
    char buffer[bufferSize];
    std::snprintf(buffer, bufferSize, "\033[48;2;%d;%d;%dm", color.r, color.g,
                  color.b);
    return std::string(buffer);
  }
  static std::string showCursor() { return "\033[?25h"; }
  static std::string hideCursor() { return "\033[?25l"; }
  static std::string disableScroll() { return "\033[?1049h"; }
  static std::string clearScreen() { return "\033[H\033[J"; }
  static std::string setCursorColor(const Color& color) {
    char buffer[bufferSize];
    std::snprintf(buffer, bufferSize, "\033]12;#%02X%02X%02X\a", color.r,
                  color.g, color.b);
    return std::string(buffer);
  }

  static std::string cursorUp(int v = 1) {
    if (v == 0) return "";
    return "\033[" + std::to_string(v) + "A";
  }
  static std::string cursorDown(int v = 1) {
    if (v == 0) return "";
    return "\033[" + std::to_string(v) + "B";
  }
  static std::string cursorRight(int v = 1) {
    if (v == 0) return "";
    return "\033[" + std::to_string(v) + "C";
  }
  static std::string cursorLeft(int v = 1) {
    if (v == 0) return "";
    return "\033[" + std::to_string(v) + "D";
  }
  static std::string clearArea(const Color& bgColor, const Vector2& position,
                               const Vector2& size) {
    return setBgColor(bgColor) + setCursorPosition(position) +
           Utils::multiplyString(Utils::multiplyString(" ", size.x) +
                                     cursorDown() + cursorLeft(size.x),
                                 size.y);
  }

 private:
  static inline const int bufferSize = 20;
};
}  // namespace UTUI
