# Archive Policy

Purpose: preserve history and technical rationale without bloating day‑to‑day development.

Scope: applies to `_archive/` and any legacy/backup assets (zips, screenshots, scratch code, patches).

Principles
- Keep manifests and diffs: Preserve `_MANIFEST.md` files and patch/diff sets – these are the canonical record.
- Offload bulky binaries: Move large zip/tar archives to a Release or Git LFS. Keep only links + checksums in repo.
- No build coupling: Nothing under `_archive/` is compiled. Restores happen from tags/releases, not from `_archive/`.
- Snapshot, don’t fork: Tag milestones (e.g. `legacy-YYYYMMDD`) and link them from `_archive/INDEX.md`.
- Automate manifests: Generate current active/ghost file lists from `CMakeLists.txt` to avoid drift.

Operational Workflow
1) When archiving a cleanup:
   - Move non‑compiled files into a dated folder under `_archive/`.
   - Write `_MANIFEST.md` summarizing what moved and why.
   - For large files: attach to a GitHub Release, then record link + SHA256 in the manifest.
2) Keep the build clean:
   - `_archive/` must never appear in `target_sources(...)`. The CMake guard will error if it does.
3) Restoring legacy:
   - Prefer checking out a tag/branch (e.g. `legacy-2025-08-16`).
   - Only if necessary, follow links in `_archive/INDEX.md` to retrieve assets from Releases.
4) Refresh manifests:
   - Run `python tools/archive/generate_manifest.py` to produce `_archive/CURRENT_MANIFEST.md` from the current build.
   - Cross‑link this file from `_archive/INDEX.md`.

Housekeeping
- Retain the last 1–2 archive snapshots in repo (manifests/patches only).
- Aggressively offload binaries and enormous assets to Releases/LFS.
- Keep `_archive/INDEX.md` as the human‑readable table of contents with links and checksums.

