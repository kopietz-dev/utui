#pragma once

#include "element.h"
#include "types.h"

namespace UTUI {

class Window : public Element {
 public:
  void setSize(const Vector2& newSize) { size = newSize; }
  template <typename T>
  T& append(unsigned int id = 0) {
    static_assert(std::is_base_of<Element, T>::value,
                  "T must be a class derived from UTUI::Element");
    elements.push_back(new T(shared, this, sRemoveFlag, id));

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

  template <typename T>
  T* getElementById(unsigned int id) {
    static_assert(std::is_base_of<Element, T>::value,
                  "T must be a class derived from UTUI::Element");

    for (Element* element : elements) {
      if (element->getID() == id) {
        return dynamic_cast<T*>(element);
      }
    }

    return nullptr;
  }
  void clear() {
    shared.mainBuffer +=
        ANSI::clearArea(styles.standard.bgColor, position + 1, size - 2);
  }

  void refresh() {
    clear();

    draw();

    for (Element* element : elements) {
      element->draw();
    }
  }
  void remove() {
    disabled = true;
    sRemove = true;
  }

  ~Window() {
    for (Element* element : elements) {
      delete element;
    }
  }

 private:
  std::vector<Element*> elements;

  int hoveredElement = -1, activeElement = -1;
  Flag elementRemoveFlag;

  bool update(const InputEvent& e) {
    // check if window is enabled
    if (disabled) return false;

    if (elementRemoveFlag.get()) {
      for (int i = 0; i < elements.size(); i++) {
        if (elements[i]->sRemove) {
          delete elements[i];
          elements.erase(elements.begin() + i);

          if (hoveredElement == i) hoveredElement = -1;
          if (activeElement == i) activeElement = -1;

          if (hoveredElement > i) hoveredElement--;
          if (activeElement > i) activeElement--;

          i--;
        }
      }
    }

    if (activeElement != -1 && (e.type >= 90 || e.type == 0)) {
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

    if (!Utils::isInBoundaries(absolutePosition(), size, e.position)) {
      return false;
    }

    if (e.type == InputEventType::MOUSE_MOVE) {
      for (int i = 0; i < elements.size(); i++) {
        Element* element = elements[i];
        if (Utils::isInBoundaries(element->absolutePosition(), element->size,
                                  e.position)) {
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

  void draw() {
    shared.mainBuffer +=
        // Setting colors and position
        ANSI::setColor(styles.standard) +
        ANSI::setCursorPosition(absolutePosition()) +
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
  using Element::Element;
};
}  // namespace UTUI
