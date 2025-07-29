//
// Created by castle on 2/18/25.
//
export module cydui.animations.AnimationState;

import std;

import fabric.logging;
import fabric.async;
import fabric.wiring.signals;
export import cydui.core.Component.impl;
export import cydui.easing_functions;
export import cydui.interpolation;
export import cydui.animations.complexity;
export import cydui.animations.Animation;

using namespace std::chrono_literals;

namespace cydui::animations {
  export struct AnimationState {
    Animation                             anim;
    [[refl::ignore]] detail::ComponentImpl& component;
    std::chrono::system_clock::time_point started;
    AnimationComplexity                   complexity = AnimationComplexity::REPAINT;
    std::unordered_map<
      std::pair<refl::type_id_t, std::string>,
      std::pair<const refl::field_info*, refl::any>>
      property_map{};

    AnimationState(
      const Animation&                      anim_,
      detail::ComponentImpl&                  component_,
      std::chrono::system_clock::time_point started_
    )
        : anim(anim_),
          component(component_),
          started(started_) {}
  };
} // namespace cydui::animations
