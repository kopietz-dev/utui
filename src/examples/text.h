#include <fstream>

#include "../element.h"

namespace UTUI {

class Text : public Element {
 public:
  void setLine(const std::string& v, int index) {
    value[index] = v;
    refresh();
  }
  void addLine(const std::string& v, int index) {
    const int stringWidth = Utils::getStringWidth(v);
    if (size.x < stringWidth) size.x = stringWidth;

    value.insert(value.begin() + index, v);
    refresh();
  }
  void deleteLine(int index) {
    value.erase(value.begin() + index);
    refresh();
  }
  std::vector<std::string> getValue() { return value; }
  void clearValue() { value.clear(); }
  void loadTextFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file) {
      return;
    }
    std::string line;
    while (std::getline(file, line)) {
      value.push_back(line);
    }
    file.close();
  }

 private:
  std::vector<std::string> value;
  void draw() override {
    size.y = value.size();
    shared.mainBuffer += ANSI::setFgColor(styles.fgColor) +
                         ANSI::setBgColor(styles.bgColor) +
                         ANSI::setCursorPosition(absolutePosition());

    for (const std::string& line : value) {
      shared.mainBuffer +=
          line + ANSI::cursorDown() + ANSI::cursorLeft(line.length());
    }
  }

  using Element::Element;
};
}  // namespace UTUI
