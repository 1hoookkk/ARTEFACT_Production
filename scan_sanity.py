import os
import sys

# --- CONFIGURATION ---
# The root directory of your source code.
SOURCE_ROOT = "Source"
# --- END CONFIGURATION ---

class SanityScanner:
    def __init__(self, root_dir):
        self.root_dir = root_dir
        self.missing_headers = []

    def run(self):
        print("--- Sanity Scan Report ---")
        print(f"Scanning directory: '{os.path.abspath(self.root_dir)}'...")
        
        if not os.path.isdir(self.root_dir):
            print(f"\nFATAL ERROR: Source directory not found at '{os.path.abspath(self.root_dir)}'")
            print("Please ensure you are running this script from your project root.")
            return False

        for dirpath, _, filenames in os.walk(self.root_dir):
            for filename in filenames:
                if filename.endswith(".cpp"):
                    cpp_path = os.path.join(dirpath, filename)
                    header_name = filename.replace(".cpp", ".h")
                    header_path = os.path.join(dirpath, header_name)
                    
                    if not os.path.exists(header_path):
                        self.missing_headers.append(cpp_path)
        
        self.print_report()
        return not self.missing_headers

    def print_report(self):
        if not self.missing_headers:
            print("\n✅ SUCCESS: Project sanity check passed. All .cpp files have corresponding .h files.")
            return

        print(f"\n❌ FAILURE: Found {len(self.missing_headers)} implementation files with missing headers.")
        print("---------------------------------------------------------------------")
        for cpp_path in self.missing_headers:
            header_name = os.path.basename(cpp_path).replace(".cpp", ".h")
            print(f"CRITICAL: Missing header file '{header_name}' for:\n  -> {os.path.abspath(cpp_path)}\n")
        print("---------------------------------------------------------------------")

if __name__ == "__main__":
    scanner = SanityScanner(SOURCE_ROOT)
    is_sane = scanner.run()
    
    if not is_sane:
        print("RECOMMENDATION:")
        print("1. Restore the missing .h files from your archive (`_archive` folder).")
        print("2. OR, remove the corresponding .cpp files from your `CMakeLists.txt` `target_sources` list.")
        print("The build cannot succeed until the source tree is consistent.")
        sys.exit(1)
    else:
        sys.exit(0)