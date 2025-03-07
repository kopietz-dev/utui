#include "../element.h"

namespace UTUI {

class Menu : public Element {
 public:
  struct Option {
    std::string value;
    std::vector<Option> options;
    bool expanded = false;
  };
  std::vector<Option> options;
  Color fgColorActive, bgColorActive = {255, 100, 100}, fgColorActiveHover,
                       bgColorActiveHover = {100, 255, 100};
  int clickedDeepnessMin = 0, clickedDeepnessMax = 999;
  Option* getActive() { return getOption(active, options); }
  Option* getExpanded() { return expandedOption; }
  Option* getCollapsed() { return collapsedOption; }

  int getActiveDeepness() { return clickedDeepness; }

  Option* getOption(int index, std::vector<Option>& options) {
    static bool first = true;
    static int counter = 0;

    if (first) {
      counter = 0;
      first = false;
    }

    for (Option& option : options) {
      if (counter == index) {
        first = true;
        return &option;
      }
      counter++;
      Option* opt = getOption(index, option.options);
      if (opt != nullptr) return opt;
    }
    return nullptr;
  }
  Option* getOption(int index) { return getOption(index, options); }

 private:
  int clickedDeepness = 0;
  Option *hovered = nullptr, *expandedOption = nullptr,
         *collapsedOption = nullptr;
  int active = -1;
  void checkForClick(std::vector<Option>& branch, const int deepness,
                     const InputEvent& e) {
    if (e.value != 'M') return;
    static int counter;
    if (deepness == 0) {
      counter = 0;
    }

    for (Option& option : branch) {
      Vector2 optionPosition =
          absolutePosition() + Vector2({deepness, counter});

      if (Utils::isInBoundaries(optionPosition,
                                {static_cast<int>(option.value.length()) +
                                     (option.value.empty() ? 0 : 2),
                                 1},
                                e.position)) {
        if (!option.options.empty() && e.position.x == optionPosition.x) {
          option.expanded = !option.expanded;
          if (option.expanded == false) {
            collapsedOption = &option;
            pushEvent(Event::COLLAPSED_OPTIONS);
          } else {
            expandedOption = &option;
            pushEvent(Event::EXPANDED_OPTIONS);
          }
          refresh();
        } else if (deepness >= clickedDeepnessMin &&
                   deepness <= clickedDeepnessMax) {
          active = counter;
          clickedDeepness = deepness;
          pushEvent(Event::SELECTION_CHANGED);
          draw();
        }
        return;
      }

      counter++;
      if (option.expanded) {
        checkForClick(option.options, deepness + 1, e);
      }
    }
  }
  void handleLeftClick(const InputEvent& e) override {
    checkForClick(options, 0, e);
  }
  void deactivate(const InputEvent& e) override {
    if (active != -1) {
      active = -1;
      draw();
    }
  }
  bool checkForHover(std::vector<Option>& branch, const int deepness,
                     const InputEvent& e) {
    static int counter;
    if (deepness == 0) {
      counter = 0;
    }

    for (Option& option : branch) {
      Vector2 optionPosition =
          absolutePosition() + Vector2({deepness, counter});

      counter++;

      if (deepness >= clickedDeepnessMin && deepness <= clickedDeepnessMax &&
          Utils::isInBoundaries(optionPosition,
                                {static_cast<int>(option.value.length()) +
                                     (option.options.empty() ? 0 : 2),
                                 1},
                                e.position)) {
        hovered = &option;
        return true;
      }

      if (option.expanded) {
        if (checkForHover(option.options, deepness + 1, e)) return true;
      }
    }
    return false;
  }
  void stoppedHover(const InputEvent& e) override {
    if (hovered != nullptr) {
      hovered = nullptr;
      draw();
    }
  }
  void handleMouseMove(const InputEvent& e) override {
    if (checkForHover(options, 0, e)) {
      draw();
    } else {
      stoppedHover(e);
    }
  }

  void renderBranch(std::vector<Option>& branch, const int deepness) {
    static int counter = 0;
    if (deepness == 0) counter = 0;

    for (Option& option : branch) {
      Color fgColor =
          (counter == active)
              ? (&option == hovered ? fgColorActiveHover : fgColorActive)
              : (&option == hovered ? styles.fgColorHover : styles.fgColor);
      Color bgColor =
          (counter == active)
              ? (&option == hovered ? bgColorActiveHover : bgColorActive)
              : (&option == hovered ? styles.bgColorHover : styles.bgColor);
      const int additionalSize = (option.options.empty() ? 0 : 2),
                currentSize = Utils::getStringWidth(option.value) +
                              additionalSize + deepness;
      if (size.x < currentSize) {
        size.x = currentSize;
      }
      shared.mainBuffer +=
          ANSI::setFgColor(fgColor) + ANSI::setBgColor(bgColor);
      if (!option.options.empty()) {
        if (option.expanded) {
          shared.mainBuffer += "\u2BC6 ";
        } else {
          shared.mainBuffer += "\u2BC8 ";
        }
      }

      shared.mainBuffer +=
          option.value + ANSI::cursorDown() +
          ANSI::cursorLeft(option.value.size() - 1 + +additionalSize);

      counter++;
      if (option.expanded) {
        renderBranch(option.options, deepness + 1);
      }
      shared.mainBuffer += ANSI::cursorLeft();
    }
    if (deepness == 0) size.y = counter;
  }

  void draw() override {
    shared.mainBuffer += ANSI::setCursorPosition(absolutePosition());
    renderBranch(options, 0);
  }

  using Element::Element;
};
}  // namespace UTUI
