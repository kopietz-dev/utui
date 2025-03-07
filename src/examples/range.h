#include "../element.h"

namespace UTUI {

class Range : public Element {
 public:
  int value = 0;

  void setSize(const Vector2& newSize) { size = newSize; }

 private:
  bool isDragged = false;

  void handleLeftClick(const InputEvent& e) override {
    const Vector2 absPos = absolutePosition();
    isDragged = Utils::isInBoundaries({absPos.x + value, absPos.y}, {1, 1},
                                      e.position) &&
                e.value == 'M';

    if (isDragged) {
      pushEvent(Event::ACTIVATED);
    }
  }
  void handleLeftDrag(const InputEvent& e) override {
    if (!isDragged) {
      return;
    }

    int newValue =
        std::clamp(e.position.x - absolutePosition().x, 0, size.x - 1);

    if (value != newValue) {
      pushEvent(Event::VALUE_CHANGE);

      value = newValue;
      draw();
    }
  }
  void draw() override {
    shared.mainBuffer += ANSI::setFgColor(styles.fgColor) +
                         ANSI::setBgColor(styles.bgColor) +
                         ANSI::setCursorPosition(absolutePosition()) +
                         Utils::multiplyString("\u2500", value) + "\u25CF" +
                         Utils::multiplyString("\u2500", size.x - value - 1);
  }
  using Element::Element;
};

}  // namespace UTUI
