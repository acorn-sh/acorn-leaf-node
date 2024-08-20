#!/usr/bin/env python3

import requests

def fetch_docker_hub_repos():
    url = "https://hub.docker.com/v2/repositories/sudoacorn/"
    response = requests.get(url)
    if response.status_code == 200:
        data = response.json()
        repos = []
        for repo in data['results']:
            name = repo['name']
            link = f"https://hub.docker.com/r/sudoacorn/{name}"
            description = repo['description']
            stars = repo['star_count']
            repos.append((name, link, description, stars))
        return repos
    else:
        print("Failed to retrieve repositories")
        return []

if __name__ == "__main__":
    repos = fetch_docker_hub_repos()
    for repo in repos:
        print(f"Name: {repo[0]}, Link: {repo[1]}, Description: {repo[2]}, Stars: {repo[3]}")
