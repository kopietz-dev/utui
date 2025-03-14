#include "../element.h"

namespace UTUI {

class Range : public Element {
 public:
  int value = 0;

  void setSize(const Vector2& newSize) { size = newSize; }

  void onChange(const std::function<void()>& v) { changeListener.set(v); }

  void onGrab(const std::function<void()>& v) { grabListener.set(v); }

 private:
  bool isDragged = false;
  EventListener changeListener, grabListener;

  void handleLeftClick(const InputEvent& e) override {
    const Vector2 absPos = absolutePosition();
    isDragged = Utils::isInBoundaries({absPos.x + value, absPos.y}, {1, 1},
                                      e.position) &&
                e.value == 'M';

    if (isDragged) {
      grabListener.trigger();
    }
  }
  void handleLeftDrag(const InputEvent& e) override {
    if (!isDragged) {
      return;
    }

    int newValue =
        std::clamp(e.position.x - absolutePosition().x, 0, size.x - 1);

    if (value != newValue) {
      changeListener.trigger();

      value = newValue;
      draw();
    }
  }
  void draw() override {
    shared.mainBuffer += ANSI::setColor(styles.standard) +
                         ANSI::setCursorPosition(absolutePosition()) +
                         Utils::multiplyString("\u2500", value) + "\u25CF" +
                         Utils::multiplyString("\u2500", size.x - value - 1);
  }
  void initFromString(const std::string& v, bool alias) override {
    value = std::stoi(v);
  }
  using Element::Element;
};

}  // namespace UTUI
