# .claude/hooks/notification.py
import sys, json, subprocess, shutil, os
root = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
ps = os.path.join(root, "scripts", "notify.ps1")

def main():
    try:
        payload = json.load(sys.stdin) if not sys.stdin.isatty() else {}
    except Exception:
        payload = {}
    title = payload.get("message", payload.get("title", "Claude"))[:120]
    detail = payload.get("detail", "") or payload.get("body", "") or json.dumps(payload)[:800]
    if os.path.exists(ps):
        cmd = ["pwsh", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", ps, "-Title", title, "-Message", detail]
        proc = subprocess.run(cmd, capture_output=True, text=True)
        if proc.returncode == 0:
            print("OK: notification sent")
            sys.exit(0)
        else:
            print("ERR:", proc.returncode, proc.stdout, proc.stderr, file=sys.stderr)
            sys.exit(proc.returncode)
    else:
        # fallback: try to call SnoreToast directly
        snore = os.path.join(root, "tools", "SnoreToast.exe")
        if os.path.exists(snore):
            subprocess.run([snore, "-t", title, "-m", detail, "--silent"])
            print("OK: snoretoast invoked")
            sys.exit(0)
        print("NO-OP: no notifier found; payload:", json.dumps(payload))
        sys.exit(0)

if __name__ == "__main__":
    main()