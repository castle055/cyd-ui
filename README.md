
<h1 align="center">
  <br>
  <img src="logo.png" alt="Cyd UI" width="200">
  <br>
  CYD UI
  <br>
</h1>

<h4 align="center">A C++ library for building native user interfaces</h4>

<p align="center">
<img alt="Work In Progress" src="https://img.shields.io/badge/-WIP-red?&style=for-the-badge">
<img alt="Language" src="https://img.shields.io/static/v1?style=for-the-badge&message=C%2B%2B&color=00599C&logo=C%2B%2B&logoColor=FFFFFF&label=">
<img alt="Tool" src="https://img.shields.io/static/v1?style=for-the-badge&message=CMake&color=064F8C&logo=CMake&logoColor=FFFFFF&label=">
<img alt="Tool" src="https://img.shields.io/static/v1?style=for-the-badge&message=Cairo+Graphics&color=222222&logo=Cairo+Graphics&logoColor=F39914&label=">
<img alt="GitHub" src="https://img.shields.io/github/license/castle055/cyd-ui?style=for-the-badge">
<img alt="GitHub tag (latest SemVer)" src="https://img.shields.io/github/v/tag/castle055/cyd-ui?color=%23fcae1e&label=latest&sort=semver&style=for-the-badge">
</p>

<p align="center">
  <a href="#overview">Overview</a> •
  <a href="#integration">Integration</a> •
  <a href="#how-to-use">How To Use</a> •
  <a href="#code-architecture">Code Architecture</a> •
  <a href="#credits">Credits</a> •
  <a href="#license">License</a>
</p>

# Overview

Cyd-UI is a C++ library for building native user interfaces. It is aimed at desktop applications and can be easily integrated into existing code bases. Cyd-UI is module based, only preprocessor macros can be included through header files.

## Key Features

* **Declarative** - Or as close as it can get with c++ syntax and macros
* **Event Driven** - Built-in event bus extensible with custom events
* **Coroutines** - A coroutine runtime is available for asynchronous operations
* **Component Oriented** - Declare encapsulated components and reuse them anywhere
* **Module Based** - The whole library is imported as a C++ module
* **Reactive sizing of components** - Dimensions are specified as expressions that get recomputed as needed
* **Multithreaded** - Each window runs in its own thread. A global thread handles system events and compositing
* **Hardware Accelerated** - Components are composited together in the GPU when available

## Road map

* Better component styling:
  - Integrate with CSS (or similar).
  - Implement UI animations.
  - Integrate OpenGL shaders into compositing.
* Optimizations:
  - Allow for shared OpenGL contexts, so that externally rendered buffers can be included in the UI without copying.
* Cross-platform:
  - Windows, macOS support.

# Integration

## CMake project (recommended)

Since this is a module library, the recommended way to integrate it into your project is with CMake. This is easy to do with `FetchContent`:

```cmake
# CmakeLists.txt

include(FetchContent)

FetchContent_Declare(cyd_ui
    GIT_REPOSITORY https://github.com/castle055/cyd-ui.git
    GIT_TAG main # for the latest version, or a version tag such as 'v0.14.0'
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(cyd_ui)
include_directories(${cyd_ui_SOURCE_DIR}/include)
```

## Submodule

As an alternative for projects that do not use CMake as a build system, this repository can be added as a submodule. The whole library can be found within the `include/` directory. Make sure module dependency scanning can find this directory.

# How To Use

## Example 
This example declares two components. The first `SomeComponent` just draws a blue circle. The second component `ExampleComponent` draws an orange rectangle with black text inside as well as including the first component as a child.

```c++
// ExampleComponent.cppm

module;
// Preprocessor macros cannot be exported from modules, so we must include a header file
#include "cyd_ui/components/component_macros.h"

export module ExampleComponent;
import cydui;


export struct COMPONENT(SomeComponent, {
  // This component receives no props from the parent.
} STATE {
  // However, it does have persistent state across redraws.
  bool selected = false;
}) {
  // This component has no children
  
  FRAGMENT {
    // Draw vector graphic elements within this component.
    fragment.draw<vg::circle>()
        .x(0).y(0)
        .r(std::min($width, $height))
        .fill(state.selected? color::Red : color::Blue);
  }
  
  ON_BUTTON_PRESS {
    // React to button press events
    if (button == Button::PRIMARY) {
        state.selected = not state.selected;
        state.mark_dirty();
    }
  }
};


export struct COMPONENT(ExampleComponent, {
  std::string label;
}) {
  CHILDREN {
    // Add other components as children
    return {
        SomeComponent {}
            .y(40_px)
            .width($width)
            .height($height - 40_px)
    };
  }
  
  FRAGMENT {
    // Draw vector graphic elements within this component.
    int font_size = 14;
    fragment.draw<vg::rect>()
        .x(0).y(0)
        .w($width).h(font_size * 2)
        .fill("#FCAE1E"_color);
    fragment.draw<vg::text>(props.label)
        .x(10_px).y(font_size * 1.5)
        .font_family("Helvetica")
        .fill(color::Black);
  }
};
```

In order to show the components on the screen, a window must be created. This is done as follows:

```c++
// main.cpp

import ExampleComponent;
import cydui;

int main() {
  auto win = cyd::ui::CWindow::make<ExampleComponent>({.label = "Hello, world!"})
             .size(800, 800)
             .title("Example Window Title")
             .show();
  
  // Hold main thread in a loop or do something else...
  //while (1);
  return 0;
}
```

The moment the method `show()` is called, the window will appear on the screen and events will start being processed. The main thread is then free to do anything. Events can be used for communication between the main thread and the window thread. 

The window will self-terminate when the `CWindow` object is destroyed. For easier management of ownership, the window is wrapped within a `std::shared_ptr<>` on creation.

# Credits

This software uses the following open source projects:

- [Cairo Graphics](https://www.cairographics.org/)
- [SDL2](https://www.libsdl.org/)

# License

GPL 3.0

[LICENSE.md](LICENSE.md)

---

> GitHub [@castle055](https://github.com/castle055) &nbsp;&middot;&nbsp;

