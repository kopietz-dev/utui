#include <fstream>

#include "../element.h"

namespace UTUI {

class Text : public Element {
 public:
  void setLine(const std::string& v, int index) {
    value[index] = v;
    refresh();
    const int stringWidth = Utils::getStringWidth(v);
    if (absoluteSize().x < stringWidth) size.x = stringWidth;
  }
  void addLine(const std::string& v, int index) {
    const int stringWidth = Utils::getStringWidth(v);
    if (absoluteSize().x < stringWidth) size.x = stringWidth;

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
    shared.mainBuffer += ANSI::setColor(styles.standard) +
                         ANSI::setCursorPosition(absolutePosition());

    for (const std::string& line : value) {
      shared.mainBuffer +=
          line + ANSI::cursorDown() + ANSI::cursorLeft(line.length());
    }
  }
  void initFromString(const std::string& v, bool alias) override {
    const std::vector<std::string> newOptions = Utils::splitString(v, ',');
    value.insert(value.end(), newOptions.begin(), newOptions.end());
  }
  using Element::Element;
};
}  // namespace UTUI
