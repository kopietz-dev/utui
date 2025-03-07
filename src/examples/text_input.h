#include "../element.h"

namespace UTUI {

class TextInput : public Element {
 public:
  Color cursorColor, placeholderColor;
  std::string placeholder;
  bool coded = false, numerical = false;

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

 private:
  std::string value;
  int cursorPosition = 0;

  void updateCursorPosition() {
    shared.cursorBuffer +=
        ANSI::setCursorColor(cursorColor) +
        ANSI::setCursorPosition(absolutePosition() +
                                Vector2({cursorPosition, 0}));
  }
  void activate(const InputEvent& e) override {
    shared.cursorBuffer += ANSI::showCursor();
  }
  void deactivate(const InputEvent& e) override {
    shared.cursorBuffer += ANSI::hideCursor();
  }

  void handleLeftClick(const InputEvent& e) override {
    if (e.value != 'M') return;

    cursorPosition = (e.position.x - absolutePosition().x);

    if (cursorPosition > value.length()) {
      cursorPosition = value.length();
    }

    pushEvent(Event::ACTIVATED);
    updateCursorPosition();
  }
  void handleAlfanumKey(const InputEvent& e) override {
    switch ((char)e.value) {
      case 127:
        if (cursorPosition > 0) {
          cursorPosition--;
          value.erase(value.begin() + cursorPosition);

          pushEvent(Event::VALUE_CHANGE);
        }
        break;
      case '\n':
        pushEvent(Event::SUBMIT);
        break;
      default:
        if (value.length() < size.x - 1) {
          if (numerical && (e.value < 48 || e.value > 57)) return;

          value.insert(value.begin() + cursorPosition, 1, (char)e.value);
          cursorPosition++;

          pushEvent(Event::VALUE_CHANGE);
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
  }

  void draw() override {
    clear();
    if (value.empty()) {
      shared.mainBuffer += ANSI::setFgColor(placeholderColor) +
                           ANSI::setBgColor(styles.bgColor) +
                           ANSI::setCursorPosition(absolutePosition()) +
                           placeholder;
    } else {
      shared.mainBuffer +=
          ANSI::setCursorPosition(absolutePosition()) +
          ANSI::setFgColor(styles.fgColor) + ANSI::setBgColor(styles.bgColor) +
          (coded ? std::string(value.length(), '*') : value) +
          ANSI::cursorDown() + ANSI::cursorLeft(value.length());
    }

    updateCursorPosition();
  }

  using Element::Element;
};

}  // namespace UTUI
