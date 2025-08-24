# Repository Guidelines

## Project Structure & Module Organization
- Source code: `src/` organized by feature; shared utilities live in `src/common/`.
- Tests: `tests/` mirrors `src/` (one test file per module).
- Assets: `assets/` for images/fonts/data; `public/` for static web assets.
- Tooling: `scripts/` for maintenance tasks; config in `config/`; repo‑level dotfiles in root.
- Docs: `docs/` for architecture notes, ADRs, and runbooks.

## Build, Test, and Development Commands
- Install deps: prefer `make setup`/`make install`; otherwise `npm ci`/`pnpm install`/`yarn install` or `pip install -r requirements.txt` (Poetry/uv if configured).
- Run locally: `make dev` or `npm run dev` (hot reload if supported).
- Build: `make build` or `npm run build` (outputs to `dist/` or `build/`).
- Test: `make test`, `pytest -q`, or `npm test`.
- Lint/format: `make lint`, `ruff check`, `eslint .`, `prettier --check .`.

## Coding Style & Naming Conventions
- Formatting: use configured tools (e.g., Black/Prettier). Do not hand‑format.
- Linting: fix all errors and high‑severity warnings before PR.
- Naming: directories `lowercase-with-dashes`; files `lower_snake_case.ext`; classes `PascalCase`; constants `UPPER_SNAKE_CASE`; functions/vars follow language norms (camelCase JS/TS, snake_case Python).
- Indentation: follow formatter defaults (2 spaces JS/TS, 4 spaces Python).

## Testing Guidelines
- Frameworks: use the project’s configured framework (PyTest or Jest/Vitest).
- Structure: place tests in `tests/<feature>/` with names like `test_module.py` or `module.spec.ts`.
- Coverage: maintain or improve overall coverage; add tests for new behavior and fixes.
- Run: `make test` or the language‑specific command above; prefer CI‑friendly flags (e.g., `-q`).

## Commit & Pull Request Guidelines
- Commits: follow Conventional Commits (e.g., `feat(api): ...`, `fix(ui): ...`).
- PRs: include a clear summary, linked issues (e.g., `Closes #123`), and screenshots for UI changes.
- Checks: PRs must pass lint, tests, and build; update docs when behavior or interfaces change.

## Security & Configuration Tips
- Never commit secrets. Provide `.env.example`; load user‑specific values via `.env.local`.
- Pin dependencies when possible; run audits (`npm audit`, `pip-audit`) if available.
- Review diff for sensitive data before pushing.

