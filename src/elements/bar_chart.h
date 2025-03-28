#include <cmath>
#include <string>
#include <vector>

#include "../element.h"

namespace UTUI {
class BarChart : public Element {
 public:
  std::vector<float> values;
  float floor = -1, ceiling = 1;

  void setSize(Vector2 newSize) { size = newSize; }

 private:
  void draw() override {
    shared.mainBuffer += ANSI::setColor(styles.standard) +
                         ANSI::setCursorPosition(absolutePosition());

    const Vector2 absSize = absoluteSize();
    const int barWidth =
        (int)std::roundf((float)absSize.x / (float)values.size());

    for (int i = 0; i < values.size(); i++) {
      shared.mainBuffer += ANSI::setCursorPosition(
          absolutePosition() + Vector2({i * barWidth, absSize.y - 1}));

      shared.mainBuffer += Utils::multiplyString(
          Utils::multiplyString("\u2588", barWidth) + ANSI::cursorUp() +
              ANSI::cursorLeft(barWidth),
          (int)std::roundf((values[i] - floor) / (ceiling - floor) *
                           (float)absSize.y));
    }
  }
  void initFromString(const std::string& v, bool alias) override {
    const std::vector<std::string> strs = Utils::splitString(v, ',');

    for (const std::string& str : strs) {
      values.push_back(std::stof(str));
    }
  }
  using Element::Element;
};
}  // namespace UTUI
