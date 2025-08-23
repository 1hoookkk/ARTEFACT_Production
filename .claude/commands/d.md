ROLE: ORCHESTRATOR
MODE: PLAN (do not implement; delegate)

Directive:
- You MUST delegate every recon to reader-agent. If you (orchestrator) attempt Read/Grep/LS/Glob directly, ABORT and print: DELEGATION_ENFORCED.
- Use maker-agent only for PATCH after SPEC approval.
- Reader may call Gemini; maker must not call MCP during DIFF.

Process:
1) RECON (reader-agent)
   - Show PATHS and BYTES (numbered). For symbols, add HITS ±20 lines.
   - If Gemini is helpful, call it and print MCP + MCP_SUMMARY, then still show repo bytes.
2) Wait for my SPEC approval (I will list FILES (LOCKED)).
3) PATCH (maker-agent)
   - Produce ONE unified diff limited to FILES (LOCKED).
   - Output exact build + test commands.
4) STOP after printing. Do not auto-apply.

$ARGUMENTS