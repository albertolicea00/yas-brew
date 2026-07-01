# Contributing Guidelines

Thank you for your interest in contributing to this project! We welcome contributions from developers of all skill levels.

## Development Setup

### Prerequisites

1. **Xcode Command Line Tools**: `xcode-select --install`
2. **Toolchain + Qt 6**: `brew install cmake ninja qt`

The `brew` CLI itself must also be installed — the app is a GUI wrapper around it.

### Build environment

C++/Qt has no virtualenv; isolation comes from **out-of-source builds**: everything generated lives under `build/` (git-ignored) and the "environment" is pinned by `CMakePresets.json`. To reset the environment completely, delete `build/` and configure again.

```bash
cmake --preset default          # 1. configure — creates build/default
cmake --build --preset default  # 2. compile
ctest --preset default          # 3. run tests
open build/default/yas-brew.app
```

### Project structure

- `src/core/` — vendored YAS core (process runner, queue, models, controller). Kept identical across all YAS repos; if you fix a bug here, please mention it so the fix can be replicated suite-wide.
- `src/` — the brew adapter (command builders + output parsers) and `main.cpp`.
- `qml/core/` — shared design system and app shell (also vendored).
- `qml/` — app entry (`Main.qml`: brand accent + tag).
- `tests/` — QtTest unit tests; adapter parsers are tested against recorded CLI output.

## How to Contribute

1. **Reporting Bugs**: Open an issue using the Bug Report template.
2. **Suggesting Features**: Open an issue using the Feature Request template.
3. **Pull Requests**:
   - Fork the repository.
   - Create a feature branch (`git checkout -b feature/amazing-feature`).
   - Commit your changes (`git commit -m 'feat: add some amazing feature'`).
   - Push to the branch (`git push origin feature/amazing-feature`).
   - Open a Pull Request.

## Code Style

- Write clean, readable, and documented code.
- Follow the visual style and tokens outlined in `DESIGN.md`.
- Keep changes concise and focused.
