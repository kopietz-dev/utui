#include "../element.h"

namespace UTUI {

class Switch : public Element {
 public:
  Color fgColorOn, fgColorHoverOn, bgColorHoverOn, bgColorOn;
  std::string textOff, textOn;
  bool value = 0;

 private:
  bool hovered;

  void handleLeftClick(const InputEvent& e) override {
    if (e.value == 'm') {
      value = !value;
      pushEvent(Event::VALUE_CHANGE);
    }
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
    const std::string displayedText = (value ? textOn : textOff);
    size = {(int)displayedText.length(), 1};
    shared.mainBuffer +=
        ANSI::setFgColor(hovered ? styles.fgColorHover : styles.fgColor) +
        ANSI::setBgColor(hovered ? styles.bgColorHover : styles.bgColor) +
        ANSI::setCursorPosition(absolutePosition()) + displayedText;
  }
  using Element::Element;
};

}  // namespace UTUI
