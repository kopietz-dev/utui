#include "../element.h"

namespace UTUI {

class Button : public Element {
 public:
  void setText(const std::string& v) {
    text = v;
    refresh();
    size.x = Utils::getStringWidth(v);
  }
  std::string getText() { return text; }
  bool isHovered() { return hover; }

 private:
  std::string text;
  bool hover = false;

  void handleLeftClick(const InputEvent& e) override {
    if (e.value == 'M') {
      pushEvent(Event::BUTTON_CLICK);
    }
  }

  void startedHover(const InputEvent& e) override {
    hover = true;
    draw();
  }
  void stoppedHover(const InputEvent& e) override {
    hover = false;
    draw();
  }

  void draw() override {
    shared.mainBuffer +=
        ANSI::setFgColor(hover ? styles.fgColorHover : styles.fgColor) +
        ANSI::setBgColor(hover ? styles.bgColorHover : styles.bgColor) +
        ANSI::setCursorPosition(absolutePosition()) + text;
  }

  using Element::Element;
};
}  // namespace UTUI
