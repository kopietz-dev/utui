#pragma once

#include "ansi.h"

namespace UTUI {

struct Styles {
  Color fgColor = {255, 255, 255}, bgColor = {0, 0, 0}, fgColorHover = bgColor,
        bgColorHover = fgColor, fgColorSelected = fgColor,
        bgColorSelected = bgColor, fgColorSelectedHover = fgColorHover,
        bgColorSelectedHover = bgColorHover;
};

class Element {
 public:
  Vector2 position;
  Styles styles;
  bool disabled = false;

  void remove() {
    disabled = true;
    sRemove = true;
  }
  void refresh() {
    cleanup();
    draw();
  }
  void cleanup() {
    shared.mainBuffer +=
        ANSI::clearArea(parentBgColor, absolutePosition(), size);
  }
  void clear() {
    shared.mainBuffer +=
        ANSI::clearArea(styles.bgColor, absolutePosition(), size);
  }
  void pushEvent(unsigned int type) { shared.events.push_back({id, type}); }
  bool isActive() { return active; }
  void setID(unsigned int newID) { id = newID; };
  unsigned int getID() { return id; }
  const Vector2 absolutePosition() { return (position + parentPosition); }
  const Vector2 getSize() { return size; }
  virtual ~Element() = default;

 protected:
  SharedValues& shared;
  Vector2& parentPosition;
  Color& parentBgColor;
  bool active = false, hovered = false;
  unsigned int id;
  Vector2 size;
  bool sRemove = false;

  Element(SharedValues& shared, Vector2& parentPosition, Color& parentBgColor,
          unsigned int id)
      : shared(shared),
        parentPosition(parentPosition),
        parentBgColor(parentBgColor),
        id(id){};

  virtual void startedHover(const InputEvent& e) {}
  virtual void stoppedHover(const InputEvent& e) {}

  virtual void activate(const InputEvent& e) {}
  virtual void deactivate(const InputEvent& e) {}

  virtual void handleLeftClick(const InputEvent& e) {}
  virtual void handleMouseMove(const InputEvent& e) {}
  virtual void handleRightClick(const InputEvent& e) {}
  virtual void handleLeftDrag(const InputEvent& e) {}

  virtual void handleScrollUp(const InputEvent& e) {}
  virtual void handleScrollDown(const InputEvent& e) {}

  virtual void handleAlfanumKey(const InputEvent& e) {}
  virtual void handleSpecialKey(const InputEvent& e) {}

  virtual void draw() {}

  bool handleInputEvent(const InputEvent& e) {
    if (disabled || (e.type < 90 && !Utils::isInBoundaries(absolutePosition(),
                                                           size, e.position)))
      return false;

    switch ((InputEventType)e.type) {
      case InputEventType::MOUSE_LEFT_CLICK:
        handleLeftClick(e);
        break;
      case InputEventType::MOUSE_MOVE:
        handleMouseMove(e);
        break;
      case InputEventType::MOUSE_RIGHT_CLICK:
        handleRightClick(e);
        break;
      case InputEventType::MOUSE_DRAG_LEFT:
        handleLeftDrag(e);
        break;
      case InputEventType::SCROLL_UP:
        handleScrollUp(e);
        break;
      case InputEventType::SCROLL_DOWN:
        handleScrollDown(e);
        break;
      case InputEventType::ALFANUMERICAL_KEY:
        handleAlfanumKey(e);
        break;
      case InputEventType::SPECIAL_KEY:
        handleSpecialKey(e);
        break;
    }
    return true;
  }

  friend class Window;
  friend class Main;
};

}  // namespace UTUI
