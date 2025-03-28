#include <cmath>
#include <string>

#include "../element.h"

namespace UTUI {
class ProgressBar : public Element {
 public:
  float value, minValue, maxValue;

  void setSize(int newSize) {
    size.x = newSize;
    size.y = 1;
  }

 private:
  void draw() override {
    const Vector2 absSize = absoluteSize();
    int filled =
        (int)std::roundf(value / (maxValue - minValue) * (float)absSize.x);

    shared.mainBuffer += ANSI::setColor(styles.standard) +
                         ANSI::setCursorPosition(absolutePosition()) +
                         Utils::multiplyString("\u2588", filled) +
                         Utils::multiplyString(" ", absSize.x - filled);
  }
  void initFromString(const std::string& v, bool alias) override {
    value = std::stof(v);
  }
  using Element::Element;
};
}  // namespace UTUI
