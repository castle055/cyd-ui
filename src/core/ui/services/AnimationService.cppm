//
// Created by castle on 2/18/25.
//
module;
#include <tracy/Tracy.hpp>

#define ANONYMOUS_STRUCT(...)                                                                      \
  decltype([&] {                                                                                   \
    struct _anon_ __VA_ARGS__;                                                                     \
    return _anon_{};                                                                               \
  }())

#define KEYFRAME(POS, ...)      {POS, keyframe::make(__VA_ARGS__)}
#define AUTO_KEYFRAME(POS, ...) {POS, keyframe::make(ANONYMOUS_STRUCT(__VA_ARGS__){})}
// Alternate - if the above one doensn't work
// #define AUTO_KEYFRAME(POS, ...) {POS, keyframe::make(ANONYMOUS_STRUCT __VA_ARGS__)}

export module cydui.core.ui.services.AnimationService;

import std;
import reflect;

import fabric.logging;
import fabric.async;
import fabric.wiring.signals;
export import fabric.services;
export import cydui.easing_functions;
export import cydui.interpolation;
export import cydui.animations.Animation;
export import cydui.animations.AnimationState;
export import cydui.animations.AnimationHandle;

export import cydui.core.ui.services.UIRenderer;

using namespace std::chrono_literals;

namespace cydui::detail::ui::services {
  export class AnimationService final: public fabric::services::ServiceBase {
    fabric::tasks::executor::sptr executor_;
    UIRenderer&                   renderer_;

    std::atomic_flag enabled_{false};
    std::atomic_flag stop_flag_{false};

    [[refl::ignore]]
    std::list<animations::AnimationState> active_animations_{};

    explicit AnimationService(
      const fabric::tasks::executor::sptr& executor,
      UIRenderer&                          renderer_
    )
        : executor_(executor),
          renderer_(renderer_) {}

  public:
    static fabric::task<sptr> start(fabric::services::ServiceLocator& locator) {
      auto  exec     = co_await fabric::this_task::get_executor_sptr();
      auto& renderer = co_await locator.require<UIRenderer>();

      co_return sptr{new AnimationService(exec, renderer)};
    }

    void enable() {
      if (enabled_.test_and_set()) {
        return;
      }

      stop_flag_.clear();
      executor_->schedule([&] -> fabric::task<> {
        while (not this->stop_flag_.test()) {
          co_await this->run();
          co_await 16ms;
        }
        co_return;
      }()).detach();
    }

    void disable() {
      if (not enabled_.test()) {
        return;
      }
      stop_flag_.test_and_set();
      enabled_.clear();
    }

  private:
    fabric::task<> run() {
      ZoneScopedN("AnimationSystem");
      auto now = std::chrono::system_clock::now();

      std::forward_list<ComponentImpl*> c_pending_compose{};
      std::forward_list<ComponentImpl*> c_pending_repaint{};
      std::forward_list<ComponentImpl*> c_pending_reflow{};
      std::forward_list<ComponentImpl*> c_pending_full_update{};

      {
        ZoneScopedN("advance");
        for (auto anim = active_animations_.begin(); anim != active_animations_.end();) {
          switch (anim->complexity) {
            case animations::AnimationComplexity::REFLOW:
              c_pending_reflow.push_front(&anim->component);
              break;
            case animations::AnimationComplexity::REPAINT:
              c_pending_repaint.push_front(&anim->component);
              break;
            case animations::AnimationComplexity::COMPOSE:
              c_pending_compose.push_front(&anim->component);
              break;
            case animations::AnimationComplexity::FULL_UPDATE:
              c_pending_full_update.push_front(&anim->component);
              break;
          }

          if (not advance_animation(anim, now)) {
            // Animation Completed: De-animate components
            stop_animation(anim);
          } else {
            ++anim;
          }
        }
        // LOG::print {DEBUG}("Animations ON");
      }

      // TODO - Recompute dimensions if needed

      if (not c_pending_repaint.empty()) {
        co_await renderer_.render();
      } else if (not c_pending_compose.empty()) {
        co_await renderer_.render();
        // compositor_.compose();
      }

      if (active_animations_.empty()) {
        // LOG::print{DEBUG}("Animations OFF");
        this->disable();
      }
    }

  public:
    animations::AnimationHandle start_animation(
      const animations::Animation& anim,
      ComponentImpl&               component
    ) {
      active_animations_.emplace_back(make_animation_state(anim, component));
      const auto it = std::prev(active_animations_.end());
      return component.start_animation(animations::AnimationHandle{[=, this]() {
        stop_animation(it);
      }});
    }

    void stop_animation(const std::list<animations::AnimationState>::iterator& anim_) {
      auto&       anim           = *anim_;
      auto&       component      = anim.component;
      auto&       animated_style = component.get_style_stack().get_animated_style();
      const auto& t_info         = animated_style.get_type();

      for (const auto& [prop_id, prop_info]: anim.property_map) {
        const auto& field_info = prop_info.first;
        animated_style.erase_path({t_info, field_info});
      }

      active_animations_.erase(anim_);
    }

    void stop_animation(animations::AnimationState& anim) {
      for (auto it = active_animations_.begin(); it != active_animations_.end(); ++it) {
        auto& a = *it;
        if (&a == &anim) {
          stop_animation(it);
          return;
        }
      }
    }

  private:
    animations::AnimationState make_animation_state(
      const animations::Animation& anim,
      ComponentImpl&               component
    ) {
      animations::AnimationState state{anim, component, std::chrono::system_clock::now()};

      const refl::type_info& style_ti = component.get_blueprint().get_style_type_info();

      for (const auto& [prop_id, prop_timeline]: anim.get_timelines()) {
        auto& [type_id, prop_name] = prop_id;

        for (const auto& field_ti: style_ti.fields()) {
          if (field_ti.type().id() == type_id and field_ti.name == prop_name) {
            animations::AnimationComplexity complexity = animations::AnimationComplexity::REPAINT;

            const void* field_ptr     = field_ti.get_ptr(&component.get_style());
            refl::any   initial_value = refl::any::make(field_ti.type(), field_ptr);

            state.property_map[prop_id] = {&field_ti, initial_value};
            if (complexity > state.complexity) {
              state.complexity = complexity;
            }

            break;
          }
        }
      }

      return state;
    }

    bool advance_animation(
      std::list<animations::AnimationState>::iterator& anim,
      std::chrono::system_clock::time_point            now
    ) {
      auto x = (now - anim->started).count()
               / static_cast<float>(anim->anim.get_options().duration_.count());
      x = anim->anim.get_options().easing_function_(x);

      if (x >= 1.0) {
        apply_animation_frame(anim, 1.0);
        return false;
      }

      apply_animation_frame(anim, x);

      anim->component.get_style_stack().apply_animations();
      return true;
    }

    void apply_animation_frame(
      std::list<animations::AnimationState>::iterator& anim,
      float                                            frame_x
    ) {
      auto&       component      = anim->component;
      auto&       animated_style = component.get_style_stack().get_animated_style();
      const auto& t_info         = animated_style.get_type();

      for (const auto& [prop_id, prop_info]: anim->property_map) {
        const auto& field_info    = prop_info.first;
        const auto& initial_value = prop_info.second;
        auto&       prop_timeline = anim->anim.get_timelines().at(prop_id);

        refl::any interpolated_value = prop_timeline.interpolate(frame_x, initial_value);
        animated_style.set_field({t_info, field_info}, interpolated_value);
      }
    }
  };
} // namespace cydui::core::ui::services
