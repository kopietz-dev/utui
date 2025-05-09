#include <string>

#include "../element.h"

namespace UTUI {

class Button : public Element {
public:
  EventListener onClick;
  void setValue(const std::string &v) {
    value = v;
    refresh();
    size = {Utils::getStringWidth(v), 1};
  }
  std::string getValue() { return value; }

private:
  std::string value;

  void handleLeftClick(const InputEvent &e) override {
    if (e.value == 'M') {
      onClick.trigger();
    }
  }

  void startedHover(const InputEvent &e) override { draw(); }
  void stoppedHover(const InputEvent &e) override { draw(); }
  void draw() override {
    shared.mainBuffer +=
        ANSI::setColor(hovered ? styles.hover : styles.standard) +
        ANSI::setCursorPosition(absolutePosition()) + value;
  }
  void initFromString(const std::string &v, bool alias) override {
    value = v;
    refresh();
    size = {Utils::getStringWidth(v), 1};
  }
  using Element::Element;
};
} // namespace UTUI
