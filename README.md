# utui
Lightweight and Flexible C++ TUI library

## Table of Contents

- [Description](#description)
- [Usage](#usage)

## Description

UTUI provides a simple and flexible way to create interactive, ASCII based user interfaces in the terminal.

## Usage
Example code

```cpp
#include "src/examples/button.h"
#include "src/types.h"
#include "src/utui.h"

int main() {
  // Initialize UTUI
  UTUI::Main::init();

  // Set background color
  const UTUI::Color bgColor = {70, 70, 100};
  UTUI::Main::setBgColor(bgColor);

  // Create window with id 1
  UTUI::Window& window = UTUI::Main::append(0);
  window.styles.bgColor = bgColor;
  window.setSize(UTUI::Main::getScreenSize());

  // Create button with id 2
  auto& button = window.append<UTUI::Button>(2);
  button.setText("Click me!");

  // Display all elements before main loop
  UTUI::Main::displayAll();

  while (1) {
    // Poll user input
    UTUI::Main::update();

    // Handle events
    std::vector<UTUI::Event>& events = UTUI::Main::getEvents();

    for (UTUI::Event& e : events) {
      // Handle Global events
      if (e.elementID == UTUI::Main::GLOBAL_ID) {
        if (e.type == UTUI::Event::EXIT_SIGNAL) {
          UTUI::Main::close();
          exit(0);
        }
        if (e.type == UTUI::Event::SCREEN_RESIZE) {
          window.setSize(UTUI::Main::getScreenSize());
          UTUI::Main::displayAll();
        }
      }

      // Handle button click
      if (e.elementID == button.getID() &&
          e.type == UTUI::Event::TYPES::BUTTON_CLICK) {
        button.setText("I was clicked!");
      }
    }
  }
  return 0;
}
