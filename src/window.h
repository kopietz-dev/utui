#pragma once

#include "element.h"
#include "types.h"

namespace UTUI {

class Window : public Element {
public:
  bool borderVisibility = true;

  template <typename T> T &append(unsigned int id = 0) {
    static_assert(std::is_base_of<Element, T>::value,
                  "T must be a class derived from UTUI::Element");
    elements.push_back(new T(shared, this, sRemoveFlag, id));

    return *dynamic_cast<T *>(elements.back());
  }

  std::vector<Element *> getElementsById(unsigned int id) {
    std::vector<Element *> matching;
    for (Element *element : elements) {
      if (element->getID() == id) {
        matching.push_back(element);
      }
    }

    return matching;
  }

  template <typename T> T *getElementById(unsigned int id) {
    static_assert(std::is_base_of<Element, T>::value,
                  "T must be a class derived from UTUI::Element");

    for (Element *element : elements) {
      if (element->getID() == id) {
        return dynamic_cast<T *>(element);
      }
    }

    return nullptr;
  }

  void refresh() {
    clear();

    draw();

    for (Element *element : elements) {
      element->draw();
    }
  }
  void remove() {
    disabled = true;
    sRemove = true;
  }

  ~Window() {
    for (Element *element : elements) {
      delete element;
    }
  }

private:
  std::vector<Element *> elements;
  int hoveredElement = -1, activeElement = -1;
  Flag elementRemoveFlag;

  bool update(const InputEvent &e) {
    // check if window is enabled
    if (disabled)
      return false;

    if (elementRemoveFlag.get()) {
      for (int i = 0; i < elements.size(); i++) {
        if (elements[i]->sRemove) {
          delete elements[i];
          elements.erase(elements.begin() + i);

          if (hoveredElement == i)
            hoveredElement = -1;
          if (activeElement == i)
            activeElement = -1;

          if (hoveredElement > i)
            hoveredElement--;
          if (activeElement > i)
            activeElement--;

          i--;
        }
      }
    }

    if (activeElement != -1 &&
        (e.type >= 90 || e.type == InputEventType::MOUSE_DRAG_LEFT ||
         e.type == InputEventType::MOUSE_LEFT_CLICK)) {
      if (elements[activeElement]->handleInputEvent(e)) {
        return true;
      } else {
        elements[activeElement]->active = false;
        elements[activeElement]->deactivate(e);
        elements[activeElement]->refresh();
        activeElement = -1;
      }
    }

    if (hoveredElement != -1 && e.type >= 0 && e.type <= 65) {
      if (elements[hoveredElement]->handleInputEvent(e)) {
        if (e.type == InputEventType::MOUSE_LEFT_CLICK && e.value == 'M') {
          activeElement = hoveredElement;
          elements[activeElement]->active = true;
          elements[activeElement]->activate(e);
        }
        return true;
      } else {
        elements[hoveredElement]->hovered = false;
        elements[hoveredElement]->stoppedHover(e);
        hoveredElement = -1;
      }
    }

    if (!Utils::isInBoundaries(absolutePosition(), absoluteSize(),
                               e.position)) {
      return false;
    }

    if (e.type == InputEventType::MOUSE_MOVE) {
      for (int i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        if (Utils::isInBoundaries(element->absolutePosition(),
                                  element->absoluteSize(), e.position)) {
          hoveredElement = i;
          elements[hoveredElement]->hovered = true;
          elements[hoveredElement]->startedHover(e);
          elements[hoveredElement]->handleMouseMove(e);
          return true;
        }
      }
    }
    return false;
  }
  void handleResize() override {
    for (Element *element : elements) {
      element->handleResize();
    }
  }
  void draw() override {
    const Vector2 absSize = absoluteSize();

    if (absSize.x < 3 || absSize.y < 3)
      return;

    if (borderVisibility) {
      shared.mainBuffer +=
          // Setting colors and position
          ANSI::setColor(styles.standard) +
          ANSI::setCursorPosition(absolutePosition()) +
          // Drawing top border
          "\u256D" + Utils::multiplyString("\u2500", absSize.x - 2) + "\u256E" +
          // Drawing middle section
          Utils::multiplyString(
              ANSI::cursorDown() + ANSI::cursorLeft(absSize.x) + "\u2502" +
                  Utils::multiplyString(" ", absSize.x - 2) + "\u2502",
              absSize.y - 2) +
          // Drawing bottom border
          ANSI::cursorDown() + ANSI::cursorLeft(absSize.x) + "\u2570" +
          Utils::multiplyString("\u2500", absSize.x - 2) + "\u256F";
    } else {
      shared.mainBuffer +=
          ANSI::setColor(styles.standard) +
          ANSI::setCursorPosition(absolutePosition()) +
          Utils::multiplyString((Utils::multiplyString(" ", absSize.x) +
                                 ANSI::cursorLeft(absSize.x) +
                                 ANSI::cursorDown(1)),
                                absSize.y);
    }
  }
  friend class Main;
  using Element::Element;
};
} // namespace UTUI
