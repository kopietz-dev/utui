#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "ansi.h"
#include "event_listener.h"
#include "types.h"
#include "utils.h"

namespace UTUI {

struct Styles {
  ColorPair standard;
  ColorPair hover;
  ColorPair selected;
};

class Element {
 public:
  Styles styles;
  bool disabled = false;

  void remove() {
    disabled = true;
    if (sRemoveFlag != nullptr) {
      sRemoveFlag->set();
    }
    sRemove = true;
  }
  void clear() {
    shared.mainBuffer += ANSI::clearArea(parent->styles.standard.bgColor,
                                         absolutePosition(), size);
  }
  void disable() {
    disabled = true;
    clear();
  }
  void enable() {
    disabled = false;
    draw();
  }

  void setPosition(const Vector2& v) {
    clear();
    position = v;
    draw();
  }
  void refresh() {
    clear();
    draw();
  }
  void updateStyles() { refresh(); }
  void setRelativeOffset(const Vector2& v) {
    clear();
    position = v;
    draw();
  }
  void setID(unsigned int newID) { id = newID; };
  Vector2 absolutePosition() const {
    return position +
           ((parent != nullptr)
                ? parent->absolutePosition() +
                      Vector2(
                          {(int)std::round((float)relativeOffset.x / 100.0f *
                                           (float)(parent->size.x - 1)),
                           (int)std::round((float)relativeOffset.y / 100.0f *
                                           (float)(parent->size.y - 1))})
                : Vector2());
  }

  unsigned int getID() const { return id; }
  Vector2 getSize() const { return size; }
  Vector2 getPosition() const { return position; }
  Vector2 getRelativeOffset() const { return relativeOffset; }
  bool isActive() const { return active; }

  virtual ~Element() = default;

 protected:
  SharedValues& shared;
  const Element* parent;
  Flag* sRemoveFlag;
  bool active = false, hovered = false;
  unsigned int id;
  Vector2 size = {1, 1};
  bool sRemove = false;
  Vector2 position, relativeOffset;

  Element(SharedValues& shared, const Element* parent, Flag* sRemoveFlag,
          unsigned int id)
      : shared(shared), parent(parent), sRemoveFlag(sRemoveFlag), id(id){};

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

  virtual void initFromString(const std::string& v, bool alias) {}

  bool handleInputEvent(const InputEvent& e) {
    if (disabled ||
        ((e.type < 90 && e.type != InputEventType::MOUSE_DRAG_LEFT) &&
         !Utils::isInBoundaries(absolutePosition(), size, e.position)))
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
