ROLE: ARCHITECT  
MODE: FREEZE

Generate a FROZEN CHECKLIST from reader-agent recon output for maker-agent consumption.

Process:
1) Extract ONLY verified file paths from recon
2) Convert observations into binary assertions (exists/missing)
3) Define exact DoD requirements with test criteria
4) Strip all prose, explanations, and code snippets

FROZEN CHECKLIST FORMAT:
==FROZEN FOR MAKER==
FILES: 
□ path1 (EXISTS/MISSING)
□ path2 (EXISTS/MISSING)

ASSERTIONS:
□ Binary fact 1 (VERIFIED/UNVERIFIED)
□ Binary fact 2 (VERIFIED/UNVERIFIED)

DOD:
□ Project builds without errors
□ Test X executes and passes (exit 0)
□ No RT-safety violations

FORBIDDEN:
- Any repository narration
- Code snippets outside diff format
- Explanatory prose
- Recon observations
==END FROZEN==

The maker-agent receives ONLY the frozen section above, never full recon data.

$ARGUMENTS