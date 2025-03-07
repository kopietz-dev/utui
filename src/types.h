#pragma once

#include <sys/ioctl.h>
#include <termios.h>

#include <algorithm>
#include <codecvt>
#include <csignal>
#include <cstring>
#include <fstream>
#include <locale>
#include <string>
#include <vector>

#include "events.h"

namespace UTUI {
class UTUI;
class Window;

enum class InputEventType {
  NO_EVENT = -1,
  MOUSE_LEFT_CLICK = 0,
  MOUSE_MIDDLE_CLICK = 1,
  MOUSE_RIGHT_CLICK = 2,
  MOUSE_MOVE = 35,
  MOUSE_DRAG_LEFT = 32,
  SCROLL_UP = 64,
  SCROLL_DOWN = 65,
  ALFANUMERICAL_KEY = 90,
  SPECIAL_KEY = 92,
  UPDATE_VALUES = 93,
  DRAW = 94,
};

class Flag {
 public:
  void set() { value = true; }
  bool get() {
    if (value) {
      value = false;
      return true;
    } else
      return false;
  }

 private:
  bool value = false;
};
struct Vector2 {
  int x = 1, y = 1;

  Vector2 operator+(const Vector2& v) const { return {x + v.x, y + v.y}; }
  Vector2 operator-(const Vector2& v) const { return {x - v.x, y - v.y}; }

  Vector2 operator+(const int& v) const { return {x + v, y + v}; }
  Vector2 operator-(const int& v) const { return {x - v, y - v}; }

  Vector2 operator/(const int& v) const { return {x / v, y / v}; }
  bool operator==(const Vector2& v) const { return (x == v.x && y == v.y); }
};

struct InputEvent {
  int type, value = 0;
  Vector2 position;
};

struct Color {
  int r = 255, g = 255, b = 255;
  Color operator+(const Color& v) { return {r + v.r, g + v.g, b + v.b}; }
  Color operator-(const Color& v) { return {r - v.r, g - v.g, b - v.b}; }
  Color operator+(const int& v) { return {r + v, g + v, b + v}; }
  Color operator-(const int& v) { return {r - v, g - v, b - v}; }

  Color operator!() const { return {255 - r, 255 - g, 255 - b}; }
};
struct SharedValues {
  std::string mainBuffer;
  std::string cursorBuffer;
  Vector2 screenSize;
  Color bgColor;
  std::vector<Event> events;
};
bool operator==(int l, InputEventType r) { return l == static_cast<int>(r); }
bool operator==(InputEventType l, int r) { return r == l; }
bool operator!=(InputEventType l, int r) { return !(r == l); }
bool operator!=(int l, InputEventType r) { return !(r == l); }
}  // namespace UTUI
