# Archive Index

This index summarizes archived snapshots and how to restore or cross‑reference them.

Folders
- `20250815_cleanup/`
  - `patches/` – build/dsp fixes as diffs
  - `source_backups/` – large source zips (move to Release/LFS; record links below)
  - `misc/` – notes, tests, review artifacts
- `ghost_code_cleanup_20250816_204913/`
  - `_MANIFEST.md` – inventory of ghost vs active files at that time
  - `move_ghosts.ps1` – script used during cleanup (deprecated)

Current State
- A generated manifest of the active build vs. stray source files is maintained at `_archive/CURRENT_MANIFEST.md`.
- To regenerate, run: `python tools/archive/generate_manifest.py`

Large Assets (suggested offload)
- `_archive/20250815_cleanup/source_backups/Source*.zip`
  - Action: upload to GitHub Release and place links + SHA256 checksums here.
  - Example record:
    - `Source-3.zip` – SHA256: `<fill-me>` – Release: `<url>`

Legacy Tags (recommended)
- Tag older layouts for one‑step restore in Git: e.g., `legacy-2025-08-16`.
  - Restore: `git checkout legacy-2025-08-16`

Policy
- See `docs/ARCHIVE_POLICY.md` for the full policy and workflow.

