#include "../element.h"

namespace UTUI {

class ScrollableText : public Element {
 public:
  std::vector<std::string> value;
  void loadTextFromFile(const std::string& filename) {
    value.clear();
    std::ifstream file(filename);

    if (!file) {
      return;
    }
    std::string line;
    while (std::getline(file, line)) {
      const int stringWidth = Utils::getStringWidth(line);
      if (size.x < stringWidth + 2) size.x = stringWidth + 2;
      value.push_back(line);
    }
    file.close();
  }

  void setSize(int newSize) { size.y = newSize; }

  void setScroll(int newScroll) {
    scroll = std::clamp(newScroll, 0, (int)(value.size() - size.y));
  }
  int getScroll() const { return scroll; }

 private:
  int scroll = 0;
  bool isScrolled = false;

  void handleLeftClick(const InputEvent& e) override {
    Vector2 scrollPosition = {
        size.x - 1, (int)round(((float)(size.y - 1) * (float)(scroll) /
                                ((float)value.size() - size.y)))};

    isScrolled =
        (scrollPosition + absolutePosition() == e.position) && e.value == 'M';
  }
  void handleLeftDrag(const InputEvent& e) override {
    if (!isScrolled) return;

    const Vector2 eventRelativePosition = e.position - absolutePosition();

    scroll = std::clamp(
        (int)round((float)(value.size() - size.y) *
                   (float)eventRelativePosition.y / (float)(size.y - 1)),
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
  void displayScroll() {
    Vector2 scrollPosition = {
        size.x - 1, (int)round(((float)(size.y - 1) * (float)(scroll) /
                                ((float)value.size() - size.y)))};

    shared.mainBuffer +=
        ANSI::setCursorPosition(absolutePosition() + scrollPosition) + "\u2588";
  }
  void draw() override {
    shared.mainBuffer += ANSI::setFgColor(styles.fgColor) +
                         ANSI::setBgColor(styles.bgColor) +
                         ANSI::setCursorPosition(absolutePosition());
    for (int i = 0; i < size.y; i++) {
      if (i + scroll >= value.size() || i + scroll < 0) return;

      std::string& line = value[i + scroll];

      if (line.length() + 2 > size.x) size.x = line.length() + 2;
      shared.mainBuffer +=
          line + ANSI::cursorDown() + ANSI::cursorLeft(line.length());
    }

    displayScroll();
  }

  using Element::Element;
};
}  // namespace UTUI
