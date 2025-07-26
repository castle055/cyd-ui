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

export module cydui.animations;

import std;

import fabric.logging;
import fabric.async;
import fabric.wiring.signals;
export import cydui.core.mounted;
export import cydui.easing_functions;
export import cydui.interpolation;
export import cydui.animations.complexity;

using namespace std::chrono_literals;

export namespace cydui::animations {
  using property_id_t = std::pair<refl::type_id_t, std::string>;
}

template <>
struct std::hash<cydui::animations::property_id_t> {
  std::size_t operator()(const cydui::animations::property_id_t& x) const noexcept {
    return std::hash<std::size_t>{}(x.first) ^ (std::hash<std::string>{}(x.second) << 1);
  }
};

namespace cydui::animations {
  struct property_timeline_t {
    refl::any interpolate(
      double    x,
      refl::any initial_value
    ) const {
      if (keyframes.empty()) {
        return initial_value;
      }

      if (x >= std::get<0>(keyframes.back())) {
        return std::get<1>(keyframes.back());
      }

      const refl::any* prev   = &initial_value;
      double           prev_x = 0.0;
      for (const auto& [x_f, kf, interpolator, easing_fun]: keyframes) {
        if (x > x_f) {
          prev   = &kf;
          prev_x = x_f;
        } else {
          float z = (x - prev_x) / (x_f - prev_x);
          return interpolator(*prev, kf, easing_fun(z));
        }
      }

      return initial_value;
    }

    std::list<std::tuple<
      double,
      refl::any,
      std::function<refl::any(refl::any, refl::any, double)>,
      easing::function_type>>
      keyframes{};
  };

  using property_timeline_map_t = std::unordered_map<property_id_t, property_timeline_t>;

  export class animation_data;

  export class keyframe {
    keyframe() = default;

  public:
    class properties_base_t {
      friend class keyframe;

      friend class keyframe;
      virtual ~properties_base_t() = default;
      virtual void add_to_timeline_map(
        double                       x,
        property_timeline_map_t&     map,
        const easing::function_type& easing_fun
      ) = 0;

      virtual void set_property_interpolator(
        const std::string&      property_name,
        interpolator_base::sptr i
      ) = 0;
      virtual void set_property_interpolator(
        const property_id_t&    property_id,
        interpolator_base::sptr i
      ) = 0;
    };

    template <class PropertiesType>
    class properties_t final: public properties_base_t {
      friend class keyframe;

    public:
      explicit properties_t(PropertiesType value = {})
          : properties_base_t(),
            properties_obj_(value) {}

    private:
      void add_to_timeline_map(
        double                       x,
        property_timeline_map_t&     map,
        const easing::function_type& easing_fun
      ) override {
        for_each_prop([&]<typename Field>(auto& prop) {
          add_prop_to_timeline<Field>(x, map, easing_fun);
        });
      }

      template <typename Field>
      void add_prop_to_timeline(
        double                       x,
        property_timeline_map_t&     map,
        const easing::function_type& easing_fun
      ) {
        using PropType = typename Field::type;

        property_id_t prop_id{refl::type_id<PropType>, Field::name};

        auto                               interpolator_iter = property_interp_map_.find(prop_id);
        std::shared_ptr<interpolator_base> interpolator_ptr;
        if (interpolator_iter == property_interp_map_.end()) {
          interpolator_ptr = std::make_shared<interp::lerp>();
        } else {
          interpolator_ptr = interpolator_iter->second;
        }

        property_timeline_t& prop_timeline = map[prop_id];

        prop_timeline.keyframes.push_back(
          {x,
           refl::any::make(
             refl::type_info::from<PropType>(), &Field::from_instance(properties_obj_)
           ),
           [=](refl::any from_a, refl::any to_a, float x) -> refl::any {
             PropType& from = from_a.as<PropType>();
             PropType& to   = to_a.as<PropType>();
             if constexpr (std::is_same_v<PropType, float>) {
               PropType result = interpolator_ptr->interpolate(from, to, x);
               return refl::any::make(result);
             } else if constexpr (HasInterpolationMapping<PropType>) {
               float    from_f   = interp_mapping<PropType>::to_float(from);
               float    to_f     = interp_mapping<PropType>::to_float(to);
               float    result_f = interpolator_ptr->interpolate(from_f, to_f, x);
               PropType result   = interp_mapping<PropType>::from_float(result_f);
               return refl::any::make(result);
             } else {
               return from_a;
             }
           },
           easing_fun}
        );
      }

