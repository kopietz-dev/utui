#pragma once

#include "element.h"

namespace UTUI {

class Window {
 public:
  bool disabled = false, sRemove = false;
  Vector2 size, position;
  Styles styles;

  void setSize(const Vector2& newSize) { size = newSize; }
  template <typename T>
  T& append(unsigned int id = 0) {
    static_assert(std::is_base_of<Element, T>::value,
                  "T must be a class derived from UTUI::Element");
    elements.push_back(new T(shared, position, styles.bgColor, id));

    return *dynamic_cast<T*>(elements.back());
  }

  std::vector<Element*> getElementsById(unsigned int id) {
    std::vector<Element*> matching;
    for (Element* element : elements) {
      if (element->getID() == id) {
        matching.push_back(element);
      }
    }
    return matching;
  }

  void clear() {
    shared.mainBuffer += ANSI::clearArea(shared.bgColor, position, size);
  }

  void refresh() {
    clear();
    draw();

    for (Element* element : elements) {
      element->draw();
    }
  }

 private:
  std::vector<Element*> elements;
  SharedValues& shared;
  int id = 0;
  Element *hoveredElement = nullptr, *activeElement = nullptr;
  Window(SharedValues& shared, unsigned int id) : shared(shared), id(id) {}
  void handleInputEvent(const InputEvent& e) { update(e); }
  bool update(const InputEvent& e) {
    // check if window is enabled
    if (disabled) return false;

    if (activeElement != nullptr && (e.type >= 90 || e.type == 0)) {
      if (activeElement->handleInputEvent(e)) {
        return true;
      } else {
        activeElement->active = false;
        activeElement->deactivate(e);
        activeElement->refresh();
        activeElement = nullptr;
      }
    }

    if (hoveredElement != nullptr && e.type >= 0 && e.type <= 65) {
      if (hoveredElement->handleInputEvent(e)) {
        if (e.type == InputEventType::MOUSE_LEFT_CLICK && e.value == 'M') {
          activeElement = hoveredElement;
          activeElement->active = true;
          activeElement->handleLeftClick(e);
          activeElement->activate(e);
        }
        return true;
      } else {
        hoveredElement->hovered = false;
        hoveredElement->stoppedHover(e);
        hoveredElement = nullptr;
      }
    }

    if (!Utils::isInBoundaries(position, size, e.position)) {
      return false;
    }

    if (e.type == InputEventType::MOUSE_MOVE) {
      for (Element* element : elements) {
        if (Utils::isInBoundaries(element->absolutePosition(), element->size,
                                  e.position)) {
          hoveredElement = element;
          hoveredElement->hovered = true;
          hoveredElement->startedHover(e);
          hoveredElement->handleMouseMove(e);
          return true;
        }
      }
    }
    return false;
  }

  void draw() {
    shared.mainBuffer +=
        // Setting colors and position
        ANSI::setFgColor(styles.fgColor) + ANSI::setBgColor(styles.bgColor) +
        ANSI::setCursorPosition(position) +
        // Drawing top border
        "\u256D" + Utils::multiplyString("\u2500", size.x - 2) + "\u256E" +
        // Drawing middle section
        Utils::multiplyString(
            ANSI::cursorDown() + ANSI::cursorLeft(size.x) + "\u2502" +
                Utils::multiplyString(" ", size.x - 2) + "\u2502",
            size.y - 2) +
        // Drawing bottom border
        ANSI::cursorDown() + ANSI::cursorLeft(size.x) + "\u2570" +
        Utils::multiplyString("\u2500", size.x - 2) + "\u256F";
  }

  friend class Main;
};
}  // namespace UTUI
