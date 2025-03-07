#include "../element.h"

namespace UTUI {

class Counter : public Element {
 public:
  int value = 0, minValue = 0, maxValue = 999;
  std::string incrementText = " + ", decrementText = " - ";

 private:
  void handleLeftClick(const InputEvent& e) override {
    if (e.value != 'M') return;

    const Vector2 relativePosition = e.position - absolutePosition();
    if (relativePosition.x >= 0 &&
        relativePosition.x < decrementText.length()) {
      if (value > minValue) {
        value--;
        // pushEvent(DECREMENT_VALUE)
      }
    } else if (relativePosition.x >=
               decrementText.length() + std::to_string(value).length() + 4) {
      if (value < maxValue) {
        value++;
        // pushEvent(INCREMENT_VALUE)
      }
    }
    refresh();
  }

  void startedHover(const InputEvent& e) override {
    hovered = true;
    draw();
  }
  void stoppedHover(const InputEvent& e) override {
    hovered = false;
    draw();
  }

  void draw() override {
    const std::string strValue = std::to_string(value);
    size = {(int)(decrementText.length() + incrementText.length() +
                  strValue.length() + 4),
            1};
    shared.mainBuffer += ANSI::setCursorPosition(absolutePosition()) +
                         ANSI::setFgColor(styles.fgColor) +
                         ANSI::setBgColor(styles.bgColor) + decrementText +
                         +"  " + std::to_string(value) + "  " + incrementText;
  }

  using Element::Element;
};
}  // namespace UTUI
