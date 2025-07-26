/*! \file  frame_map.cppm
 *! \brief 
 *!
 */

export module cydui.backends.sdl3:frame_map;

import std;
export import reflect;

export import cydui.backends.frame_base;

namespace cydui::backends {
  export class SDL3_frame;

  class frame_map {
    std::unordered_map<frame_base::id_type, std::shared_ptr<SDL3_frame>> map;

  public:
    using sptr = std::shared_ptr<frame_map>;

    void register_frame(frame_base::id_type id, const std::shared_ptr<SDL3_frame>& frame) {
      map[id] = frame;
    }
    void unregister_frame(frame_base::id_type id) {
      map.erase(id);
    }

    bool contains(frame_base::id_type id) const {
      return map.contains(id);
    }

    auto at(frame_base::id_type id) {
      return map.at(id);
    }
  };
}

