#include "../element.h"

namespace UTUI {

class ScrollableMenu : public Element {
public:
  EventListener onChange;
  void setScroll(int newScroll) {
    scroll = std::clamp(newScroll, 0, (int)(options.size() - absoluteSize().y));
  }
  int getSelected() { return selected; }

  void clearValue() { options.clear(); }

  void pushOption(const std::string &v) { options.push_back({v}); }
  void popOption() { options.pop_back(); }

  void setSelected(int index) {
    int prevSelected = selected;
    selected = index;

    displayOption(prevSelected);
    displayOption(selected);

    onChange.trigger();
  }

  void removeOption(int index) { options.erase(options.begin() + index); }

  void draw() override {
    shared.mainBuffer += ANSI::setCursorPosition(absolutePosition());

    const Vector2 absSize = absoluteSize();
    for (int i = scroll; i < scroll + absSize.y; i++) {
      if (i >= options.size())
        break;

      displayOption(i);
    }

    drawScroll();
  }

private:
  std::vector<std::string> options;
  int scroll = 0;
  int hovered = -1, selected = 0;
  bool isScrolled = false;
  void handleLeftDrag(const InputEvent &e) override {
    if (!isScrolled)
      return;

    const Vector2 eventRelativePosition = e.position - absolutePosition(),
                  absSize = absoluteSize();

    scroll = std::clamp(
        (int)round(((options.size() - absSize.y) * eventRelativePosition.y) /
                   (float)(absSize.y - 1)),
        0, (int)options.size() - absSize.y);

    refresh();
  }

  void handleScrollUp(const InputEvent &e) override {
    if (scroll > 0) {
      scroll--;
      refresh();
    }
  }
  void handleScrollDown(const InputEvent &e) override {
    if (options.size() > absoluteSize().y + scroll) {
      scroll++;
      refresh();
    }
  }
  void handleMouseMove(const InputEvent &e) override {
    const Vector2 relativePosition = e.position - absolutePosition();
    if (relativePosition.x < absoluteSize().x - 2) {
      int index = relativePosition.y + scroll;

      int prevHovered = hovered;
      hovered = index;
      displayOption(hovered);
      displayOption(prevHovered);

    } else if (hovered != -1) {
      stoppedHover(e);
    }
  }
  void handleLeftClick(const InputEvent &e) override {
    Vector2 absSize = absoluteSize();

    Vector2 scrollPosition = {
        absSize.x - 1, (int)round(((float)(absSize.y - 1) * (float)(scroll) /
                                   ((float)options.size() - absSize.y)))};

    isScrolled = (scrollPosition + absolutePosition() == e.position) &&
                 e.value == 'M' && options.size() > absSize.y;
    const Vector2 relativePosition = e.position - absolutePosition();
    if (relativePosition.x < absSize.x - 2) {
      setSelected(relativePosition.y + scroll);
    }
  }

  void displayOption(int index) {
    Vector2 absSize = absoluteSize();
    if ((index < scroll || index > scroll + absSize.y) || index < 0 ||
        index >= options.size())
      return;

    shared.mainBuffer +=
        ANSI::setCursorPosition(absolutePosition() +
                                Vector2({0, index - scroll})) +
        ANSI::setColor(
            (selected == index)
                ? styles.selected
                : ((hovered == index) ? styles.hover : styles.standard)) +
        options[index] +
        Utils::multiplyString(" ", absSize.x - options[index].size() - 1);
  }

  void drawScroll() {

    Vector2 absSize = absoluteSize();
    if (options.size() <= absSize.y)
      return;

    Vector2 scrollPosition = {
        absSize.x - 1, (int)round(((float)(absSize.y - 1) * (float)(scroll) /
                                   ((float)options.size() - absSize.y)))};

    shared.mainBuffer +=
        ANSI::setFgColor(styles.standard.fgColor) +
        ANSI::setCursorPosition(absolutePosition() + scrollPosition) + "\u2588";
  }

  void stoppedHover(const InputEvent &e) override {
    if (hovered != -1) {
      int prevHovered = hovered;
      hovered = -1;
      displayOption(prevHovered);
    }
  }

  void initFromString(const std::string &v, bool alias) override {
    const std::vector<std::string> newLines =
        Utils::splitString(v, alias ? '\n' : ',');
    options.insert(options.end(), newLines.begin(), newLines.end());

    for (const std::string &line : newLines) {
      if (line.length() + 2 > absoluteSize().x)
        size.x = Utils::getStringWidth(line) + 2;
    }
  }
  using Element::Element;
};

} // namespace UTUI
