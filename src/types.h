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

namespace UTUI {
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
  bool peek() { return value; }

private:
  bool value = false;
};
struct Vector2 {
  int x = 0, y = 0;

  Vector2 operator+(const Vector2 &v) const { return {x + v.x, y + v.y}; }
  Vector2 operator-(const Vector2 &v) const { return {x - v.x, y - v.y}; }

  Vector2 operator+(const int &v) const { return {x + v, y + v}; }
  Vector2 operator-(const int &v) const { return {x - v, y - v}; }

  Vector2 operator/(const int &v) const { return {x / v, y / v}; }
  bool operator==(const Vector2 &v) const { return (x == v.x && y == v.y); }
};

struct InputEvent {
  int type, value = 0;
  Vector2 position;
};

struct Color {
  int r = 255, g = 255, b = 255;
  Color operator+(const Color &v) { return {r + v.r, g + v.g, b + v.b}; }
  Color operator-(const Color &v) { return {r - v.r, g - v.g, b - v.b}; }
  Color operator+(const int &v) { return {r + v, g + v, b + v}; }
  Color operator-(const int &v) { return {r - v, g - v, b - v}; }

  Color operator!() const { return {255 - r, 255 - g, 255 - b}; }
};
struct ColorPair {
  Color fgColor = {255, 255, 255}, bgColor = {0, 0, 0};
  operator std::string() const {
    char buffer[40];
    std::snprintf(buffer, 40, "\033[38;2;%d;%d;%dm\033[48;2;%d;%d;%dm",
                  fgColor.r, fgColor.g, fgColor.b, bgColor.r, bgColor.g,
                  bgColor.b);
    return std::string(buffer);
  }
};
struct Cursor {
public:
  void setColor(const Color &v) {
    color = v;
    updated.set();
  }
  void setPosition(const Vector2 &v) {
    position = v;
    updated.set();
  }
  void show() {
    visible = true;
    updated.set();
  }
  void hide() {
    if (!updated.peek())
      visible = false;
  }

private:
  Vector2 position = {1, 1};
  Color color = {255, 255, 255};
  bool visible = false;
  Flag updated;

  friend class Main;
};
struct SharedValues {
  std::string mainBuffer;
  Cursor cursor;
};
bool operator==(int l, InputEventType r) { return l == static_cast<int>(r); }
bool operator==(InputEventType l, int r) { return r == l; }
bool operator!=(InputEventType l, int r) { return !(r == l); }
bool operator!=(int l, InputEventType r) { return !(r == l); }
} // namespace UTUI
