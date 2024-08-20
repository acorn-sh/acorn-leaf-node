import subprocess
import sys
import re

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
            if len(parts) >= 6:  # Adjusted to account for PORTS
                container_id = parts[0]
                image = parts[1]
                ports = parts[-1] if ':' in parts[-1] else ''
                status = parts[-2] if ':' in parts[-1] else parts[-1]
                uptime = parts[4]  # Uptime information
                containers.append({'container_id': container_id, 'image': image, 'status': status, 'uptime': uptime, 'ports': ports})

        return containers

    except FileNotFoundError as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

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
                cpu = parts[2]  # Adjusted index for CPU %
                mem_usage = parts[3]  # Adjusted index for MEM USAGE / LIMIT
                net_io = parts[6]  # NET I/O field
                stats.append({'container_id': container_id, 'cpu': cpu, 'mem_usage': mem_usage, 'net_io': net_io})

        return stats

    except FileNotFoundError as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

def correlate_data(images, containers, stats):
    data = []
    for image in images:
        found_container = False
        for container in containers:
            if image['repository'] in container['image']:
                container_data = {
                    'repository': image['repository'],
                    'image_id': image['image_id'],
                    'container_id': container['container_id'],
                    'status': container['status'],
                    'uptime': container['uptime'],
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
                found_container = True

        if not found_container:
            data.append({
                'repository': image['repository'],
                'image_id': image['image_id'],
                'container_id': '',
                'status': 'Stopped',
                'uptime': '',
                'ports': '',
                'cpu': '',
                'mem_usage': '',
                'net_io': ''
            })

    return data

if __name__ == "__main__":
    images = get_sudoacorn_images()
    containers = get_running_containers()
    stats = get_container_stats()

    data = correlate_data(images, containers, stats)

    for entry in data:
        print(f"{entry['repository']} | {entry['image_id']} | {entry['container_id']} | {entry['status']} | {entry['uptime']} | {entry['cpu']} | {entry['mem_usage']} | {entry['net_io']} | {entry['ports']}")
