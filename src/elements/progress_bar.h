#include <string>

#include "../element.h"

namespace UTUI {
class ProgressBar : public Element {
 public:
  int value;

  void setSize(int newSize) {
    size.x = newSize;
    size.y = 1;
  }

 private:
  void draw() override {
    shared.mainBuffer += ANSI::setColor(styles.standard) +
                         ANSI::setCursorPosition(absolutePosition()) +
                         Utils::multiplyString("\u2588", value) +
                         Utils::multiplyString(" ", absoluteSize().x - value);
  }
  void initFromString(const std::string& v, bool alias) override {
    value = std::stoi(v);
  }
  using Element::Element;
};
}  // namespace UTUI
