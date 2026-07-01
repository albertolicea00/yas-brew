# CLAUDE.md — YAS Brew

## What
Native GUI wrapper for **Homebrew** (`brew`). Part of YAS suite (Yet Another Store).
Status: **scaffolded** — vendored core + brew adapter + QML shell + tests in place. Not yet compiled/QA'd on a real machine.

## Stack
- C++20 + Qt 6.5+ (Qt Quick / QML, Controls style "Basic"), CMake ≥ 3.24, Ninja via CMakePresets.
- Native windowing via Qt QPA plugin: **cocoa** (macOS).
- CLI execution: `QProcess` wrapping `brew` (args as list, never shell strings). Never bundle Homebrew.

## Architecture (deliberate: NO shared library)
Suite decision: every YAS repo is **self-contained**; the core is a vendored copy, identical across repos.
- `src/core/` — vendored YAS core: `ProcessRunner` (streaming QProcess), `OperationQueue` (serialized FIFO), `CommandLog` (persistent history), `PackageListModel`, `CliDetector`, `AppController` (QML facade, exposed as `App` context property), `PackageManagerAdapter` (abstract).
- `src/brewadapter.{h,cpp}` — the only brew-specific C++: command builders + parsers (`--json=v2` everywhere; text parsing only for `brew search`). Full CLI coverage via `actionCatalog()` (doctor, cleanup, leaves, deps, uses, taps, services...).
- `qml/core/` — vendored design system (`Theme` singleton, URI `Yas.Core`) + `YasAppWindow` shell + views (Explorer/Installed/Updates/Actions/History/Settings).
- `qml/Main.qml` — brand entry: accent `#FFC107`, tag `BREW`.
- `tests/` — QtTest: process runner, list model, brew parsers (inline fixtures).
- **If you fix a bug in `src/core/` or `qml/core/`, replicate the fix in the other YAS repos** (master template: `../yas-core/` local folder, not published).

## Build & run
```bash
cmake --preset default && cmake --build --preset default
open build/default/yas-brew.app
ctest --preset default
```
Qt via `brew install cmake ninja qt`; presets already point CMAKE_PREFIX_PATH at /opt/homebrew.

## brew specifics
- No root/sudo ever — brew refuses root.
- brew paths hardcoded in adapter: `/opt/homebrew/bin` (arm64), `/usr/local/bin` (intel) — GUI apps don't inherit shell PATH.
- Formulae vs casks distinguished by `Package::kind`; cask ops get `--cask`; casks can't be pinned.
- Parsers trust brew's `outdated` flag over version string comparison (keg-only/HEAD suffixes).

## Design (see DESIGN.md)
- Dark theme. Base `#1E1E2E`, accent **Yellow `#FFC107`**, highlight `#FFC1071A`, text `#F8F8F2` / `#A9B1D6`.
- App tag: **BREW**. Fonts bundled in `assets/fonts/` (Inter, Outfit, JetBrains Mono — OFL).

## Conventions
- Conventional Commits, no co-author attribution, **never push to origin** without explicit ask.
- macOS packaging: `.app` bundle (icons/app.icns) → codesign → notarize → `.dmg`; brew cask later.

## Key files
README.md (build section) · CONTRIBUTING.md (dev setup) · DESIGN.md · EULA.md · SECURITY.md · icons/
