#include "../element.h"

namespace UTUI {

class ClickMenu : public Element {
 public:
  std::vector<std::string> options;

  int getSelected() { return value; }
  void setSelected(int index) {
    if (index >= 0 && index < options.size()) {
      value = index;
    }
    refresh();
  }

 private:
  int value = 0;
  bool hovered = false;

  void handleLeftClick(const InputEvent& e) override {
    if (Utils::isInBoundaries(absolutePosition(), size, e.position) &&
        e.value == 'M') {
      value++;
      if (value >= options.size()) value = 0;
      pushEvent(Event::SELECTION_CHANGED);
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
    const std::string displayedText = options[value];
    size = {Utils::getStringWidth(displayedText), 1};

    shared.mainBuffer +=
        ANSI::setFgColor(hovered ? styles.fgColorHover : styles.fgColor) +
        ANSI::setBgColor(hovered ? styles.bgColorHover : styles.bgColor) +
        ANSI::setCursorPosition(absolutePosition()) + displayedText;
  }

  using Element::Element;
};

}  // namespace UTUI
