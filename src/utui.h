#pragma once
#include "window.h"

namespace UTUI {

class Main {
 public:
  static inline const unsigned int GLOBAL_ID = 0;

  static const Vector2 getScreenSize() { return shared.screenSize; }
  static InputEvent getInputEvent() { return event; }
  static bool hasScreenSizeChanged() { return screenSizeChanged.get(); }
  static void displayAll() {
    for (Window* window : windows) {
      window->refresh();
    }
  }
  static std::vector<Event>& getEvents() { return shared.events; }
  static void update() {
    // Poll user input
    shared.events.clear();
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
    // Buffers flushing
    if (shared.cursorBuffer.size() > 0) {
      shared.mainBuffer += shared.cursorBuffer;
      shared.cursorBuffer.clear();
    }
    if (shared.mainBuffer.size() > 0) {
      write(1, shared.mainBuffer.c_str(), shared.mainBuffer.size());
      shared.mainBuffer.clear();
    }
  }
  static void close() {
    tcgetattr(STDIN_FILENO, &oldTerm);
    write(1, "\033[?1003l\033[?1006l\033[?25h\033[H\033[J", 28);
  }
  static Window& append(unsigned int id) {
    Window* newWindow = new Window(shared, id);
    newWindow->size = {3, 3};
    windows.push_back(newWindow);
    return *windows.back();
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
  static void hideCursor() { shared.mainBuffer += ANSI::hideCursor(); }
  static void showCursor() { shared.mainBuffer += ANSI::showCursor(); }
  static void disableScroll() { shared.mainBuffer += ANSI::disableScroll(); }
  static void setBgColor(const Color& color) {
    shared.bgColor = color;
    shared.mainBuffer += ANSI::setBgColor(shared.bgColor);
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

    handleResizeSignal(0);

    std::signal(SIGINT, Main::handleCloseSignal);
    std::signal(SIGWINCH, Main::handleResizeSignal);

    hideCursor();
    disableScroll();
    clearScreen();
  }

 private:
  static inline SharedValues shared;
  static inline std::vector<Window*> windows;
  static inline Flag screenSizeChanged;
  static inline InputEvent event;
  static inline struct termios oldTerm;

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
    shared.events.push_back({0, Event::EXIT_SIGNAL});
  }
  static void handleResizeSignal(int signal) {
    struct winsize win_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size);

    shared.screenSize = {win_size.ws_col, win_size.ws_row};
    shared.events.push_back({0, Event::SCREEN_RESIZE});
  }
};
}  // namespace UTUI
