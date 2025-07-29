/*! \file  service_scopes.cppm
 *! \brief
 *!
 */

export module cydui.service_scopes;

export import fabric.services;

export namespace cydui::services {
  using GlobalScope = fabric::services::GlobalScope;
  using WindowScope = fabric::services::ScopeTag;
  using UIScope = fabric::services::ScopeTag;
}
