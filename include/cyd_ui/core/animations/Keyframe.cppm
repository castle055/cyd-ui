//
// Created by castle on 2/18/25.
//
export module cydui.animations.Keyframe;

import std;
import reflect;

import fabric.logging;
export import cydui.interpolation;
export import cydui.animations.PropertyTimeline;

using namespace std::chrono_literals;

namespace cydui::animations {
  export class Keyframe {
    Keyframe() = default;

  public:
    class properties_base_t {
      friend class Keyframe;

      friend class Keyframe;
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
      friend class Keyframe;

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
    static Keyframe make(
      float          position,
      PropertiesType value = {}
    ) {
      Keyframe kf;
      kf.position_   = position;
      kf.properties_ = std::make_shared<properties_t<PropertiesType>>(value);
      return kf;
    }

    Keyframe& easing(easing::function_type fun) {
      easing_function_ = fun;
      return *this;
    }

    template <typename I>
      requires(std::derived_from<
               I,
               interpolator_base>)
    Keyframe& interp(
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
    Keyframe& interp(
      property_id_t property,
      I             interpolator
    ) {
      properties_->set_property_interpolator(property, std::make_shared<I>(interpolator));
      return *this;
    }

    Keyframe& interp(
      const std::string&    property_name,
      easing::function_type fun
    ) {
      properties_->set_property_interpolator(property_name, std::make_shared<interp::easing>(fun));
      return *this;
    }

    Keyframe& interp(
      property_id_t         property,
      easing::function_type fun
    ) {
      properties_->set_property_interpolator(property, std::make_shared<interp::easing>(fun));
      return *this;
    }

    Keyframe& config(auto&& fun) {
      fun(*this);
      return *this;
    }

    float get_position() const {
      return position_;
    }

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

  export using keyframe_list = std::initializer_list<Keyframe>;
} // namespace cydui::animations
