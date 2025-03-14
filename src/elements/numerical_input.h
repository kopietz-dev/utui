#include <string>

#include "../element.h"

namespace UTUI {

class NumericalInput : public Element {
 public:
  Color cursorColor, placeholderColor;
  std::string placeholder;
  bool coded = false, floatingPoint = true, negativeNumbers = false;

  void setSize(int newSize) { size.x = newSize; }
  void clearValue() {
    value.clear();
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
  void pushCharacter(char v) {
    value.insert(value.begin() + cursorPosition, 1, v);
    cursorPosition++;

    changeListener.trigger();
  }
  void handleAlfanumKey(const InputEvent& e) override {
    const int spaceLeft = size.x - value.size() - 1;
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
      case '-':
        if (value.empty()) {
          pushCharacter(e.value);
        }
        break;
      case '.':
        if (spaceLeft > 0 && floatingPoint &&
            value.find('.') == std::string::npos) {
          pushCharacter(e.value);
        }
        break;
      default:
        if (spaceLeft > 0 && e.value >= 48 && e.value <= 57) {
          pushCharacter(e.value);
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

  void draw() override {
    clear();
    if (value.empty()) {
      shared.mainBuffer += ANSI::setFgColor(placeholderColor) +
                           ANSI::setBgColor(styles.standard.bgColor) +
                           ANSI::setCursorPosition(absolutePosition()) +
                           placeholder;
    } else {
      shared.mainBuffer += ANSI::setCursorPosition(absolutePosition()) +
                           ANSI::setColor(styles.selected) +
                           (coded ? std::string(value.length(), '*') : value) +
                           ANSI::cursorDown() +
                           ANSI::cursorLeft(value.length());
    }

    updateCursorPosition();
  }

  using Element::Element;
};

}  // namespace UTUI
