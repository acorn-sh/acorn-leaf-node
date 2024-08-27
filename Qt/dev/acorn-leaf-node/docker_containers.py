import subprocess
import sys
import re
import json
from datetime import datetime

# Retrieves information about Sudoacorn images with all fields
# Example terminal usage: python script_name.py -c get_sudoacorn_images
# Example output: [{"repository": "sudoacorn/image1", "tag": "latest", "image_id": "abc123", "created": "2 days ago", "size": "500MB"}]
def get_sudoacorn_images():
    try:
        result = subprocess.run(
            ['docker', 'images', '--format', '{{.Repository}}|{{.Tag}}|{{.ID}}|{{.CreatedSince}}|{{.Size}}'],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        if result.returncode != 0:
            print(f"Error listing Docker images: {result.stderr}", file=sys.stderr)
            return []

        images = []
        lines = result.stdout.splitlines()
        for line in lines:
            if re.search(r'^sudoacorn/', line):
                parts = line.split("|")
                if len(parts) == 5:
                    images.append({
                        'repository': parts[0].strip(),
                        'tag': parts[1].strip(),
                        'image_id': parts[2].strip(),
                        'created': parts[3].strip(),
                        'size': parts[4].strip()
                    })

        return images

    except FileNotFoundError as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        return []

# Retrieves information about running and stopped containers with all fields
# Example terminal usage: python script_name.py -c get_running_containers
# Example output: [{"container_id": "abc123", "image": "sudoacorn/image1", "command": "/bin/sh", "created": "2023-08-27 10:00:00", "status": "Up 5 minutes", "ports": "80/tcp", "name": "my_container"}]
def get_running_containers():
    try:
        result = subprocess.run(
            ['docker', 'ps', '-a', '--format', '{{.ID}}|{{.Image}}|{{.Command}}|{{.CreatedAt}}|{{.Status}}|{{.Ports}}|{{.Names}}'],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        if result.returncode != 0:
            print(f"Error listing containers: {result.stderr}", file=sys.stderr)
            return []

        containers = []
        lines = result.stdout.splitlines()
        for line in lines:
            parts = line.split("|")
            if len(parts) == 7:  # Ensure all fields are captured
                containers.append({
                    'container_id': parts[0].strip(),
                    'image': parts[1].strip(),
                    'command': parts[2].strip(),
                    'created': parts[3].strip(),
                    'status': parts[4].strip(),
                    'ports': parts[5].strip(),
                    'name': parts[6].strip()
                })

        return containers

    except FileNotFoundError as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        return []

# Retrieves stats for all running containers with all fields
# Example terminal usage: python script_name.py -c get_container_stats
# Example output: [{"container_id": "abc123", "name": "my_container", "cpu": "0.25%", "mem_usage": "200MiB", "mem_limit": "2GiB", "mem_perc": "10.0%", "net_io": "1MB / 1MB", "block_io": "0B / 0B", "pids": "3"}]
def get_container_stats():
    try:
        result = subprocess.run(
            ['docker', 'container', 'stats', '--no-stream', '--format', '{{.Container}}|{{.Name}}|{{.CPUPerc}}|{{.MemUsage}}|{{.MemPerc}}|{{.NetIO}}|{{.BlockIO}}|{{.PIDs}}'],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        if result.returncode != 0:
            print(f"Error retrieving container stats: {result.stderr}", file=sys.stderr)
            return []

        stats = []
        lines = result.stdout.splitlines()
        for line in lines:
            parts = line.split("|")
            if len(parts) == 8:  # Ensure all stats fields are captured
                mem_usage_split = parts[3].split(" / ")
                stats.append({
                    'container_id': parts[0].strip(),
                    'name': parts[1].strip(),
                    'cpu': parts[2].strip(),
                    'mem_usage': mem_usage_split[0].strip(),
                    'mem_limit': mem_usage_split[1].strip() if len(mem_usage_split) > 1 else "",
                    'mem_perc': parts[4].strip(),
                    'net_io': parts[5].strip(),
                    'block_io': parts[6].strip(),
                    'pids': parts[7].strip()
                })

        return stats

    except FileNotFoundError as e:
        print(f"Error: {str(e)}", file.sys.stderr)
        return []

# Retrieves the creation time of a specific container by its ID
# Example terminal usage: python script_name.py -c get_container_creation_time abc123
# Example output: {"container_id": "abc123", "creation_time": "2023-08-27 10:00:00"}
def get_container_creation_time(container_id):
    try:
        result = subprocess.run(
            ['docker', 'inspect', container_id],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        if result.returncode != 0:
            print(f"Error inspecting container {container_id}: {result.stderr}", file=sys.stderr)
            return "N/A"

        container_info = json.loads(result.stdout)
        creation_time_str = container_info[0]["State"]["StartedAt"]

        # Use regex to remove fractional seconds before the timezone info
        creation_time_str = re.sub(r'\.\d+', '', creation_time_str)

        # Parse the cleaned string into a datetime object
        creation_time = datetime.fromisoformat(creation_time_str.replace('Z', '+00:00'))

        # Format the creation time as "YYYY-MM-DD HH:MM:SS"
        formatted_time = creation_time.strftime("%Y-%m-%d %H:%M:%S")
        return formatted_time

    except Exception as e:
        print(f"Failed to get creation time for container {container_id}: {e}", file=sys.stderr)
        return "N/A"

if __name__ == "__main__":
    if '-c' in sys.argv:
        command = sys.argv[sys.argv.index('-c') + 1]

        if command == 'get_running_containers':
            containers = get_running_containers()
            print(json.dumps(containers))  # Serialize as JSON
        elif command == 'get_sudoacorn_images':
            images = get_sudoacorn_images()
            print(json.dumps(images))  # Serialize as JSON
        elif command == 'get_container_stats':
            stats = get_container_stats()
            print(json.dumps(stats))  # Serialize as JSON
        elif command == 'get_container_creation_time':
            container_id = sys.argv[sys.argv.index('-c') + 2]  # Assume container_id is passed after the command
            creation_time = get_container_creation_time(container_id)
            print(json.dumps({"container_id": container_id, "creation_time": creation_time}))  # Serialize as JSON
        else:
            print("Invalid command")
    else:
        print("No command specified. Use -c <command_name> to retrieve data.")
