/*! \file  UI.cxx
 *! \brief
 *!
 */

module;
#include <tracy/Tracy.hpp>

module cydui.ui_handle;

import std;
import reflect;
import fabric.logging;
import fabric.profiling;
import cydui.ui_handle.impl;

namespace cydui {
  fabric::task<UI::sptr>
  make_impl(Platform::sptr platform, Blueprint::uptr root, const UIOptions& options) {
    co_return co_await UIImpl::make(
      std::dynamic_pointer_cast<platform::PlatformImpl>(platform),
      std::move(root),
      options);
  }
} // namespace cydui
