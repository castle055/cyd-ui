/*! \file  fs_dialogs.cxx
 *! \brief
 *!
 */

module;
#include <nfd.hpp>
module cydui.fs;

using namespace cydui::fs;

std::optional<Path> cydui::fs::open_file_dialog(const OpenFileDialogOptions& opts) {
  NFD::Guard guard {};

  nfdfilteritem_t*             filter_items = nullptr;
  int                          filter_count = 0;
  std::vector<nfdfilteritem_t> filters;
  for (const auto& f: opts.filters) {
    filters.push_back({f.first.c_str(), f.second.c_str()});
  }
  if (!filters.empty()) {
    filter_items = filters.data();
    filter_count = static_cast<int>(filters.size());
  }

  const nfdnchar_t* def_path = opts.default_path.empty() ? nullptr : opts.default_path.c_str();

  NFD::UniquePath path;
  if (NFD::OpenDialog(path, filter_items, filter_count, def_path) == NFD_OKAY) {
    return path.get();
  }

  return std::nullopt;
}
std::optional<std::vector<Path>> cydui::fs::open_multiple_files_dialog(const OpenFileDialogOptions& opts) {
  NFD::Guard guard {};

  nfdfilteritem_t*             filter_items = nullptr;
  int                          filter_count = 0;
  std::vector<nfdfilteritem_t> filters;
  for (const auto& f: opts.filters) {
    filters.push_back({f.first.c_str(), f.second.c_str()});
  }
  if (!filters.empty()) {
    filter_items = filters.data();
    filter_count = static_cast<int>(filters.size());
  }

  const nfdnchar_t* def_path = opts.default_path.empty() ? nullptr : opts.default_path.c_str();

  NFD::UniquePathSet paths;
  if (NFD::OpenDialogMultiple(paths, filter_items, filter_count, def_path) == NFD_OKAY) {
    std::vector<Path> out;
    nfdpathsetsize_t  count;
    NFD::PathSet::Count(paths, count);
    for (size_t i = 0; i < count; ++i) {
      NFD::UniquePathSetPathN p;
      NFD::PathSet::GetPath(paths, i, p);
      out.emplace_back(p.get());
    }
    return out;
  }

  return std::nullopt;
}

std::optional<Path> cydui::fs::save_file_dialog(const SaveFileDialogOptions& opts) {
  NFD::Guard      guard {};
  NFD::UniquePath path;

  nfdfilteritem_t*             filter_items = nullptr;
  int                          filter_count = 0;
  std::vector<nfdfilteritem_t> filters;
  for (const auto& f: opts.filters) {
    filters.push_back({f.first.c_str(), f.second.c_str()});
  }
  if (!filters.empty()) {
    filter_items = filters.data();
    filter_count = static_cast<int>(filters.size());
  }

  const nfdnchar_t* def_path = opts.default_path.empty() ? nullptr : opts.default_path.c_str();
  const nfdnchar_t* def_name = opts.default_name.empty() ? nullptr : opts.default_name.c_str();

  if (NFD::SaveDialog(path, filter_items, filter_count, def_path, def_name) == NFD_OKAY) {
    return path.get();
  }
  return std::nullopt;
}

std::optional<Path> cydui::fs::pick_folder_dialog(const PickFolderDialogOptions& opts) {
  NFD::Guard        guard {};
  const nfdnchar_t* def_path = opts.default_path.empty() ? nullptr : opts.default_path.c_str();

  NFD::UniquePath path;
  if (NFD::PickFolder(path, def_path) == NFD_OKAY) {
    return path.get();
  }

  return std::nullopt;
}

std::optional<std::vector<Path>> cydui::fs::pick_multiple_folders_dialog(const PickFolderDialogOptions& opts) {
  NFD::Guard guard {};

  const nfdnchar_t* def_path = opts.default_path.empty() ? nullptr : opts.default_path.c_str();

  NFD::UniquePathSet paths;
  if (NFD::PickFolderMultiple(paths, def_path) == NFD_OKAY) {
    std::vector<Path> out;
    nfdpathsetsize_t  count;
    NFD::PathSet::Count(paths, count);
    for (size_t i = 0; i < count; ++i) {
      NFD::UniquePathSetPathN p;
      NFD::PathSet::GetPath(paths, i, p);
      out.emplace_back(p.get());
    }
    return out;
  }

  return std::nullopt;
}
