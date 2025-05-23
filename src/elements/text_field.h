#include "../element.h"

namespace UTUI {

class TextField : public Element {
public:
  Color cursorColor, placeholderColor;
  std::string placeholder;
  void setSize(const Vector2 &newSize) { size = newSize; }

  void clearValue() {
    value.clear();
    refresh();
  }
  std::vector<std::string> getValue() { return value; }
  void setPlaceholder(const std::string &v) {
    placeholder = v;

    if (scroll == 0 &&
        std::all_of(value.begin(), value.end(),
                    [](const std::string &s) { return s.empty(); })) {
      refresh();
    }
  }

  void onChange(const std::function<void()> &v) { changeListener.set(v); }

  void draw() override {
    const Vector2 absSize = absoluteSize();

    if (value.size() < absSize.y) {
      value.resize(absSize.y);
    }
    clear();
    if (scroll == 0 &&
        std::all_of(value.begin(), value.end(),
                    [](const std::string &s) { return s.empty(); })) {
      shared.mainBuffer += ANSI::setFgColor(placeholderColor) +
                           ANSI::setBgColor(styles.standard.bgColor) +
                           ANSI::setCursorPosition(absolutePosition()) +
                           placeholder;
      return;
    }

    shared.mainBuffer += ANSI::setCursorPosition(absolutePosition()) +
                         ANSI::setColor(styles.standard);

    for (int i = 0; i < absSize.y; i++) {
      if (i + scroll >= value.size())
        return;

      const std::string &line = value[i + scroll];

      shared.mainBuffer +=
          line + ANSI::cursorDown() + ANSI::cursorLeft(line.length());
    }
  }

private:
  std::vector<std::string> value;
  int scroll = 0;
  Vector2 cursorPosition;
  EventListener changeListener;

  void updateCursorPosition() {
    if (!active)
      return;
    shared.cursor.setPosition(absolutePosition() + cursorPosition);
    shared.cursor.setColor(cursorColor);
  }
  void activate(const InputEvent &e) override { shared.cursor.show(); }
  void deactivate(const InputEvent &e) override { shared.cursor.hide(); }

  void handleLeftClick(const InputEvent &e) override {
    if (e.value != 'M')
      return;
    cursorPosition = (e.position - absolutePosition());

    updateCursorPosition();
  }
  void newLine() {
    if (cursorPosition.y + 1 == absoluteSize().y + scroll) {
      scroll++;
    }
    value.insert(value.begin() + cursorPosition.y + 1,
                 value[cursorPosition.y].substr(cursorPosition.x));

    value[cursorPosition.y] =
        value[cursorPosition.y].substr(0, cursorPosition.x);

    cursorPosition.x = 0;
    cursorPosition.y++;
  }
  void handleAlfanumKey(const InputEvent &e) override {
    std::string &currentLine = value[cursorPosition.y];
    switch (e.value) {
    case 127:
      if (cursorPosition.x > 0) {
        cursorPosition.x--;
        currentLine.erase(currentLine.begin() + cursorPosition.x);
      } else if (cursorPosition.y > 0) {
        if (scroll > 0)
          scroll--;
        cursorPosition.y--;
        cursorPosition.x = value[cursorPosition.y].length();
        value[cursorPosition.y] += value[cursorPosition.y + 1];
        value.erase(value.begin() + cursorPosition.y + 1);
      } else {
        return;
      }
      clear();
      break;
    default:
      if (value[cursorPosition.y].length() < absoluteSize().x - 1) {
        value[cursorPosition.y].insert(value[cursorPosition.y].begin() +
                                           cursorPosition.x,
                                       1, (char)e.value);

        cursorPosition.x++;

      } else if (cursorPosition.y + 1 < value.size()) {
        newLine();

        cursorPosition.x++;
        value[cursorPosition.y] += (char)e.value;
      } else {
        return;
      }
      break;
    case '\n':
      newLine();
      break;
    }
    changeListener.trigger();

    draw();
    updateCursorPosition();
  }
  void handleSpecialKey(const InputEvent &e) override {
    const Vector2 absSize = absoluteSize();
    switch (e.value) {
    case 'C':
      if (cursorPosition.x < value[cursorPosition.y].length()) {
        cursorPosition.x++;
      } else if (cursorPosition.y + 1 < value.size()) {
        cursorPosition.x = 0;
        cursorPosition.y++;
      }
      break;
    case 'B':
      if (cursorPosition.y == absSize.y + scroll - 1 &&
          absSize.y + scroll < value.size()) {
        scroll++;
        refresh();
      }
      if (cursorPosition.y + 1 < value.size())
        cursorPosition.y++;
      break;
    case 'D':
      if (cursorPosition.x > 0) {
        cursorPosition.x--;
      } else if (cursorPosition.y > 0) {
        cursorPosition.x = value[cursorPosition.y - 1].length();
        cursorPosition.y--;
      }
      break;
    case 'A':
      if (cursorPosition.y == scroll && scroll > 0) {
        scroll--;
        refresh();
      }
      if (cursorPosition.y > 0)
        cursorPosition.y--;
      break;
    }

    updateCursorPosition();
  }

  void initFromString(const std::string &v, bool alias) override {
    placeholder = v;
  }
  using Element::Element;
};

} // namespace UTUI
