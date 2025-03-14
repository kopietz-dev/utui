#include <string>

#include "../element.h"

namespace UTUI {

class Button : public Element {
 public:
  void setValue(const std::string& v) {
    value = v;
    refresh();
    size.x = Utils::getStringWidth(v);
  }
  std::string getVaue() { return value; }

  void onClick(const std::function<void()>& v) { clickListener.set(v); }

 private:
  EventListener clickListener;
  std::string value;

  void handleLeftClick(const InputEvent& e) override {
    if (e.value == 'M') {
      clickListener.trigger();
    }
  }

  void startedHover(const InputEvent& e) override { draw(); }
  void stoppedHover(const InputEvent& e) override { draw(); }
  void draw() override {
    shared.mainBuffer +=
        ANSI::setColor(hovered ? styles.hover : styles.standard) +
        ANSI::setCursorPosition(absolutePosition()) + value;
  }
  void initFromString(const std::string& v, bool alias) override {
    value = v;
    refresh();
    size.x = Utils::getStringWidth(v);
  }
  using Element::Element;
};
}  // namespace UTUI
