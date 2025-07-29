/*! \file  UIEventHandler.cppm
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#define DECL_EVENT_HANDLER(EVENT) on##EVENT(const EVENT& ev)

#define HANDLE_EVENT(EVENT)                                                                        \
  make_listener([&](const EVENT& ev) -> fabric::task<> { co_await on##EVENT(ev); })


export module cydui.core.ui.services.UIEventHandler;

export import cydui.core.ui.services.ComponentTree;
export import cydui.core.ui.services.StyleStore;
export import cydui.core.ui.services.HoverState;
export import cydui.core.ui.services.FocusState;
export import cydui.core.ui.services.UIUpdater;

import std;
import fabric.logging;
import fabric.profiling;


namespace cydui::detail::ui::services {
  export class UIEventHandler final: public fabric::services::ServiceBase {
    PlatformController& frame_;
    ComponentTree&      tree_;
    StyleStore&         style_;
    HoverState&         hover_state_;
    FocusState&         focus_state_;
    UIUpdater&          updater_;

    std::vector<fabric::async::raw_listener::sptr> listeners{};

    UIEventHandler(
      PlatformController& frame,
      ComponentTree&      tree,
      StyleStore&         style,
      HoverState&         hover_state,
      FocusState&         focus_state,
      UIUpdater&          updater
    )
        : frame_(frame),
          tree_(tree),
          style_(style),
          hover_state_(hover_state),
          focus_state_(focus_state),
          updater_(updater) {}

  public:
    static fabric::task<sptr> start(fabric::services::ServiceLocator& locator) {
      auto& platform    = co_await locator.require<PlatformController>();
      auto& tree        = co_await locator.require<ComponentTree>();
      auto& style_store = co_await locator.require<StyleStore>();
      auto& hover_state = co_await locator.require<HoverState>();
      auto& focus_state = co_await locator.require<FocusState>();
      auto& updater     = co_await locator.require<UIUpdater>();

      co_return sptr{
        new UIEventHandler(platform, tree, style_store, hover_state, focus_state, updater)
      };
    }

    ~UIEventHandler() override {
      remove_all_listeners();
    }

    fabric::task<> start_listeners() {
      co_await make_event_listeners();
    }

    fabric::task<> stop_listeners() {
      co_await remove_all_listeners();
    }

  private:
    fabric::task<> remove_all_listeners() {
      for (auto& item: listeners) {
        item->remove();
      }
      listeners.clear();
      co_return;
    }

    fabric::task<> make_event_listeners() {
      ZoneScopedN("Layout:make_event_listeners");
      co_await remove_all_listeners();

      auto make_listener = [&](auto&& fun) { return frame_.get_bus().on_event(fun).raw(); };

      listeners = {
        HANDLE_EVENT(RequestComponentFocus),
        HANDLE_EVENT(RedrawEvent),
        HANDLE_EVENT(KeyEvent),
        HANDLE_EVENT(TextInputEvent),
        HANDLE_EVENT(ButtonEvent),
        HANDLE_EVENT(ScrollEvent),
        HANDLE_EVENT(MotionEvent),
        HANDLE_EVENT(ResizeEvent),
      };
    }

    fabric::task<> onRequestComponentFocus(const RequestComponentFocus& ev);
    fabric::task<> onRedrawEvent(const RedrawEvent& ev);
    fabric::task<> onKeyEvent(const KeyEvent& ev);
    fabric::task<> onTextInputEvent(const TextInputEvent& ev);
    fabric::task<> onButtonEvent(const ButtonEvent& ev);
    fabric::task<> onScrollEvent(const ScrollEvent& ev);
    fabric::task<> onMotionEvent(const MotionEvent& ev);
    fabric::task<> onResizeEvent(const ResizeEvent& ev);
  };
} // namespace cydui::core::ui::services
