import subprocess
import sys

def execute_docker_command(container_id, command, options=None):
    try:
        # Construct the full command with options
        full_command = ['docker', command]
        if options:
            full_command.extend(options)
        full_command.append(container_id)

        print(f"Executing '{' '.join(full_command)}' on container with ID: {container_id}")
        result = subprocess.run(full_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode == 0:
            print(f"Successfully executed '{command}' on container with ID {container_id}")
        else:
            print(f"Error executing '{command}' on Docker container: {result.stderr}")
    except FileNotFoundError as e:
        print(f"Error: {str(e)}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 docker_commands.py <container_id> <command> [options]")
        sys.exit(1)

    container_id = sys.argv[1]
    command = sys.argv[2].lower()
    options = sys.argv[3:]  # Any additional arguments are considered options

    valid_commands = {
        "start": [],
        "stop": [],
        "pause": [],
        "restart": [],
        "rm": ["--force"],
    }

    if command not in valid_commands:
        print(f"Invalid command: {command}. Valid commands are: {', '.join(valid_commands.keys())}.")
        sys.exit(1)

    # Check if the provided options are valid for the command
    if options and not set(options).issubset(valid_commands[command]):
        print(f"Invalid options for command '{command}'. Expected options: {valid_commands[command]}")
        sys.exit(1)

    execute_docker_command(container_id, command, options)
