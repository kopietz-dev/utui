#pragma once
#include <iostream>

#include "elements/bar_chart.h"
#include "elements/button.h"
#include "elements/click_menu.h"
#include "elements/menu.h"
#include "elements/progress_bar.h"
#include "elements/scrollable_menu.h"
#include "elements/scrollable_text.h"
#include "elements/switch.h"
#include "elements/tabbar.h"
#include "elements/text.h"
#include "elements/text_field.h"
#include "elements/text_input.h"
#include "parser.h"
#include "screen.h"
#include "utils.h"
#include "window.h"

namespace UTUI {

class Main {
 public:
  static inline const unsigned int GLOBAL_ID = 0;

  static const Vector2& getScreenSize() { return consoleScreen->size; }
  static InputEvent getInputEvent() { return event; }
  static void displayAll() {
    for (Window* window : windows) {
      if (!window->disabled) window->refresh();
    }
  }
  static void update() {
    // Poll user input
    updateInputEvent();

    if (event.type != InputEventType::NO_EVENT) {
      // Handling event
      for (int i = windows.size() - 1; i >= 0; i--) {
        Window* window = windows[i];

        if (window->sRemove) {
          windows.erase(windows.begin() + i);
          i--;
          continue;
        }

        if (window->update(event)) {
          windows.erase(windows.begin() + i);
          windows.push_back(window);
          //  break;
        }
      }
    }

    const int mainBufferSize = shared.mainBuffer.size();

    // Displaying cursor
    if (mainBufferSize > 0 || shared.cursor.updated.get()) {
      shared.mainBuffer +=
          (shared.cursor.visible ? ANSI::showCursor() : ANSI::hideCursor()) +
          ANSI::setCursorColor(shared.cursor.color) +
          ANSI::setCursorPosition(shared.cursor.position);
    }

    // Buffer flushing
    if (mainBufferSize > 0) {
      write(1, shared.mainBuffer.c_str(), shared.mainBuffer.size());
      shared.mainBuffer.clear();
    }
  }
  static void close() {
    tcgetattr(STDIN_FILENO, &oldTerm);
    write(1, "\033[?1003l\033[?1006l\033[?25h\033[H\033[J", 28);
  }
  static Window& append(unsigned int id = 0) {
    Window* newWindow = new Window(shared, consoleScreen, nullptr, id);

    windows.push_back(newWindow);
    return *windows.back();
  }
  static Window* getWindowById(unsigned int id) {
    for (Window* window : windows) {
      if (window->getID() == id) return window;
    }

    return nullptr;
  }
  static std::vector<Element*> getElementsById(unsigned int id) {
    std::vector<Element*> matching;
    for (Window* window : windows) {
      std::vector<Element*> windowElements = window->getElementsById(id);
      matching.insert(windowElements.begin(), windowElements.end(),
                      matching.end());
    }
    return matching;
  }
  template <typename T>
  static T* getElementById(unsigned int id) {
    static_assert(std::is_base_of<Element, T>::value,
                  "T must be a class derived from UTUI::Element");
    for (Window* window : windows) {
      Element* matching = window->getElementById<T>(id);
      if (matching != nullptr) return dynamic_cast<T*>(matching);
    }
    return nullptr;
  }
  static void hideCursor() { shared.mainBuffer += ANSI::hideCursor(); }
  static void showCursor() { shared.mainBuffer += ANSI::showCursor(); }
  static void disableScroll() { shared.mainBuffer += ANSI::disableScroll(); }
  static void setBgColor(const Color& color) {
    consoleScreen->styles.standard.bgColor = color;
    shared.mainBuffer +=
        ANSI::setBgColor(consoleScreen->styles.standard.bgColor);
    clearScreen();
  }
  static void clearScreen() { shared.mainBuffer += ANSI::clearScreen(); }
  static void init() {
    setlocale(LC_ALL, "");
    struct termios term;

    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ECHO);
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    setvbuf(stdout, nullptr, _IONBF, 0);

    shared.mainBuffer += "\033[?1003h\033[?1006h";

    consoleScreen->position = {1, 1};

    handleResizeSignal(0);

    std::signal(SIGINT, Main::handleCloseSignal);
    std::signal(SIGWINCH, Main::handleResizeSignal);

    hideCursor();
    disableScroll();
    clearScreen();
  }

  static void setColorProperty(
      const std::unordered_map<std::string, std::string>& params,
      const std::string& key, Color& property) {
    auto it = params.find(key);
    if (it != params.end()) {
      if (it->second[0] == '$') {
        auto var = variables.find(it->second);

        if (var != variables.end())
          property = Utils::stringToColor(var->second);
        else {
          Utils::throwError("Variable \"" + it->second +
                            "\" does not exist!\n");
        }
      } else {
        property = Utils::stringToColor(it->second);
      }
    }
  }
  static void setVector2Property(
      const std::unordered_map<std::string, std::string>& params,
      const std::string& key, Vector2& property) {
    auto it = params.find(key);
    if (it != params.end()) {
      property = Utils::stringToVector2(it->second);
    }
  }

