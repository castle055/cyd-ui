/*! \file  CairoFrameRenderer.cppm
 *! \brief
 *!
 */

module;
#include <cairomm/cairomm.h>

#include "../../../../debug/profiling/macros.h"
#define PROF_CURRENT_MODULE cydui::platform::render::cairo::CairoFrameRenderer

export module cydui.platform.render.cairo.CairoFrameRenderer;

import std;
import reflect;

import fabric.logging;
export import cydui.platform.render.renderer_base;
export import cydui.platform.render.cairo;
export import cydui.platform.render.cairo.CairoSceneGraph;
export import cydui.platform.render.cairo.CairoRenderContext;
export import cydui.platform.render.cairo.CairoRenderData;
import cydui.platform.render.cairo.ElementPainters;
import cydui.debug.profiling;

constexpr bool between(
  auto x,
  auto min,
  auto max) noexcept {
  return x >= min && x <= max;
}

constexpr bool rectangle_contains(
  cydui::vec2 x,
  cydui::vec2 size,
  cydui::vec2 origin = cydui::vec2::zero()) noexcept {
  return between(x[0], origin[0], origin[0] + size[0]) and between(x[1], origin[1], origin[1] + size[1]);
}

namespace cydui::platform::render {
  export class CairoFrameRenderer final: public FrameRendererBase {
    CairoRenderContext::sptr context_;
    CairoSurface&            target_surface_;
    int                      width_, height_;

  public:
    explicit CairoFrameRenderer(
      const CairoRenderContext::sptr& context,
      int                             width,
      int                             height)
        : context_(context),
          target_surface_(context_->get_swap_buffer().get_back()),
          width_(width),
          height_(height) {
      target_surface_.resize(width_, height_);
    }

    fabric::task<> render(detail::ComponentImpl& root_component) override {
      CairoSceneGraph      scene {target_surface_};
      CairoSceneNode::sptr root_node {scene.get_root()};
      root_node->transform = root_component.get_layer().transform;

      auto [w, h] = target_surface_.get_size();
      const vec2 size {static_cast<double>(w), static_cast<double>(h)};
      construct_scene(scene, root_node, root_component, AffineTransform {}, {AffineTransform {}, size});
      target_surface_.clear();
      co_await render_scene(*scene.get_root());
      compose_scene(*scene.get_root());

      co_return;
    }

    fabric::task<Surface> finish() override {
      auto& buffer = context_->get_swap_buffer();
      buffer.swap();
      co_return buffer.get_front().get_surface();
    }

  private:
    void construct_scene(
      CairoSceneGraph&            scene,
      const CairoSceneNode::sptr& node,
      detail::ComponentImpl&      component,
      const AffineTransform&      transform,
      const CairoLayerClip&       clip) {
      PROF_SCOPE(Construct Scene);
      Layer& current_layer      = component.get_layer();
      auto   absolute_transform = clip.transform * transform;

      node->append_layer(&current_layer, transform, clip);

      AffineTransform scroll_translation =
        AffineTransform::from_translation(current_layer.viewport.origin - current_layer.viewport.position);
      AffineTransform clip_transform =
        absolute_transform * AffineTransform::from_translation(current_layer.viewport.position);

      CairoSceneNode::sptr target_node {node};
      std::size_t          children_count = node->children.size();
      std::size_t          children_index = 1;
      for (const auto& child: component.get_children_impl()) {
        Layer& layer = child->get_layer();
        if (not layer.render_data.has_value()) {
          LOG::print {ERROR} //
          ("Component has no render data: {} [ID: {}]", child->get_name(), child->get_id().str());
        }

        // Cull if outside of parent's viewport
        auto p0 = layer.transform * vec2 {0, 0};
        auto p1 = layer.transform * vec2 {layer.size[0], 0};
        auto p2 = layer.transform * layer.size;
        auto p3 = layer.transform * vec2 {0, layer.size[1]};

        bool cull = (layer.style.position == position_e::RELATIVE)
                    and not rectangle_contains(p0, current_layer.viewport.size, current_layer.viewport.origin * -1)
                    and not rectangle_contains(p1, current_layer.viewport.size, current_layer.viewport.origin * -1)
                    and not rectangle_contains(p2, current_layer.viewport.size, current_layer.viewport.origin * -1)
                    and not rectangle_contains(p3, current_layer.viewport.size, current_layer.viewport.origin * -1);
        if (not cull) {
          CairoRenderData& data = layer.render_data.value().as<CairoRenderData>();

          if (needs_dedicated_surface(layer)) {
            CairoSceneNode::sptr child_node {nullptr};
            CairoLayerClip       clip_ {};
            if (layer.style.position == position_e::RELATIVE) {
              child_node = node->make_children(layer.size, clip_transform * scroll_translation * layer.transform);
              clip_ = CairoLayerClip {clip_transform, current_layer.viewport.origin * -0, current_layer.viewport.size};
            } else {
              child_node = node->make_children(layer.size, layer.transform);
              clip_      = CairoLayerClip {
                     {},
                     vec2 {0, 0},
                     vec2 {static_cast<double>(width_), static_cast<double>(height_)}
              };
            }
            if (children_index < children_count) {
              target_node = node->make_children(target_node->size, AffineTransform {});
            }
            construct_scene(scene, child_node, *child, AffineTransform {}, clip_);
          } else {
            construct_scene(
              scene,
              target_node,
              *child,
              scroll_translation * layer.transform,
              CairoLayerClip {clip_transform, current_layer.viewport.origin * -0, current_layer.viewport.size});
          }
        }

        ++children_index;
      }
    }

