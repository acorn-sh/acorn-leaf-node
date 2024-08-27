import subprocess
import sys

def execute_docker_command(container_id, command):
    try:
        print(f"Executing '{command}' on container with ID: {container_id}")
        result = subprocess.run(['docker', command, container_id], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode == 0:
            print(f"Successfully executed '{command}' on container with ID {container_id}")
        else:
            print(f"Error executing '{command}' on Docker container: {result.stderr}")
    except FileNotFoundError as e:
        print(f"Error: {str(e)}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 docker_commands.py <container_id> <command>")
        sys.exit(1)

    container_id = sys.argv[1]
    command = sys.argv[2].lower()

    if command not in ["start", "stop", "pause", "restart"]:
        print(f"Invalid command: {command}. Valid commands are: start, stop, pause, restart.")
        sys.exit(1)

    execute_docker_command(container_id, command)
