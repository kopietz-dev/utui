#include "../element.h"

namespace UTUI {
class ProgressBar : public Element {
 public:
  int value;

  void setSize(int newSize) { size.x = newSize; }

 private:
  void draw() override {
    shared.mainBuffer += ANSI::setFgColor(styles.fgColor) +
                         ANSI::setBgColor(styles.bgColor) +
                         ANSI::setCursorPosition(absolutePosition()) +
                         Utils::multiplyString("\u2588", value) +
                         Utils::multiplyString(" ", size.x - value);
  }

  using Element::Element;
};
}  // namespace UTUI
