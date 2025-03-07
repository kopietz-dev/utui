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
    scroll = std::clamp(newScroll, 0, (int)(value.size() - size.y));
  }
  int getScroll() const { return scroll; }

  int getOptionsSize() { return value.size(); }
  void addOption(const std::string& v, int index) {
    const int stringWidth = Utils::getStringWidth(v);
    if (size.x < stringWidth + 2) size.x = stringWidth + 2;

    if (index <= selected && selected != -1) selected++;
    value.insert(value.begin() + index, v);
  }
  void addOption(const std::string& v) { addOption(v, getOptionsSize()); }
  void deleteOption(int index) { value.erase(value.begin() + index); }

 private:
  std::vector<std::string> value;
  int selected = -1, hovered = -1;
  int scroll = 0;
  bool isScrolled = false;

  void handleLeftDrag(const InputEvent& e) override {
    if (!isScrolled) return;

    const Vector2 eventRelativePosition = e.position - absolutePosition();

    scroll = std::clamp(
        (int)round(((value.size() - size.y) * eventRelativePosition.y) /
                   (float)(size.y - 1)),
        0, (int)value.size() - size.y);

    refresh();
  }

  void handleScrollUp(const InputEvent& e) override {
    if (scroll > 0) {
      scroll--;
      refresh();
    }
  }
  void handleScrollDown(const InputEvent& e) override {
    if (value.size() > size.y + scroll) {
      scroll++;
      refresh();
    }
  }
  void handleMouseMove(const InputEvent& e) override {
    const Vector2 relativePosition = e.position - absolutePosition();
    if (relativePosition.x < size.x - 2) {
      int index = relativePosition.y + scroll;

      hovered = index;
      refresh();
    } else if (hovered != -1) {
      hovered = -1;
      refresh();
    }
  }
  void handleLeftClick(const InputEvent& e) override {
    Vector2 scrollPosition = {
        size.x - 1, (int)round(((float)(size.y - 1) * (float)(scroll) /
                                ((float)value.size() - size.y)))};

    isScrolled =
        (scrollPosition + absolutePosition() == e.position) && e.value == 'M';

    const Vector2 relativePosition = e.position - absolutePosition();
    if (relativePosition.x < size.x - 2) {
      selected = relativePosition.y + scroll;
      pushEvent(Event::SELECTION_CHANGED);
      refresh();
    }
  }
  void stoppedHover(const InputEvent& e) override {
    if (hovered != -1) {
      hovered = -1;
      draw();
    }
  }
  void deactivate(const InputEvent& e) override {
    if (selected != -1) {
      selected = -1;
      draw();
    }
  }
  void displayScroll() {
    Vector2 scrollPosition = {
        size.x - 1, (int)round(((float)(size.y - 1) * (float)(scroll) /
                                ((float)value.size() - size.y)))};

    shared.mainBuffer +=
        ANSI::setFgColor(styles.fgColor) +
        ANSI::setCursorPosition(absolutePosition() + scrollPosition) + "\u2588";
  }
  void draw() override {
    shared.mainBuffer += ANSI::setFgColor(styles.fgColor) +
                         ANSI::setBgColor(styles.bgColor) +
                         ANSI::setCursorPosition(absolutePosition());
    for (int i = 0; i < size.y; i++) {
      if (i + scroll >= value.size() || i + scroll < 0) return;

      std::string& line = value[i + scroll];

      if (selected == i + scroll)
        shared.mainBuffer += ANSI::setFgColor((hovered == i + scroll)
                                                  ? styles.fgColorSelectedHover
                                                  : styles.fgColorSelected) +
                             ANSI::setBgColor((hovered == i + scroll)
                                                  ? styles.bgColorSelectedHover
                                                  : styles.bgColorSelected) +

                             line + ANSI::cursorDown() +
                             ANSI::cursorLeft(line.length());
      else
        shared.mainBuffer +=
            ANSI::setFgColor((hovered == i + scroll) ? styles.fgColorHover
                                                     : styles.fgColor) +
            ANSI::setBgColor((hovered == i + scroll) ? styles.bgColorHover
                                                     : styles.bgColor) +

            line + ANSI::cursorDown() + ANSI::cursorLeft(line.length());
    }

    displayScroll();
  }

  using Element::Element;
};
}  // namespace UTUI