      void set_property_interpolator(
        const std::string&      property_name,
        interpolator_base::sptr i
      ) override {
        for_each_prop([&]<typename Field>(auto& prop) {
          if (Field::name == property_name) {
            refl::type_id_t property_type_name = refl::type_id<typename Field::type>;
            property_id_t   property_id{property_type_name, property_name};
            property_interp_map_[property_id] = i;
          }
        });
      }
      void set_property_interpolator(
        const property_id_t&    property_id,
        interpolator_base::sptr i
      ) override {
        const auto& [property_type_name, property_name] = property_id;
        for_each_prop([&]<typename Field>(auto& prop) {
          if (Field::name == property_name
              and refl::type_id<typename Field::type> == property_type_name) {
            property_interp_map_[property_id] = i;
          }
        });
      }

      void for_each_prop(auto&& fun) {
        [&]<std::size_t... I>(std::index_sequence<I...>) {
          (for_each_prop_i<refl::field<PropertiesType, I>>(std::forward<decltype(fun)>(fun)), ...);
        }(std::make_index_sequence<refl::field_count<PropertiesType>>{});
      }
      template <typename Field>
      void for_each_prop_i(auto&& fun) {
        fun.template operator()<Field>(Field::from_instance(properties_obj_));
      }

      PropertiesType                                             properties_obj_;
      std::unordered_map<property_id_t, interpolator_base::sptr> property_interp_map_{};
    };

  public:
    template <class PropertiesType>
    static keyframe make(
      float          position,
      PropertiesType value = {}
    ) {
      keyframe kf;
      kf.position_   = position;
      kf.properties_ = std::make_shared<properties_t<PropertiesType>>(value);
      return kf;
    }

    friend class animation_data;

    keyframe& easing(easing::function_type fun) {
      easing_function_ = fun;
      return *this;
    }

    template <typename I>
      requires(std::derived_from<
               I,
               interpolator_base>)
    keyframe& interp(
      const std::string& property_name,
      I                  interpolator
    ) {
      properties_->set_property_interpolator(property_name, std::make_shared<I>(interpolator));
      return *this;
    }

    template <typename I>
      requires(std::derived_from<
               I,
               interpolator_base>)
    keyframe& interp(
      property_id_t property,
      I             interpolator
    ) {
      properties_->set_property_interpolator(property, std::make_shared<I>(interpolator));
      return *this;
    }

    keyframe& interp(
      const std::string&    property_name,
      easing::function_type fun
    ) {
      properties_->set_property_interpolator(property_name, std::make_shared<interp::easing>(fun));
      return *this;
    }

    keyframe& interp(
      property_id_t         property,
      easing::function_type fun
    ) {
      properties_->set_property_interpolator(property, std::make_shared<interp::easing>(fun));
      return *this;
    }

    keyframe& config(auto&& fun) {
      fun(*this);
      return *this;
    }

  private:
    void add_to_timeline_map(property_timeline_map_t& map) {
      properties_->add_to_timeline_map(position_, map, easing_function_);
    }

  private:
    float                              position_{0.0f};
    easing::function_type              easing_function_{easing::linear};
    std::shared_ptr<properties_base_t> properties_{};
    // std::unordered_map<std::pair<refl::type_id_t, std::string>,
    // std::shared_ptr<properties_base_t>> properties_{};
  };

  export class AnimationSystem;
  export class animation;

  export struct animation_opts {
    using duration_t = std::chrono::system_clock::duration;

    animation_opts() = default;

    animation_opts& easing(easing::function_type fun) {
      easing_function_ = fun;
      return *this;
    }

