#!/usr/bin/env python3

import subprocess
import sys

def is_docker_installed():
    try:
        result = subprocess.run(['docker', '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode == 0:
            return True, result.stdout.strip()
        else:
            return False, result.stderr.strip()
    except FileNotFoundError:
        return False, "Docker CLI not found."

def is_docker_daemon_running():
    try:
        result = subprocess.run(['docker', 'info'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode == 0:
            return True, "Docker daemon is running."
        else:
            return False, result.stderr.strip()
    except FileNotFoundError:
        return False, "Docker CLI not found."

def main():
    installed, install_message = is_docker_installed()
    if not installed:
        print(f"Docker is not installed: {install_message}")
        sys.exit(1)
    else:
        print(f"Docker is installed: {install_message}")

    running, daemon_message = is_docker_daemon_running()
    if not running:
        print(f"Docker daemon is not running: {daemon_message}")
        sys.exit(1)
    else:
        print("Docker daemon is running.")

if __name__ == "__main__":
    main()
