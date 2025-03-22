#include <unistd.h>

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

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
  Styles activeStyles;
  int clickedDeepnessMin = 0, clickedDeepnessMax = 999;
  Option* getActive() { return selected; }
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
  int getOptionIndex(const Option& woption, std::vector<Option>& options) {
    static bool first = true;
    static int counter = 0;

    if (first) {
      counter = 0;
      first = false;
    }

    for (Option& option : options) {
      if (&option == &woption) {
        first = true;
        return counter;
      }
      counter++;
      int opt = getOptionIndex(woption, option.options);
      if (opt != -1) return opt;
    }
    return -1;
  }
  void setSelected(int v) {
    selected = getOption(v);
    refresh();
  }
  Option* getOption(int index) { return getOption(index, options); }

  void onExpand(const std::function<void()>& v) { expandListener.set(v); }
  void onCollaps(const std::function<void()>& v) { collapseListener.set(v); }
  void onChange(const std::function<void()>& v) { changeListener.set(v); }

 private:
  int clickedDeepness = 0;
  Option *expandedOption = nullptr, *collapsedOption = nullptr,
         *selected = nullptr, *hovered = nullptr;

  EventListener changeListener, collapseListener, expandListener;

  bool checkForClick(std::vector<Option>& branch, int deepness,
                     const InputEvent& e) {
    if (e.value != 'M') return false;
    static int counter;
    if (deepness == 0) {
      counter = 0;
    }

    for (Option& option : branch) {
      Vector2 optionPosition =
          absolutePosition() + Vector2({deepness, counter});

      if (Utils::isInBoundaries(
              optionPosition,
              {(int)option.value.size() + (option.options.empty() ? 0 : 2), 1},
              e.position)) {
        if (!option.options.empty() && e.position.x == optionPosition.x) {
          option.expanded = !option.expanded;
          if (option.expanded == false) {
            collapsedOption = &option;
            collapseListener.trigger();
          } else {
            expandedOption = &option;
            expandListener.trigger();
          }
        } else if (deepness >= clickedDeepnessMin &&
                   deepness <= clickedDeepnessMax) {
          selected = &option;
          clickedDeepness = deepness;
          changeListener.trigger();
        }
        refresh();
        return true;
      }

      counter++;
      if (option.expanded) {
        if (checkForClick(option.options, deepness + 1, e)) return true;
      }
    }
    return false;
  }
  void handleLeftClick(const InputEvent& e) override {
    checkForClick(options, 0, e);
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

      if (deepness >= clickedDeepnessMin && deepness <= clickedDeepnessMax &&
          Utils::isInBoundaries(
              optionPosition,
              {(int)option.value.length() + (option.options.empty() ? 0 : 2),
               1},
              e.position)) {
        hovered = &option;
        return true;
      }

      counter++;
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

  void renderBranch(std::vector<Option>& branch, int deepness) {
    static int counter = 0;
    if (deepness == 0) counter = 0;

    for (Option& option : branch) {
      ColorPair pair =
          (&option == selected)
              ? (styles.selected)
              : (&option == hovered ? styles.hover : styles.standard);

      const int additionalSize = (option.options.empty() ? 0 : 2),

                currentSize = Utils::getStringWidth(option.value) +
                              additionalSize + deepness;

      if (absoluteSize().x < currentSize) {
        size.x = currentSize;
      }
      shared.mainBuffer += ANSI::setColor(pair);

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

  void initOption(const std::string& v, std::vector<Option>* ptr,
                  int deepness) {
    std::string buffer = "";
    std::vector<char> braces;
    bool isString = false;

    char openBrace = '[', closeBrace = ']', stringBrace = '"';

    for (char c : v) {
      if (c == stringBrace) isString = !isString;

      if (c == openBrace) {
        if (braces.empty()) {
          ptr->push_back({buffer});

          buffer.clear();
        }

        braces.push_back(c);
      } else if (c == closeBrace) {
        if (!braces.empty() && braces.back() == openBrace) {
          braces.pop_back();
        }
        if (braces.empty()) {
          std::string inside = buffer.substr(1, buffer.size() - 1);
          initOption(inside, &ptr->back().options, deepness + 1);
          buffer.clear();
          continue;
        }
      }
      if (c != stringBrace && (isString && braces.empty()) || !braces.empty())
        buffer += c;
    }
  }

  void initFromString(const std::string& v, bool alias) override {
    initOption(v, &options, 0);

    if (!options.empty()) selected = &options[0];
  }
  using Element::Element;
};
}  // namespace UTUI
