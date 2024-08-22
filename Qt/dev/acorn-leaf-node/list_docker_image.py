import subprocess
import sys
import re
import json
from datetime import datetime, timezone

# Variables for payment simulation
pay_interval = 60  # seconds
pay_rate = 1  # dollars per interval

def get_sudoacorn_images():
    try:
        result = subprocess.run(['docker', 'images'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print(f"Error listing Docker images: {result.stderr}")
            sys.exit(1)

        images = []
        lines = result.stdout.splitlines()
        for line in lines[1:]:
            if re.search(r'^sudoacorn/', line):
                parts = re.split(r'\s{2,}', line)
                if len(parts) >= 3:
                    repository = parts[0]
                    image_id = parts[2]
                    size = parts[-1]
                    images.append({'repository': repository, 'image_id': image_id, 'size': size})

        return images

    except FileNotFoundError as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

def get_running_containers():
    try:
        result = subprocess.run(['docker', 'ps', '-a'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print(f"Error listing running containers: {result.stderr}")
            sys.exit(1)

        containers = []
        lines = result.stdout.splitlines()
        for line in lines[1:]:
            parts = re.split(r'\s{2,}', line)
            if len(parts) >= 6:
                container_id = parts[0]
                image = parts[1]
                ports = parts[-1] if ':' in parts[-1] else ''
                status = parts[-2] if ':' in parts[-1] else parts[-1]
                containers.append({'container_id': container_id, 'image': image, 'status': status, 'ports': ports})

        return containers

    except FileNotFoundError as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

def get_container_creation_time(container_id):
    try:
        result = subprocess.run(['docker', 'inspect', container_id], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print(f"Error inspecting container {container_id}: {result.stderr}")
            return None

        container_info = json.loads(result.stdout)
        creation_time_str = container_info[0]["State"]["StartedAt"]
        creation_time = datetime.fromisoformat(creation_time_str.replace('Z', '+00:00'))

        return creation_time

    except Exception as e:
        print(f"Failed to get creation time for container {container_id}: {e}")
        return None

def calculate_elapsed_time(creation_time):
    if creation_time is None:
        return "N/A"

    current_time = datetime.now(timezone.utc)
    elapsed_time = current_time - creation_time
    hours, remainder = divmod(elapsed_time.total_seconds(), 3600)
    minutes, seconds = divmod(remainder, 60)

    return f"{int(hours):02}:{int(minutes):02}:{int(seconds):02}"

def get_container_stats():
    try:
        result = subprocess.run(['docker', 'container', 'stats', '--no-stream'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print(f"Error retrieving container stats: {result.stderr}")
            sys.exit(1)

        stats = []
        lines = result.stdout.splitlines()
        for line in lines[1:]:
            parts = re.split(r'\s{2,}', line)
            if len(parts) >= 7:
                container_id = parts[0]
                cpu = parts[2]
                mem_usage = parts[3]
                net_io = parts[6]
                stats.append({'container_id': container_id, 'cpu': cpu, 'mem_usage': mem_usage, 'net_io': net_io})

        return stats

    except FileNotFoundError as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

def correlate_data(images, containers, stats):
    data = []
    for image in images:
        for container in containers:
            if image['repository'] in container['image']:
                if container['status'].lower() == "running":
                    creation_time = get_container_creation_time(container['container_id'])
                    elapsed_time = calculate_elapsed_time(creation_time)
                    payer(elapsed_time)  # Simulate payment for running time
                else:
                    elapsed_time = ""

                container_data = {
                    'repository': image['repository'],
                    'image_id': image['image_id'],
                    'container_id': container['container_id'],
                    'status': container['status'],
                    'creation_time': elapsed_time,
                    'ports': container['ports'],
                    'cpu': '',
                    'mem_usage': '',
                    'net_io': ''
                }
                for stat in stats:
                    if stat['container_id'] == container['container_id']:
                        container_data['cpu'] = stat['cpu']
                        container_data['mem_usage'] = stat['mem_usage']
                        container_data['net_io'] = stat['net_io']
                        break

                data.append(container_data)

    return data

def payer(elapsed_time):
    # Convert elapsed time to total seconds
    h, m, s = map(int, elapsed_time.split(':'))
    total_seconds = h * 3600 + m * 60 + s

    # Calculate number of payments
    payments = total_seconds // pay_interval
    for _ in range(payments):
        print(f"Transaction of ${pay_rate} made for container running {pay_interval} seconds")

if __name__ == "__main__":
    images = get_sudoacorn_images()
    containers = get_running_containers()
    stats = get_container_stats()

    data = correlate_data(images, containers, stats)

    for entry in data:
        print(f"{entry['repository']} | {entry['image_id']} | {entry['container_id']} | {entry['status']} | {entry['creation_time']} | {entry['cpu']} | {entry['mem_usage']} | {entry['net_io']} | {entry['ports']}")