    animation_opts& duration(duration_t d) {
      duration_ = d;
      return *this;
    }

    easing::function_type easing_function_{easing::linear};
    duration_t            duration_;
  };

  class animation_data {
    friend class animation;
    friend class AnimationSystem;

    using duration = std::chrono::system_clock::duration;

    animation_data() = default;

    explicit animation_data(
      std::initializer_list<keyframe> keyframes,
      animation_opts&                 opts
    )
        : options_(opts) {
      for (const auto& kf: keyframes) {
        keyframes_.emplace_back(kf.position_, kf);
      }
      compile_timelines();
    }

  private:
    void compile_timelines() {
      timelines_.clear();
      for (auto& [x, kf]: keyframes_) {
        kf.add_to_timeline_map(timelines_);
      }
    }

  private:
    std::vector<std::pair<double, keyframe>> keyframes_;
    property_timeline_map_t                  timelines_{};

    animation_opts options_{};
  };

  export using keyframe_list = std::initializer_list<keyframe>;

  class animation {
  public:
    friend class AnimationSystem;

    using duration         = animation_data::duration;
    using frame_iterator_t = decltype(animation_data::keyframes_.begin());

    explicit animation(
      std::initializer_list<keyframe> keyframes,
      animation_opts                  opts = {}
    )
        : data_(
            std::shared_ptr<animation_data>(new animation_data{
              keyframes,
              opts
            })
          ) {}


  private:
    std::shared_ptr<animation_data> data_;
  };

  export struct animation_state {
    friend class AnimationSystem;

    animation_state(
      const animation&                      anim_,
      core::mounted_component_t&            component_,
      std::chrono::system_clock::time_point started_
    )
        : anim(anim_),
          component(component_),
          started(started_) {}

    animation anim;
    [[refl::ignore]]
    core::mounted_component_t&            component;
    std::chrono::system_clock::time_point started;

  private:
    AnimationComplexity complexity = AnimationComplexity::REPAINT;
    std::unordered_map<
      std::pair<refl::type_id_t, std::string>,
      std::pair<const refl::field_info*, refl::any>>
      property_map{};
  };

  class AnimationSystem {
    fabric::tasks::executor::sptr executor_;
    std::atomic_flag              enabled_{false};
    std::atomic_flag              stop_flag_{false};

    [[refl::ignore]]
    std::list<animation_state> active_animations_{};

  public:
    fabric::wiring::output_signal<AnimationSystem> s_render_all{};
    fabric::wiring::output_signal<AnimationSystem> s_compose_all{};

    explicit AnimationSystem(const fabric::tasks::executor::sptr& executor)
        : executor_(executor) {}

    void enable() {
      if (enabled_.test_and_set()) {
        return;
      }

      stop_flag_.clear();
      executor_->schedule([&] -> fabric::task<> {
        while (not this->stop_flag_.test()) {
          this->run();
          co_await 16ms;
        }
        co_return;
      });
    }
    void disable() {
      if (not enabled_.test()) {
        return;
      }
      stop_flag_.test_and_set();
      enabled_.clear();
    }