  static void appendFromFile(const std::string& filename) {
    std::vector<std::unordered_map<std::string, std::string>> elementParams =
        parser.appendFromFile(filename);

    Window* winPointer = nullptr;
    Element* elemPointer = nullptr;

    for (const std::unordered_map<std::string, std::string>& params :
         elementParams) {
      elemPointer = nullptr;

      const std::string& type = params.find("type")->second;

      if (type[0] == '$') {
        variables[type] = params.find("value")->second;
      }
      if (type[0] == '@') {
        appendFromFile(type.substr(1));
      }

      if (type == "#window") {
        winPointer = &append();
        elemPointer = winPointer;
      }
      if (type == "#button") elemPointer = &winPointer->append<Button>();
      if (type == "#scrollable_text")
        elemPointer = &winPointer->append<ScrollableText>();
      if (type == "#scrollable_menu")
        elemPointer = &winPointer->append<ScrollableMenu>();
      if (type == "#text_input") elemPointer = &winPointer->append<TextInput>();
      if (type == "#text") elemPointer = &winPointer->append<Text>();
      if (type == "#click_menu") elemPointer = &winPointer->append<ClickMenu>();
      if (type == "#progress_bar")
        elemPointer = &winPointer->append<ProgressBar>();
      if (type == "#tabbar") elemPointer = &winPointer->append<Tabbar>();
      if (type == "#text_field") elemPointer = &winPointer->append<TextField>();
      if (type == "#switch") elemPointer = &winPointer->append<Switch>();
      if (type == "#menu") elemPointer = &winPointer->append<Menu>();

      if (elemPointer != nullptr) {
        setColorProperty(params, "fgColor",
                         elemPointer->styles.standard.fgColor);
        setColorProperty(params, "bgColor",
                         elemPointer->styles.standard.bgColor);
        setColorProperty(params, "hover.fgColor",
                         elemPointer->styles.hover.fgColor);
        setColorProperty(params, "hover.bgColor",
                         elemPointer->styles.hover.bgColor);

        setColorProperty(params, "selected.fgColor",
                         elemPointer->styles.selected.fgColor);
        setColorProperty(params, "selected.bgColor",
                         elemPointer->styles.selected.bgColor);

        setVector2Property(params, "relativePosition",
                           elemPointer->relativePosition);
        setVector2Property(params, "position", elemPointer->position);
        setVector2Property(params, "size", elemPointer->size);
        setVector2Property(params, "relativeSize", elemPointer->relativeSize);

        if (auto it = params.find("id"); it != params.end())
          elemPointer->setID(std::stoi(it->second));

        if (auto it = params.find("value"); it != params.end()) {
          if (it->second[0] == '$') {
            elemPointer->initFromString(variables.find(it->second)->second,
                                        true);
          } else {
            elemPointer->initFromString(it->second, false);
          }
        }
      }
    }

    if (auto it = variables.find("$global.bgColor"); it != variables.end()) {
      setBgColor(Utils::stringToColor(it->second));
    }
  }

  static void onResize(const std::function<void()>& v) {
    resizeListener.set(v);
  }
  static void onExit(const std::function<void()>& v) { exitListener.set(v); }

 private:
  static inline SharedValues shared;
  static inline std::vector<Window*> windows;
  static inline InputEvent event;
  static inline struct termios oldTerm;
  static inline EventListener resizeListener, exitListener;
  static inline Parser parser;

  static inline Screen* consoleScreen = new Screen(shared, nullptr, nullptr, 0);

  static inline std::unordered_map<std::string, std::string> variables;

  static void updateInputEvent() {
    char buf[16];
    std::memset(buf, '\a', 16);
    int bytesRead = read(STDIN_FILENO, buf, sizeof(buf));

    if (buf[0] == '\033') {
      if (buf[2] == '<') {
        int c = 0;
        std::string eventValues[4];

        c = 3;
        for (int i = 0; i < 3; i++) {
          while (buf[c] != ';' && buf[c] != 'M' && buf[c] != 'm') {
            eventValues[i] += buf[c];
            c++;
          }
          c++;
        }
        event = {std::stoi(eventValues[0]), buf[c - 1],
                 std::stoi(eventValues[1]), std::stoi(eventValues[2])};
      } else {
        event = {92, buf[2], -1, -1};
      }
    } else {
      if (buf[0] == '\a')
        event = {-1};
      else if (buf[1] == '\a')
        event = {90, buf[0], -1, -1};
    }
  }
  static void handleCloseSignal(int signal) {
    exitListener.trigger();

    UTUI::Main::close();

    exit(0);
  }
  static void handleResizeSignal(int signal) {
    struct winsize win_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size);

    consoleScreen->size = {win_size.ws_col, win_size.ws_row};

    for (Window* window : windows) {
      window->handleResize();
    }

    resizeListener.trigger();
  }
};
}  // namespace UTUI
