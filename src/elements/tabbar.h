#include <string>

#include "../element.h"

namespace UTUI {
class Tabbar : public Element {
 public:
  std::vector<std::string> options;
  int getSelected() { return selected; }
  int offset = 1;

  void onChange(const std::function<void()>& v) { changeListener.set(v); }

 private:
  int selected = 0, hovered = -1;
  EventListener changeListener;
  void handleLeftClick(const InputEvent& e) override {
    int xoffset = 0;
    const Vector2 absPos = absolutePosition();
    for (int i = 0; i < options.size(); i++) {
      std::string& option = options[i];
      if (Utils::isInBoundaries({absPos.x + xoffset, absPos.y},
                                {(int)option.length(), 1}, e.position)) {
        selected = i;
        changeListener.trigger();
        draw();
        return;
      }

      xoffset += option.length() + offset;
    }
  }
  void handleMouseMove(const InputEvent& e) override {
    int xoffset = 0;
    const Vector2 absPos = absolutePosition();
    for (int i = 0; i < options.size(); i++) {
      std::string& option = options[i];
      if (Utils::isInBoundaries({absPos.x + xoffset, absPos.y},
                                {(int)option.length(), 1}, e.position)) {
        hovered = i;
        draw();
        return;
      }

      xoffset += option.length() + offset;
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
      std::string& option = options[i];
      size.x += option.length() + offset;

      ColorPair pair = (i == selected)
                           ? (styles.selected)
                           : (i == hovered ? styles.hover : styles.standard);

      shared.mainBuffer += ANSI::setColor(pair) + option +
                           ANSI::setBgColor(parent->styles.standard.bgColor) +
                           Utils::multiplyString(" ", offset);
    }
  }
  void initFromString(const std::string& v, bool alias) override {
    const std::vector<std::string> newOptions = Utils::splitString(v, ',');
    options.insert(options.end(), newOptions.begin(), newOptions.end());
  }
  using Element::Element;
};
}  // namespace UTUI
