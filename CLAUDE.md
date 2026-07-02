# CLAUDE.md — YAS Brew

## What
Native GUI wrapper for **Homebrew** (`brew`). Part of YAS suite (Yet Another Store).
Status: **building & tests green** — vendored core + brew adapter + QML shell compile, 3/3 QtTest suites pass, app launches clean. Pending: manual QA against real brew operations.

## Stack
- C++20 + Qt 6.5+ (Qt Quick / QML, Controls style "Basic"), CMake ≥ 3.24, Ninja via CMakePresets.
- Native windowing via Qt QPA plugin: **cocoa** (macOS).
- CLI execution: `QProcess` wrapping `brew` (args as list, never shell strings). Never bundle Homebrew.

## Architecture (deliberate: NO shared library)
Suite decision: every YAS repo is **self-contained**; the core is a vendored copy, identical across repos.
- `src/core/` — vendored YAS core: `ProcessRunner` (streaming QProcess), `OperationQueue` (serialized FIFO), `CommandLog` (persistent history), `PackageListModel`, `CliDetector`, `AppController` (QML facade, exposed as `App` context property), `PackageManagerAdapter` (abstract).
- `src/brewadapter.{h,cpp}` — the only brew-specific C++: command builders + parsers (`--json=v2` everywhere; text parsing only for `brew search`). Full CLI coverage via `actionCatalog()` (doctor, cleanup, leaves, deps, uses, taps, services...).
- `qml/core/` — vendored design system (`Theme` singleton, URI `Yas.Core`) + `YasAppWindow` shell + views (Explorer/Installed/Updates/Actions/History/Settings). UI model: **uniform base across the suite + per-app extra views** via `YasAppWindow.extraViews` (`[{label, icon, source}]` → sidebar entry + Loader).
- `qml/Main.qml` — brand entry: accent `#FFC107`, tag `BREW`, extraViews (none yet for brew).
- `tests/` — QtTest: process runner, list model, brew parsers (inline fixtures).
- **If you fix a bug in `src/core/` or `qml/core/`, replicate the fix in the other YAS repos** (master template: `../yas-core/` local folder, not published).

## Build & run
```bash
make build   # cmake --preset default + build
make test    # ctest --output-on-failure
make run     # launch the .app
make icon    # regenerate icons/app.icns from icons/icon-left.svg
```
Qt via `brew install cmake ninja qt`; presets point CMAKE_PREFIX_PATH at /opt/homebrew.
Gotcha: raw string literals (`R"(...)"`) in files with `Q_OBJECT` break moc's scanner ("No relevant classes found" → vtable link error) — use escaped literals in tests.

## brew specifics
- No root/sudo ever — brew refuses root.
- brew paths hardcoded in adapter: `/opt/homebrew/bin` (arm64), `/usr/local/bin` (intel) — GUI apps don't inherit shell PATH.
- Formulae vs casks distinguished by `Package::kind`; cask ops get `--cask`; casks can't be pinned.
- Parsers trust brew's `outdated` flag over version string comparison (keg-only/HEAD suffixes).

## Design (see DESIGN.md)
- Dark theme. Base `#222629`, accent **Yellow `#FFC107`**, highlight `#FFC1071A`, text `#F8F8F2` / `#ACADAD`.
- App tag: **BREW**. Fonts bundled in `assets/fonts/` (Inter, Outfit, JetBrains Mono — OFL).

## Conventions
- Conventional Commits, no co-author attribution, **never push to origin** without explicit ask.
- macOS packaging: `.app` bundle (icons/app.icns) → codesign → notarize → `.dmg`; brew cask later.

## Key files
README.md (build section) · CONTRIBUTING.md (dev setup) · DESIGN.md · EULA.md · SECURITY.md · icons/
