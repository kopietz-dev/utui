#include <string>

#include "../element.h"

namespace UTUI {

class ScrollableText : public Element {
 public:
  std::vector<std::string> getValue() const { return value; }

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
  std::vector<std::string> value;

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
        ANSI::setFgColor(styles.standard.fgColor) +
        ANSI::setCursorPosition(absolutePosition() + scrollPosition) + "\u2588";
  }
  void draw() override {
    shared.mainBuffer += ANSI::setColor(styles.standard);
    for (int i = 0; i < size.y; i++) {
      if (i + scroll >= value.size() || i + scroll < 0) return;

      std::string& line = value[i + scroll];

      shared.mainBuffer +=
          ANSI::setCursorPosition(absolutePosition() + Vector2({0, i})) + line;
    }

    displayScroll();
  }
  void initFromString(const std::string& v, bool alias) override {
    const std::vector<std::string> newLines =
        Utils::splitString(v, alias ? '\n' : ',');
    value.insert(value.end(), newLines.begin(), newLines.end());

    for (const std::string& line : newLines) {
      if (line.length() + 2 > size.x) size.x = Utils::getStringWidth(line) + 2;
    }
  }
  using Element::Element;
};
}  // namespace UTUI
