#include <functional>

namespace UTUI {

class EventListener {
 public:
  void set(const std::function<void()>& v) { listener = v; }
  void unset() { listener = nullptr; }
  void trigger() {
    if (listener != nullptr) {
      listener();
    }
  }

 private:
  std::function<void()> listener = nullptr;
};

}  // namespace UTUI