  private:
    void run() {
      ZoneScopedN("AnimationSystem");
      auto now = std::chrono::system_clock::now();

      std::forward_list<core::mounted_component_t*> c_pending_compose{};
      std::forward_list<core::mounted_component_t*> c_pending_repaint{};
      std::forward_list<core::mounted_component_t*> c_pending_reflow{};
      std::forward_list<core::mounted_component_t*> c_pending_full_update{};

      {
        ZoneScopedN("advance");
        for (auto anim = active_animations_.begin(); anim != active_animations_.end();) {
          switch (anim->complexity) {
            case AnimationComplexity::REFLOW:
              c_pending_reflow.push_front(&anim->component);
              break;
            case AnimationComplexity::REPAINT:
              c_pending_repaint.push_front(&anim->component);
              break;
            case AnimationComplexity::COMPOSE:
              c_pending_compose.push_front(&anim->component);
              break;
            case AnimationComplexity::FULL_UPDATE:
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
        s_render_all.emit();
      } else if (not c_pending_compose.empty()) {
        s_compose_all.emit();
      }

      if (active_animations_.empty()) {
        // LOG::print{DEBUG}("Animations OFF");
        this->disable();
      }
    }

  public:
    animation_state& start_animation(
      const animation&           anim,
      core::mounted_component_t& component
    ) {
      component.start_animation();
      active_animations_.emplace_back(make_animation_state(anim, component));
      return active_animations_.back();
    }

    void stop_animation(std::list<animation_state>::iterator& anim_) {
      auto&       anim           = *anim_;
      auto&       component      = anim.component;
      auto&       animated_style = component.get_style_stack().get_animated_style();
      const auto& t_info         = animated_style.get_type();

      for (const auto& [prop_id, prop_info]: anim.property_map) {
        const auto& field_info = prop_info.first;
        animated_style.erase_path({t_info, field_info});
      }

      anim.component.stop_animation();
      active_animations_.erase(anim_);
    }

    void stop_animation(animation_state& anim) {
      for (auto it = active_animations_.begin(); it != active_animations_.end(); ++it) {
        auto& a = *it;
        if (&a == &anim) {
          stop_animation(it);
          return;
        }
      }
    }

    void clear_animations_for_component(core::mounted_component_t& component) {
      for (auto it = active_animations_.begin(); it != active_animations_.end();) {
        if (&component == &it->component) {
          component.stop_animation();
          it = active_animations_.erase(it);
        } else {
          ++it;
        }
      }
    }

  private:
    animation_state make_animation_state(
      const animation&           anim,
      core::mounted_component_t& component
    ) {
      animation_state state{anim, component, std::chrono::system_clock::now()};

      const refl::type_info& style_ti = component.get_blueprint()->get_style_type_info();

      for (const auto& [prop_id, prop_timeline]: anim.data_->timelines_) {
        auto& [type_id, prop_name] = prop_id;

        for (const auto& field_ti: style_ti.fields()) {
          if (field_ti.type().id() == type_id and field_ti.name == prop_name) {
            AnimationComplexity complexity = AnimationComplexity::REPAINT;

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
      std::list<animation_state>::iterator& anim,
      std::chrono::system_clock::time_point now
    ) {
      auto x = (now - anim->started).count()
               / static_cast<float>(anim->anim.data_->options_.duration_.count());
      x = anim->anim.data_->options_.easing_function_(x);

      if (x >= 1.0) {
        apply_animation_frame(anim, 1.0);
        return false;
      }

      apply_animation_frame(anim, x);

      anim->component.get_style_stack().apply_animations();
      return true;
    }

    void apply_animation_frame(
      std::list<animation_state>::iterator& anim,
      float                                 frame_x
    ) {
      auto&       component      = anim->component;
      auto&       animated_style = component.get_style_stack().get_animated_style();
      const auto& t_info         = animated_style.get_type();

      for (const auto& [prop_id, prop_info]: anim->property_map) {
        const auto& field_info    = prop_info.first;
        const auto& initial_value = prop_info.second;
        auto&       prop_timeline = anim->anim.data_->timelines_.at(prop_id);

        refl::any interpolated_value = prop_timeline.interpolate(frame_x, initial_value);
        animated_style.set_field({t_info, field_info}, interpolated_value);
      }
    }
  };

  export class animation_handle {
    AnimationSystem& system_;
    animation_state& state_;

  public:
    animation_handle(
      AnimationSystem& system,
      animation_state& state
    )
        : system_(system),
          state_{state} {}

    void stop() {
      system_.stop_animation(state_);
    }
  };

  export animation_handle animate(
    core::mounted_component_t& component,
    const animation&           anim
  ) {
    const auto& win              = component.get_window();
    auto&       resource_context = *win->get_executor()->get_spawn_context();
    auto&       anim_sys         = *resource_context.get_resource<AnimationSystem>();

    anim_sys.enable();

    auto& state = anim_sys.start_animation(anim, component);
    return animation_handle(anim_sys, state);
  }
} // namespace cydui::animations
