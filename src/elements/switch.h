#include <string>
#include <vector>

#include "../element.h"

namespace UTUI {

class Switch : public Element {
 public:
  std::string textOff, textOn;
  bool value = 0;

  void onChange(const std::function<void()>& v) { changeListener.set(v); }

 private:
  EventListener changeListener;

  void handleLeftClick(const InputEvent& e) override {
    if (e.value == 'm') {
      value = !value;
      changeListener.trigger();
    }
  }

  void startedHover(const InputEvent& e) override { draw(); }
  void stoppedHover(const InputEvent& e) override { draw(); }

  void draw() override {
    const std::string displayedText = (value ? textOn : textOff);
    size = {(int)displayedText.length(), 1};
    shared.mainBuffer +=
        ANSI::setColor(hovered ? styles.hover : styles.standard) +
        ANSI::setCursorPosition(absolutePosition()) + displayedText;
  }

  void initFromString(const std::string& v, bool alias) override {
    const std::vector<std::string> texts = Utils::splitString(v, ',');
    textOff = texts[0];
    textOn = texts[1];
  }

  using Element::Element;
};

}  // namespace UTUI
