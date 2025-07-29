/*! \file  fs_dialogs.cppm
 *! \brief
 *!
 */

export module cydui.fs;

import std;
import reflect;

export namespace cydui::fs {
  using Path = std::filesystem::path;

  struct OpenFileDialogOptions {
    std::vector<std::pair<std::string, std::string>> filters {};
    std::string                                      default_path {""};
  };

  struct SaveFileDialogOptions {
    std::vector<std::pair<std::string, std::string>> filters {};
    std::string                                      default_path {""};
    std::string                                      default_name {""};
  };

  struct PickFolderDialogOptions {
    bool        allow_multiple {false};
    std::string default_path {""};
  };

  std::optional<Path>              open_file_dialog(const OpenFileDialogOptions& options = {});
  std::optional<std::vector<Path>> open_multiple_files_dialog(const OpenFileDialogOptions& options = {});

  std::optional<Path> save_file_dialog(const SaveFileDialogOptions& options = {});

  std::optional<Path>              pick_folder_dialog(const PickFolderDialogOptions& options = {});
  std::optional<std::vector<Path>> pick_multiple_folders_dialog(const PickFolderDialogOptions& options = {});
} // namespace cydui::fs
