#!/usr/bin/env python3

import subprocess
import sys
import re
import json

# Retrieves information about Sudoacorn repositories on Docker Hub
# Example terminal usage: python script_name.py -c get_sudoacorn_repos
# Example output: [{"name": "repo1", "link": "https://hub.docker.com/r/sudoacorn/repo1", "description": "Description of repo1", "stars": 10}]
def get_sudoacorn_repos():
    command = [
        'curl', '-s',
        'https://hub.docker.com/v2/repositories/sudoacorn/?page_size=100'
    ]
    try:
        result = subprocess.run(
            command,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )

        if result.returncode != 0:
            print(f"Error fetching repositories: {result.stderr}", file=sys.stderr)
            return []

        data = json.loads(result.stdout)
        repos = []

        for repo in data['results']:
            repos.append({
                'name': repo['name'],
                'link': f"https://hub.docker.com/r/sudoacorn/{repo['name']}",
                'description': repo['description'],
                'stars': repo['star_count']
            })

        return repos

    except FileNotFoundError as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        return []

# Extracts repository name from a Docker Hub URL
# Example terminal usage: python script_name.py -c extract_repository_name https://hub.docker.com/r/sudoacorn/repo1
# Example output: sudoacorn/repo1
def extract_repository_name(url):
    match = re.search(r'hub\.docker\.com/r/([^/]+/[^/]+)', url)
    if match:
        return match.group(1)
    else:
        return None

# Runs a Docker command and returns success status and output
# Example terminal usage: python script_name.py -c run_docker_command "docker images"
# Example output: True, "<list of docker images>"
def run_docker_command(command):
    try:
        result = subprocess.run(
            command,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )

        if result.returncode == 0:
            return True, result.stdout.strip()
        else:
            return False, result.stderr.strip()

    except FileNotFoundError as e:
        return False, str(e)

# Retrieves the image ID for a given repository name
# Example terminal usage: python script_name.py -c get_image_id sudoacorn/repo1
# Example output: abc123
def get_image_id(repository):
    try:
        # Run the Docker command to list images
        command = ['docker', 'images', '--format', '{{.Repository}}:{{.Tag}}|{{.ID}}']
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        if result.returncode != 0:
            print(f"Error fetching image ID: {result.stderr}", file=sys.stderr)
            return ""

        # Split the output by lines
        lines = result.stdout.splitlines()
        for line in lines:
            # Split each line into the repository:tag and image ID
            repo_tag, image_id = line.split('|')
            if repo_tag.startswith(repository):
                # Return the image ID if the repository matches
                return image_id.strip()

        return ""

    except FileNotFoundError as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        return ""

# Deletes a Docker image
# Example terminal usage: python script_name.py -c delete https://hub.docker.com/r/sudoacorn/repo1
# Example output: Successfully deleted sudoacorn/repo1
def delete_docker_image(repository):
    success, output = run_docker_command(
        ['docker', 'ps', '-a', '--filter', f'ancestor={repository}', '--format', '{{.ID}}']
    )
    if success:
        container_ids = output.splitlines()
        for container_id in container_ids:
            success, error = run_docker_command(['docker', 'stop', container_id])
            if not success:
                print(f"Error stopping container {container_id}: {error}", file=sys.stderr)
                continue

            success, error = run_docker_command(['docker', 'rm', container_id])
            if not success:
                print(f"Error removing container {container_id}: {error}", file=sys.stderr)
                continue

    success, error = run_docker_command(['docker', 'rmi', repository])
    if success:
        print(f"Successfully deleted {repository}")
    else:
        print(f"Error deleting Docker image: {error}", file=sys.stderr)

# Pulls a Docker image
# Example terminal usage: python script_name.py -c pull https://hub.docker.com/r/sudoacorn/repo1
# Example output: Successfully pulled sudoacorn/repo1
def pull_docker_image(repository):
    success, error = run_docker_command(['docker', 'pull', repository])
    if success:
        print(f"Successfully pulled {repository}")
    else:
        print(f"Error pulling Docker image: {error}", file=sys.stderr)

# Starts a Docker container
# Example terminal usage: python script_name.py -c start sudoacorn/repo1
# Example output: Successfully started container from sudoacorn/repo1
def start_docker_container(repository):
    success, error = run_docker_command(['docker', 'run', '-d', repository])
    if success:
        print(f"Successfully started container from {repository}")
    else:
        print(f"Error starting container from {repository}: {error}", file=sys.stderr)

# Main function to handle commands
if __name__ == "__main__":
    if '-c' in sys.argv:
        command = sys.argv[sys.argv.index('-c') + 1]

        if command == 'get_sudoacorn_repos':
            repos = get_sudoacorn_repos()
            print(json.dumps(repos))  # Serialize as JSON
        elif command == 'pull':
            if len(sys.argv) > sys.argv.index('-c') + 2:
                url = sys.argv[sys.argv.index('-c') + 2]
                repository = extract_repository_name(url)
                if repository:
                    pull_docker_image(repository)
                else:
                    print(f"Failed to extract repository name from {url}", file=sys.stderr)
            else:
                print("No URL provided for pull command", file=sys.stderr)
        elif command == 'delete':
            if len(sys.argv) > sys.argv.index('-c') + 2:
                url = sys.argv[sys.argv.index('-c') + 2]
                repository = extract_repository_name(url)
                if repository:
                    delete_docker_image(repository)
                else:
                    print(f"Failed to extract repository name from {url}", file=sys.stderr)
            else:
                print("No URL provided for delete command", file.sys.stderr)
        elif command == 'start':
            if len(sys.argv) > sys.argv.index('-c') + 2:
                repository = sys.argv[sys.argv.index('-c') + 2]
                start_docker_container(repository)
            else:
                print("No repository name provided for start command", file.sys.stderr)
        elif command == 'get_image_id':
            if len(sys.argv) > sys.argv.index('-c') + 2:
                repository = sys.argv[sys.argv.index('-c') + 2]
                image_id = get_image_id(repository)
                print(image_id)
            else:
                print("No repository name provided for get_image_id command", file.sys.stderr)
        else:
            print("Invalid command", file.sys.stderr)
    else:
        print("No command specified. Use -c <command_name> to retrieve data.", file.sys.stderr)
