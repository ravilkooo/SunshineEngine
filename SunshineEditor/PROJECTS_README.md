# Project management (Editor)

This document explains how to use the simple console-driven project management commands implemented in `EditorApp`.

The editor stores projects in the `Projects/` folder (PROJECTS_DIR). Each project is a folder containing a `scene.json` file and is listed in `Projects/projlist`.

Console flow (when Editor starts):

- When the editor runs it calls `ChooseProject()` which prints project entries and prompts you for input.
- You can provide either an index to select a project, or one of the following management commands:
  - `n` — Create a new project. You will be asked for the folder name. The editor will create the folder under `Projects/`, create a default `scene.json` and add it to the `projlist`.
  - `a` — Add an existing project. You will be asked for the directory name (relative to `Projects/`). The editor will check for `scene.json` inside and add the project to `projlist` if found.
  - `r` — Remove an entry from the project list. You will be asked for the index to remove. This only removes the listing from `projlist`, it does not delete files.

Notes & behavior
- All project names you type should be relative to `Projects/` (e.g. `MyProject` or `MyProject/`).
- The implementation will append a trailing `/` if missing.
- The operations update `Projects/projlist` using the existing `SE::SaveProjects()` serialization.

Next steps
- Add an ImGui interface to expose the same commands and to allow deleting project folders from disk (optional).
- Optionally improve timestamp metadata handling (read last-write time for `scene.json`).
