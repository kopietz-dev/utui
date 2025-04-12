#include <math.h>

#include <algorithm>
#include <cmath>
#include <fstream>

#include "../element.h"

namespace UTUI {
class ScrollableMenu : public Element {
public:
  void setSize(int newSize) { size.y = newSize; }

  void setScroll(int newScroll) {
    scroll = std::clamp(newScroll, 0, (int)(value.size() - absoluteSize().y));
  }
  int getScroll() const { return scroll; }

  int getOptionsSize() { return value.size(); }
  void addOption(const std::string &v, int index) {
    const int stringWidth = Utils::getStringWidth(v);
    if (absoluteSize().x < stringWidth + 2)
      size.x = stringWidth + 2;

    if (index <= selected && selected != -1)
      selected++;
    value.insert(value.begin() + index, v);
  }
  void addOption(const std::string &v) { addOption(v, getOptionsSize()); }
  void deleteOption(int index) { value.erase(value.begin() + index); }

  void onChange(const std::function<void()> &v) { changeListener.set(v); }

private:
  std::vector<std::string> value;
  int selected = 0, hovered = -1;
  int scroll = 0;
  bool isScrolled = false;
  EventListener changeListener;
  void handleLeftDrag(const InputEvent &e) override {
    if (!isScrolled)
      return;

    const Vector2 eventRelativePosition = e.position - absolutePosition(),
                  absSize = absoluteSize();

    scroll = std::clamp(
        (int)round(((value.size() - absSize.y) * eventRelativePosition.y) /
                   (float)(absSize.y - 1)),
        0, (int)value.size() - absSize.y);

    refresh();
  }

  void handleScrollUp(const InputEvent &e) override {
    if (scroll > 0) {
      scroll--;
      refresh();
    }
  }
  void handleScrollDown(const InputEvent &e) override {
    if (value.size() > absoluteSize().y + scroll) {
      scroll++;
      refresh();
    }
  }
  void handleMouseMove(const InputEvent &e) override {
    const Vector2 relativePosition = e.position - absolutePosition();
    if (relativePosition.x < absoluteSize().x - 2) {
      int index = relativePosition.y + scroll;

      hovered = index;
      refresh();
    } else if (hovered != -1) {
      hovered = -1;
      refresh();
    }
  }
  void handleLeftClick(const InputEvent &e) override {
    Vector2 absSize = absoluteSize();
    Vector2 scrollPosition = {
        absSize.x - 1, (int)round(((float)(absSize.y - 1) * (float)(scroll) /
                                   ((float)value.size() - absSize.y)))};

    isScrolled =
        (scrollPosition + absolutePosition() == e.position) && e.value == 'M';

    const Vector2 relativePosition = e.position - absolutePosition();
    if (relativePosition.x < absSize.x - 2) {
      selected = relativePosition.y + scroll;
      changeListener.trigger();
      refresh();
    }
  }
  void stoppedHover(const InputEvent &e) override {
    if (hovered != -1) {
      hovered = -1;
      draw();
    }
  }
  void deactivate(const InputEvent &e) override {
    if (selected != -1) {
      selected = -1;
      draw();
    }
  }
  void displayScroll() {
    Vector2 absSize = absoluteSize();

    Vector2 scrollPosition = {
        absSize.x - 1, (int)round(((float)(absSize.y - 1) * (float)(scroll) /
                                   ((float)value.size() - absSize.y)))};

    shared.mainBuffer +=
        ANSI::setFgColor(styles.standard.bgColor) +
        ANSI::setCursorPosition(absolutePosition() + scrollPosition) + "\u2588";
  }

  void draw() override {
    shared.mainBuffer += ANSI::setColor(styles.standard) +
                         ANSI::setCursorPosition(absolutePosition());
    for (int i = 0; i < absoluteSize().y; i++) {
      if (i + scroll >= value.size() || i + scroll < 0)
        return;

      std::string &line = value[i + scroll];

      ColorPair pair = (i + scroll == selected)
                           ? (styles.selected)
                           : (i == hovered ? styles.hover : styles.standard);

      shared.mainBuffer += ANSI::setColor(pair) + line + ANSI::cursorDown() +
                           ANSI::cursorLeft(line.length());
    }

    displayScroll();
  }
  void initFromString(const std::string &v, bool alias) override {
    const std::vector<std::string> newLines =
        Utils::splitString(v, alias ? '\n' : ',');
    value.insert(value.end(), newLines.begin(), newLines.end());

    for (const std::string &line : newLines) {
      if (line.length() + 2 > absoluteSize().x)
        size.x = Utils::getStringWidth(line) + 2;
    }
  }

  using Element::Element;
};
} // namespace UTUI
