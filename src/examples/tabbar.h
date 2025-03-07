#include "../element.h"

namespace UTUI {
class Tabbar : public Element {
 public:
  struct Option {
    std::string value;
  };
  std::vector<Option> options;
  Color fgColorActive, bgColorActive, fgColorActiveHover, bgColorActiveHover;
  bool isSelected() { return selected >= 0; }
  Option& getSelected() { return options[selected]; }
  int offset = 0;

 private:
  int selected = -1, hovered = -1;

  void handleLeftClick(const InputEvent& e) override {
    int xoffset = 0;
    const Vector2 absPos = absolutePosition();
    for (int i = 0; i < options.size(); i++) {
      Option& option = options[i];
      if (Utils::isInBoundaries({absPos.x + xoffset, absPos.y},
                                {(int)option.value.length(), 1}, e.position)) {
        selected = i;
        pushEvent(Event::SELECTION_CHANGED);
        draw();
        return;
      }

      xoffset += option.value.length() + offset;
    }
  }
  void handleMouseMove(const InputEvent& e) override {
    int xoffset = 0;
    const Vector2 absPos = absolutePosition();
    for (int i = 0; i < options.size(); i++) {
      Option& option = options[i];
      if (Utils::isInBoundaries({absPos.x + xoffset, absPos.y},
                                {(int)option.value.length(), 1}, e.position)) {
        hovered = i;
        draw();
        return;
      }

      xoffset += option.value.length() + offset;
    }
    stoppedHover(e);
  }

  void stoppedHover(const InputEvent& e) override {
    if (hovered != -1) {
      hovered = -1;
      draw();
    }
  }

  void draw() override {
    shared.mainBuffer += ANSI::setCursorPosition(absolutePosition());

    size = {0, 1};
    for (int i = 0; i < options.size(); i++) {
      Option& option = options[i];
      size.x += option.value.length() + offset;
      Color fgColor =
          (i == active) ? (i == hovered ? fgColorActiveHover : fgColorActive)
                        : (i == hovered ? styles.fgColorHover : styles.fgColor);
      Color bgColor =
          (i == active) ? (i == hovered ? bgColorActiveHover : bgColorActive)
                        : (i == hovered ? styles.bgColorHover : styles.bgColor);

      shared.mainBuffer += ANSI::setFgColor(fgColor) +
                           ANSI::setBgColor(bgColor) + options[i].value +
                           ANSI::setBgColor(parentBgColor) +
                           Utils::multiplyString(" ", offset);
    }
  }

  using Element::Element;
};
}  // namespace UTUI
