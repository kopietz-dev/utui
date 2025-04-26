#include <fstream>

#include "../element.h"

namespace UTUI {

class Text : public Element {
public:
  std::vector<std::string> value;

  void setLine(const std::string &v, int index) {
    clear();
    value[index] = v;
    const int stringWidth = v.size();
    if (absoluteSize().x < stringWidth)
      size.x = stringWidth;

    draw();
  }
  void addLine(const std::string &v) {
    clear();
    value.push_back(v);
    const int stringWidth = v.size();

    if (absoluteSize().x < stringWidth)
      size.x = stringWidth;

    draw();
  }
  void deleteLine(int index) {
    clear();
    value.erase(value.begin() + index);
    draw();
  }
  std::vector<std::string> getValue() { return value; }
  void clearValue() { value.clear(); }
  void loadTextFromFile(const std::string &filename) {
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
  void draw() override {
    size.y = value.size();

    shared.mainBuffer += ANSI::setColor(styles.standard) +
                         ANSI::setCursorPosition(absolutePosition());

    for (const std::string &line : value) {
      shared.mainBuffer +=
          line + ANSI::cursorDown() + ANSI::cursorLeft(line.length());
    }
  }

private:
  void initFromString(const std::string &v, bool alias) override {
    const std::vector<std::string> newOptions = Utils::splitString(v, ',');

    for (const std::string &str : newOptions) {
      addLine(str);
    }
  }
  using Element::Element;
};
} // namespace UTUI
