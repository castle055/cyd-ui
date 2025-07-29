/*! \file  CairoSceneGraph.cppm
 *! \brief
 *!
 */

export module cydui.platform.render.cairo.CairoSceneGraph;

import std;
import reflect;

import fabric.logging;
export import cydui.layer;
export import cydui.platform.render.cairo.CairoSurface;

namespace cydui::platform::render {
  export struct CairoLayerClip {
    AffineTransform transform;
    vec2            position;
    vec2            size;
  };

  export struct CairoLayerInstance {
    AffineTransform transform;
    CairoLayerClip  clip;
    Layer*          layer;
  };

  //---------------------------------------------------------------------------
  export struct CairoSceneNode {
    using sptr = std::shared_ptr<CairoSceneNode>;

    CairoSurface                  surface;
    AffineTransform               transform;
    vec2                          size;
    std::list<CairoLayerInstance> layers;
    std::list<sptr>               children {};

    static sptr make(
      const CairoSurface& surface,
      AffineTransform     transform = {},
      vec2 size                     = {}) {
      if (size == vec2::zero()) {
        size = {
          static_cast<double>(surface.get_size().first),
          static_cast<double>(surface.get_size().second)};
      }
      return std::make_shared<CairoSceneNode>(surface, transform, size);
    }

    sptr make_children(
      const vec2&            size,
      const AffineTransform& transform) {
      auto ptr =
        make(CairoSurface {static_cast<int>(size[0]), static_cast<int>(size[1])}, transform, size);
      children.push_back(ptr);
      return ptr;
    }

    void append_layer(
      Layer*                 layer,
      const AffineTransform& transform,
      const CairoLayerClip&  clip) {
      layers.push_back({transform, clip, layer});
    }
  };

  export class CairoSceneGraph {
    CairoSceneNode::sptr root_;

  public:
    explicit CairoSceneGraph(const CairoSurface& root_surface)
        : root_(CairoSceneNode::make(root_surface)) {}

    CairoSceneNode::sptr get_root() {
      return root_;
    }
  };
} // namespace cydui::platform::render
