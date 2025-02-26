//
// Created by castle on 2/26/25.
//

export module cydui.animations.complexity;

export namespace cyd::ui {
  enum class AnimationComplexity {
    COMPOSE     = 0, // Only needs compositing
    REPAINT     = 1, // Requires updating and repainting the fragment
    REFLOW      = 2, // Requires recomputing dimensions
    FULL_UPDATE = 3, // Requires an entire component update
  };
}