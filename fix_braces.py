#!/usr/bin/env python3
"""Fix missing opening braces in PluginProcessor.cpp"""

import re

# Read the file
with open('Source/Core/PluginProcessor.cpp', 'r', encoding='utf-8-sig') as f:
    lines = f.readlines()

# Pattern to match function definitions
func_pattern = re.compile(r'^(void|bool|int|float|double|auto|juce::\w+)\s+ARTEFACTAudioProcessor::\w+.*\)(\s*const)?\s*$')

# Fix the file
fixed_lines = []
i = 0
while i < len(lines):
    line = lines[i]
    fixed_lines.append(line)
    
    # Check if this is a function definition line
    if func_pattern.match(line.strip()):
        # Check if next line starts with '{' or is a comment/empty
        if i + 1 < len(lines):
            next_line = lines[i + 1]
            # If next line doesn't start with '{', add it
            if not next_line.strip().startswith('{'):
                fixed_lines.append('{\n')
    
    i += 1

# Also need to add closing braces before next function definitions
final_lines = []
for i, line in enumerate(fixed_lines):
    # Check if this is a function definition and previous lines need a closing brace
    if func_pattern.match(line.strip()) and i > 0:
        # Look back to see if we need a closing brace
        # Check if previous non-empty line is not a closing brace or directive
        prev_idx = i - 1
        while prev_idx >= 0 and (not fixed_lines[prev_idx].strip() or 
                                  fixed_lines[prev_idx].strip().startswith('//')):
            prev_idx -= 1
        
        if prev_idx >= 0:
            prev_line = fixed_lines[prev_idx].strip()
            # If previous line is not }, #endif, or other preprocessor, add }
            if (prev_line and 
                not prev_line.endswith('}') and 
                not prev_line.startswith('#') and
                not prev_line.startswith('//====')):
                # Insert closing brace before any comment lines
                insert_idx = prev_idx + 1
                while insert_idx < i and fixed_lines[insert_idx].strip().startswith('//'):
                    insert_idx -= 1
                if insert_idx == prev_idx + 1:
                    final_lines.append('}\n')
                    final_lines.append('\n')
    
    final_lines.append(line)

# Make sure file ends with a closing brace if needed
if final_lines and not final_lines[-1].strip().endswith('}'):
    # Check if last function needs closing
    for i in range(len(final_lines) - 1, -1, -1):
        if func_pattern.match(final_lines[i].strip()):
            final_lines.append('}\n')
            break

# Write the fixed file
with open('Source/Core/PluginProcessor.cpp.fixed', 'w', encoding='utf-8') as f:
    f.writelines(final_lines)

print(f"Fixed file written to Source/Core/PluginProcessor.cpp.fixed")
print(f"Original lines: {len(lines)}")
print(f"Fixed lines: {len(final_lines)}")