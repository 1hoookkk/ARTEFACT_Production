import sys


def main():
    if len(sys.argv) < 4:
        print("Usage: python scripts/print_segment.py <file> <start_line> <end_line>")
        sys.exit(1)

    path = sys.argv[1]
    start = int(sys.argv[2])
    end = int(sys.argv[3])

    # Ensure sensible bounds
    if start < 1 or end < start:
        print("Invalid range")
        sys.exit(1)

    try:
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            for i, line in enumerate(f, start=1):
                if i < start:
                    continue
                if i > end:
                    break
                # Print without adding extra newlines
                sys.stdout.write(line)
    except FileNotFoundError:
        print(f"File not found: {path}")
        sys.exit(1)


if __name__ == "__main__":
    main()

