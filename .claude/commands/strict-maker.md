ROLE: ENFORCER
MODE: STRICT

This command enforces ZERO TOLERANCE for maker-agent violations.

INSTRUCTION TO MAKER-AGENT:
You are FORBIDDEN from ANY text except the exact schema:

==DIFF==
[unified diff only]
==BUILD==  
[commands only]
==TEST==
[commands only]
==END==

OR if files missing:
MISSING_FILE: <path>

ANY OTHER TEXT = INSTANT REJECTION

No "Perfect!", no "successfully", no explanations, no build verification text.
SCHEMA ONLY or you are non-compliant.

$ARGUMENTS