import subprocess
import sys
import re

def extract_repository_name(url):
    match = re.search(r'hub\.docker\.com/r/([^/]+/[^/]+)', url)
    if match:
        return match.group(1)
    else:
        return None

def pull_docker_image(repository):
    docker_command = '/usr/local/bin/docker'  # Replace with the actual path to docker
    try:
        # Stream the output of the docker pull command
        process = subprocess.Popen(
            [docker_command, 'pull', repository],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        for line in process.stdout:
            print(line.strip())
            # Print progress information to be captured by the parent process

        process.wait()

        if process.returncode == 0:
            print(f"Successfully pulled {repository}")
        else:
            print(f"Error pulling Docker image: {process.stderr.read().strip()}")
    except FileNotFoundError as e:
        print(f"Error pulling Docker image: {str(e)}")
    except Exception as e:
        print(f"An unexpected error occurred: {str(e)}")

def main(url):
    repository = extract_repository_name(url)
    if repository:
        print(f"Extracted repository name: {repository}")
        pull_docker_image(repository)
    else:
        print(f"Failed to extract repository name from {url}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 pull_docker_image.py <docker_hub_url>")
        sys.exit(1)

    url = sys.argv[1]
    main(url)
