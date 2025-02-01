
<h1 align="center">
  <br>
  <img src="logo.png" alt="Cyd UI" width="200">
  <br>
  CYD UI
  <br>
</h1>

<h4 align="center">A C++ library for building native user interfaces</h4>

<p align="center">
<img alt="Language" src="https://img.shields.io/static/v1?style=for-the-badge&message=C%2B%2B&color=00599C&logo=C%2B%2B&logoColor=FFFFFF&label=">
<img alt="Tool" src="https://img.shields.io/static/v1?style=for-the-badge&message=CMake&color=064F8C&logo=CMake&logoColor=FFFFFF&label=">
<img alt="Tool" src="https://img.shields.io/static/v1?style=for-the-badge&message=Cairo+Graphics&color=222222&logo=Cairo+Graphics&logoColor=F39914&label=">
<img alt="GitHub" src="https://img.shields.io/github/license/castle055/cyd-ui?style=for-the-badge">
<img alt="GitHub tag (latest SemVer)" src="https://img.shields.io/github/v/tag/castle055/cyd-ui?color=%23fcae1e&label=latest&sort=semver&style=for-the-badge">
</p>

<p align="center">
  <a href="#overview">Overview</a> •
  <a href="#integration">Integration</a> •
  <a href="#usage">Usage</a> •
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

# Usage

## Components 
Components are declared with the macro `COMPONENT() {}`, which must be included from the file `include/cyd_ui/components/component_macros.h`. The syntax is as follows:

```cpp
#include "cyd_ui/components/component_macros.h"

struct COMPONENT(Name, { /* Props... */ } STATE { /* State Variables... */ }) {
  // Event Handlers...
};
```

The name must be a valid class name. The properties and state declarations are both struct declarations, so fields must be separated by semicolons `;`.

Components may implement as many or as few event handlers as they need. User input events follow the pattern `ON_<EVENT> {}`. For instance, if a component needs to react to a key press, it should implement `ON_KEY_PRESS {}`:

```cpp
struct COMPONENT(Name, { /* Props... */ } STATE { /* State Variables... */ }) {
  ON_KEY_PRESS {
    // Do something
    state.mark_dirty(); // Indicate this component has changed and need to be redrawn.
  }
};
```

If the event handler determines that the component has changed and that it should be updated and redrawn, it must call `state.mark_dirty()`.

There are two special handlers, `CHILDREN {}` and `FRAGMENT {}` which do not handle specific events. These are called when it is determined that a component has changed in any way. The `CHILDREN {}` handler specifies the children components, their attributes/props and their dimensions. The `FRAGMENT {}` handler specifies graphic elements to be drawn inside the component. Fragment elements are inspired by SVG element tags and allow for rendering primitives (line, rectangle, circle, ...) as well as text and more.

In this way, the `CHILDREN {}` handler defines the structure of the UI whereas the `FRAGMENT {}` handler defines the actual visual elements that get drawn. Note that components may implement either or both depending on their role.

## Events and Coroutines

Each Cyd-UI window provides its own thread-safe asynchronous bus which is exposed to components as the `window` variable. This includes an event queue and a coroutine runtime.

Events are identified by their type and can be emitted with the following syntax:

```cpp
window.emit< /* Event Type */ >({ /* Event Data */ });
```

Where the event type is defined with the `EVENT() {}` macro as follows:

```cpp
EVENT(SomeEvent) {
  /* Event Data Struct */
};
```

Components can then implement handlers for any event. These handlers will only be active for as long as the component is mounted and showing on the UI.

```cpp
struct COMPONENT(Name, { /* Properties... */ } STATE { /* State Variables... */ }) {
  ON_EVENT( /* Event Type */ , /* Handler Statement */)
  
  ON_EVENT( /* Event Type */ , {
    /* Handler Block */
  })
};
```

The coroutine runtime is still a work in progress and so only implements minimal features. Nonetheless, the syntax for enqueuing an asynchronous operation is as follows:

```cpp
window.coroutine_enqueue([](/* Args... */) -> fabric::async::async</* Return Type */> {
  co_return /* ... */;
}, /* args... */);
```

The return type and the use of `co_return` are needed to tell the compiler that this is a coroutine. This enables the use of async constructs such as `co_await` and `co_yield`. However, the runtime is still in its early stages of development and may not properly handle this yet.

## Contexts



## Example 
This example declares two components. The first `SomeComponent` just draws a blue circle. The second component `ExampleComponent` draws an orange rectangle with black text inside as well as including the first component as a child.

```cpp
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

```cpp
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

GPL 3.0 &nbsp;&middot;&nbsp; [LICENSE.MD](LICENSE.md)

---

> GitHub [@castle055](https://github.com/castle055) &nbsp;&middot;&nbsp;