    fabric::task<> render_scene(CairoSceneNode& node) {
      render_scene_node(node);
      for (const auto& child: node.children) {
        co_await render_scene(*child);
      }
    }

    void render_scene_node(CairoSceneNode& node) {
      PROF_SCOPE(Render Scene Node);
      static constexpr bool debug = false;
      auto                  ctx   = Cairo::Context::create(node.surface.get_cairo_surface());
      for (const auto& [transform, clip, layer]: node.layers) {
        PROF_SCOPE(Render Layer);
        ctx->save();
        ctx->transform(to_cairo_matrix(clip.transform));
        if constexpr (debug) {
          element_painter<elements::Rectangle>::paint(
            ctx,
            elements::Rectangle {
          }
              .border("#ff0000"_color)
              .border_dasharray({{10, 10}})
              .border_width(1)
              .border_radius(1_px)
              .x(clip.position[0])
              .y(clip.position[1])
              .w(clip.size[0] - 1)
              .h(clip.size[1] - 1));
        }
        ctx->rectangle(clip.position[0], clip.position[1], clip.size[0], clip.size[1]);
        ctx->clip();
        ctx->transform(to_cairo_matrix(transform));
        element_painter<elements::Rectangle>::paint(ctx, layer->style.background);
        if constexpr (debug) {
          element_painter<elements::Rectangle>::paint(
            ctx,
            elements::Rectangle {}
              .border("#00ff00"_color)
              .border_width(1)
              .border_radius(1_px)
              .x(-0_px)
              .y(-0_px)
              .w(layer->size[0] - 1)
              .h(layer->size[1] - 1));
        }
        AffineTransform origin_translation = AffineTransform::from_translation(layer->viewport.origin);
        ctx->transform(to_cairo_matrix(origin_translation));
        if constexpr (debug) {
          element_painter<elements::Rectangle>::paint(
            ctx,
            elements::Rectangle {}
              .border("#0000ff"_color)
              .border_width(1)
              .border_radius(1_px)
              .x(-1_px)
              .y(-1_px)
              .w(layer->viewport.size[0] + 1)
              .h(layer->viewport.size[1] + 1));
        }
        ctx->rectangle(0, 0, layer->viewport.size[0], layer->viewport.size[1]);
        ctx->clip();
        for (const auto& element: layer->elements) {
          std::visit(
            [&]<typename T>(T&& it) { //
              element_painter<std::remove_cvref_t<T>>::paint(ctx, it);
            },
            element);
        }
        ctx->restore();
      }
    }

    void compose_scene(CairoSceneNode& node) {
      auto ctx = Cairo::Context::create(node.surface.get_cairo_surface());

      for (const auto& child: node.children) {
        compose_scene_node(*ctx, *child);
      }
    }

    void compose_scene_node(
      Cairo::Context& ctx,
      CairoSceneNode& node) {
      PROF_SCOPE(Compose Scene Node);
      ctx.save();
      ctx.transform(to_cairo_matrix(node.transform));
      ctx.set_source(node.surface.get_cairo_surface(), 0, 0);
      ctx.rectangle(0, 0, node.size[0], node.size[1]);
      ctx.clip();
      ctx.paint();
      ctx.restore();
      for (const auto& child: node.children) {
        compose_scene_node(ctx, *child);
      }
    }

    static bool needs_dedicated_surface(const Layer& layer) {
      const LayerStyle& style = layer.style;
      return style.animated or style.op != LayerStyle::OVERLAY or style.position != position_e::RELATIVE;
    }
  };
} // namespace cydui::platform::render
