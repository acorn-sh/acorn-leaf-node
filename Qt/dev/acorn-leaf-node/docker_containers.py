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

def get_container_info(container_id):
    try:
        # Use docker inspect to get detailed information about the container
        result = subprocess.run(
            ['docker', 'inspect', container_id],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        if result.returncode != 0:
            print(f"Error inspecting container {container_id}: {result.stderr}", file=sys.stderr)
            return {}

        # Load the JSON output from docker inspect
        container_info = json.loads(result.stdout)[0]

        # Extract necessary fields from the JSON output
        status = container_info["State"]["Status"]
        running = container_info["State"]["Running"]
        paused = container_info["State"]["Paused"]
        restarting = container_info["State"]["Restarting"]
        created_str = container_info["Created"]
        started_at_str = container_info["State"]["StartedAt"]
        finished_at_str = container_info["State"]["FinishedAt"]
        image = container_info["Image"]

        # Function to format ISO 8601 time strings to "YYYY-MM-DD HH:MM:SS"
        def format_time(iso_time_str):
            if not iso_time_str or iso_time_str == "0001-01-01T00:00:00Z":
                return None
            # Remove fractional seconds and parse the time string
            iso_time_str = re.sub(r'\.\d+', '', iso_time_str)  # Remove fractional seconds
            time_obj = datetime.fromisoformat(iso_time_str.rstrip('Z'))  # Convert to datetime
            return time_obj.strftime("%Y-%m-%d %H:%M:%S")

        # Format the datetime strings
        created = format_time(created_str)
        started_at = format_time(started_at_str)
        finished_at = format_time(finished_at_str) if finished_at_str else None

        # Construct the container details dictionary
        container_details = {
            "container_id": container_id,
            "status": status,
            "running": running,
            "paused": paused,
            "restarting": restarting,
            "created": created,
            "started_at": started_at,
            "finished_at": finished_at,
            "image": image
        }

        return container_details

    except Exception as e:
        print(f"Failed to get container info for container {container_id}: {e}", file=sys.stderr)
        return {}
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
        if command == 'get_container_info':
            container_id = sys.argv[sys.argv.index('-c') + 2]  # Assume container_id is passed after the command
            container_info = get_container_info(container_id)
            print(json.dumps(container_info))  # Serialize as JSON
        else:
            print("Invalid command")
    else:
        print("No command specified. Use -c <command_name> to retrieve data.")
