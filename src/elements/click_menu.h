#include <functional>
#include <string>

#include "../element.h"

namespace UTUI {

class ClickMenu : public Element {
 public:
  std::vector<std::string> options;

  int getSelected() { return selected; }
  void setSelected(int index) {
    if (index >= 0 && index < options.size()) {
      selected = index;
    }
    refresh();
  }
  void onChange(const std::function<void()>& v) { changeListener.set(v); }

 private:
  int selected = 0;
  bool hovered = false;

  EventListener changeListener;

  void handleLeftClick(const InputEvent& e) override {
    if (Utils::isInBoundaries(absolutePosition(), size, e.position) &&
        e.value == 'M') {
      selected++;

      if (selected >= options.size()) {
        selected = 0;
      }
      changeListener.trigger();
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
    const std::string displayedText = options[selected];
    size = {Utils::getStringWidth(displayedText), 1};

    shared.mainBuffer +=
        ANSI::setColor(hovered ? styles.hover : styles.standard) +
        ANSI::setCursorPosition(absolutePosition()) + displayedText;
  }
  void initFromString(const std::string& v, bool alias) override {
    const std::vector<std::string> newOptions = Utils::splitString(v, ',');
    options.insert(options.end(), newOptions.begin(), newOptions.end());
  }
  using Element::Element;
};

}  // namespace UTUI
