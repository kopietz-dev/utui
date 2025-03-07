namespace UTUI {

struct Event {
  unsigned int elementID = 0;
  unsigned int type = 0;

  enum TYPES {
    BUTTON_CLICK = 1,
    VALUE_CHANGE = 2,
    SELECTION_CHANGED = 3,
    COLLAPSED_OPTIONS = 4,
    EXPANDED_OPTIONS = 5,
    SUBMIT = 6,
    ACTIVATED = 7,

    SCREEN_RESIZE = 8,
    EXIT_SIGNAL = 9,
  };
};

}  // namespace UTUI
