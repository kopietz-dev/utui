#include "../src/utui.h"

int main() {
  // Initialize UTUI
  UTUI::Main::init();

  // Load elements from file
  UTUI::Main::appendFromFile(UTUI::Utils::getExecutableDir() + "index.utui");

  // Retrieve reference to button
  auto &button1 = *UTUI::Main::getElementById<UTUI::Button>(10);

  // Handle resize
  UTUI::Main::onResize([&]() { UTUI::Main::refresh(); });

  // Set event callback
  button1.onClick.set([&]() {
    button1.setValue(" I was clicked! ");

    UTUI::Color red =
        UTUI::Utils::stringToColor(UTUI::Main::getVariable("$red"));

    button1.styles.standard.bgColor = red;
    button1.styles.hover.fgColor = red;

    button1.updateStyles();
  });

  // Display all elements before main loop
  UTUI::Main::refresh();
  while (1) {
    // Poll user input
    UTUI::Main::update();
  }
  return 0;
}
