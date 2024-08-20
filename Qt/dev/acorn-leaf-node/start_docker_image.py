import subprocess
import sys
import re

def get_container_id_by_image(image_name):
    try:
        print(f"Finding container ID for image: {image_name}")
        result = subprocess.run(['docker', 'ps', '-a', '--filter', f'ancestor={image_name}', '--format', '{{.ID}}'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode == 0 and result.stdout.strip():
            container_id = result.stdout.strip().split('\n')[0]
            print(f"Found container ID: {container_id}")
            return container_id
        else:
            print(f"Error: No container found for image {image_name}. {result.stderr}")
            return None
    except FileNotFoundError as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

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
        print("Usage: python3 start_docker_image.py <image_name> <command>")
        sys.exit(1)

    image_name = sys.argv[1]
    command = sys.argv[2].lower()

    if command not in ["start", "stop", "pause", "restart"]:
        print(f"Invalid command: {command}. Valid commands are: start, stop, pause, restart.")
        sys.exit(1)

    container_id = get_container_id_by_image(image_name)
    if container_id:
        execute_docker_command(container_id, command)
    else:
        print(f"No container found for image {image_name}.")
