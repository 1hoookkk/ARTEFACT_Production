ROLE: ARCHITECT
MODE: PLAN

Enforce delegation policy for this session:
- Orchestrator must NOT call Read/Grep/LS/Glob directly.
- All recon = reader-agent; print READER_USED=YES.
- Maker emits DIFF + cmake/ctest commands only; no MCP/Task during PATCH.
- Violations: print DELEGATION_ENFORCED and STOP.

Print ACKNOWLEDGED and restate the rules, then wait.