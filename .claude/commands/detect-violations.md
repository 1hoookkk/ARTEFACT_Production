ROLE: VALIDATOR
MODE: AUDIT

Scan maker-agent output for protocol violations. Auto-reject any output containing forbidden patterns.

VIOLATION PATTERNS (instant reject):
□ Repository narration: "I see", "The file contains", "Looking at", "Based on"
□ Code analysis prose: "This shows", "Currently implements", "The analysis"  
□ Line references: "Line X", "At line", "Lines 1-20"
□ Architecture explanation: "The system", "This class", "The integration"
□ Recon observations: "From the reconnaissance", "The reader-agent found"

REQUIRED FORMAT VIOLATIONS:
□ Missing ==DIFF== markers
□ Missing ==BUILD== markers  
□ Missing ==TEST== markers
□ Missing ==END== marker
□ Text outside schema blocks

ACCEPTABLE OUTPUT (only):
□ "MISSING_FILE: <path>"
□ Unified diff in ==DIFF== block
□ Build commands in ==BUILD== block
□ Test commands in ==TEST== block

USAGE:
Pass maker-agent output as $ARGUMENTS. Returns PASS/FAIL with violation details.

ENFORCEMENT:
- FAIL = discard output, rollback to last green state
- No salvaging or editing of violating output
- Retry with stricter frozen checklist

$ARGUMENTS