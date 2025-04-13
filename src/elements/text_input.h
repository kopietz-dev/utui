#include <string>

#include "../element.h"

namespace UTUI {

class TextInput : public Element {
public:
  std::string placeholder;
  EventListener onChange, onSubmit;
  bool coded = false;

  void setSize(int newSize) { size.x = newSize; }
  void clearValue() {
    value.clear();
    cursorPosition = 0;
    refresh();
  }
  std::string getValue() { return value; }
  void setPlaceholder(const std::string &v) {
    placeholder = v;

    if (value.empty()) {
      refresh();
    }
  }
  void draw() override {
    shared.mainBuffer += ANSI::setCursorPosition(absolutePosition());

    if (active) {
      shared.mainBuffer += ANSI::setBgColor(styles.selected.bgColor);
    } else {
      shared.mainBuffer += ANSI::setBgColor(styles.standard.bgColor);
    }

    if (value.empty()) {
      shared.mainBuffer +=
          ANSI::setFgColor(styles.standard.fgColor) + placeholder +
          Utils::multiplyString(" ", absoluteSize().x - placeholder.size());
    } else {
      shared.mainBuffer +=
          ANSI::setFgColor(styles.selected.fgColor) +
          (coded ? std::string(value.length(), '*') : value) +
          Utils::multiplyString(" ", absoluteSize().x - value.length());
    }

    updateCursorPosition();
  }

private:
  std::string value;
  int cursorPosition = 0;

  void updateCursorPosition() {
    shared.cursor.setPosition(absolutePosition() +
                              Vector2({cursorPosition, 0}));
    shared.cursor.setColor(styles.selected.fgColor);
  }
  void activate(const InputEvent &e) override {
    shared.cursor.show();
    draw();
  }
  void deactivate(const InputEvent &e) override {
    shared.cursor.hide();
    draw();
  }

  void handleLeftClick(const InputEvent &e) override {
    if (e.value != 'M')
      return;

    cursorPosition = (e.position.x - absolutePosition().x);

    if (cursorPosition > value.length()) {
      cursorPosition = value.length();
    }

    updateCursorPosition();
  }
  void handleAlfanumKey(const InputEvent &e) override {
    switch ((char)e.value) {
    case 127:
      if (cursorPosition > 0) {
        cursorPosition--;
        value.erase(value.begin() + cursorPosition);

        onChange.trigger();
      }
      break;
    case '\n':
      onSubmit.trigger();
      break;
    default:
      if (value.length() < absoluteSize().x - 1) {
        value.insert(value.begin() + cursorPosition, 1, (char)e.value);

        cursorPosition++;

        onChange.trigger();
      }
      break;
    }

    draw();
  }
  void handleSpecialKey(const InputEvent &e) override {
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
  void initFromString(const std::string &v, bool alias) override {
    placeholder = v;
  }
  using Element::Element;
};

} // namespace UTUI
