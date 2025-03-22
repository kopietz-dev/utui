#include <string>

#include "../element.h"

namespace UTUI {

class TextInput : public Element {
 public:
  Color cursorColor, placeholderColor;
  std::string placeholder;
  bool coded = false;

  void setSize(int newSize) { size.x = newSize; }
  void clearValue() {
    value.clear();
    cursorPosition = 0;
    refresh();
  }
  std::string getValue() { return value; }
  void setPlaceholder(const std::string& v) {
    placeholder = v;

    if (value.empty()) {
      refresh();
    }
  }
  void onChange(const std::function<void()>& v) { changeListener.set(v); }
  void onSubmit(const std::function<void()>& v) { submitListener.set(v); }

  void draw() override {
    if (value.empty()) {
      shared.mainBuffer +=
          ANSI::setFgColor(placeholderColor) +
          ANSI::setBgColor(styles.standard.bgColor) +
          ANSI::setCursorPosition(absolutePosition()) + placeholder +
          Utils::multiplyString(" ", absoluteSize().x - placeholder.size());
    } else {
      shared.mainBuffer +=
          ANSI::setCursorPosition(absolutePosition()) +
          ANSI::setColor(styles.standard) +
          (coded ? std::string(value.length(), '*') : value) +
          Utils::multiplyString(" ", absoluteSize().x - value.length());
    }

    updateCursorPosition();
  }

 private:
  std::string value;
  int cursorPosition = 0;

  EventListener changeListener, submitListener;

  void updateCursorPosition() {
    shared.cursor.setPosition(absolutePosition() +
                              Vector2({cursorPosition, 0}));
    shared.cursor.setColor(cursorColor);
  }
  void activate(const InputEvent& e) override { shared.cursor.show(); }
  void deactivate(const InputEvent& e) override { shared.cursor.hide(); }

  void handleLeftClick(const InputEvent& e) override {
    if (e.value != 'M') return;

    cursorPosition = (e.position.x - absolutePosition().x);

    if (cursorPosition > value.length()) {
      cursorPosition = value.length();
    }

    updateCursorPosition();
  }
  void handleAlfanumKey(const InputEvent& e) override {
    switch ((char)e.value) {
      case 127:
        if (cursorPosition > 0) {
          cursorPosition--;
          value.erase(value.begin() + cursorPosition);

          changeListener.trigger();
        }
        break;
      case '\n':
        submitListener.trigger();
        break;
      default:
        if (value.length() < absoluteSize().x - 1) {
          value.insert(value.begin() + cursorPosition, 1, (char)e.value);

          cursorPosition++;

          changeListener.trigger();
        }
        break;
    }

    draw();
  }
  void handleSpecialKey(const InputEvent& e) override {
    switch ((char)e.value) {
      case 'C':
        if (cursorPosition < value.length()) {
          cursorPosition++;
        }
        break;
      case 'D':
        if (cursorPosition > 0) {
          cursorPosition--;
        }
        break;
    }

    updateCursorPosition();
  }
  void handleResize() override {
    const Vector2 absSize = absoluteSize();

    if (absSize.x < value.length()) {
      value.resize(absSize.x - 1);

      cursorPosition = value.length();
    }
  }
  void initFromString(const std::string& v, bool alias) override {
    placeholder = v;
  }
  using Element::Element;
};

}  // namespace UTUI
