#!/usr/bin/env python3

import os
import requests
from PIL import Image
import subprocess
import sys
from io import BytesIO

def download_image_from_url(url):
    response = requests.get(url)
    if response.status_code == 200:
        return Image.open(BytesIO(response.content))
    else:
        print(f"Failed to download image from {url}. Status code: {response.status_code}")
        sys.exit(1)

def get_logo_paths():
    return "https://raw.githubusercontent.com/acorn-sh/acorn-leaf-node/acorn_temp_05/Qt/dev/acorn-leaf-node/images/acorn_sh_logo.png?token=GHSAT0AAAAAACVV2M6MSG4K4D4OKINNTR3IZWOMVGQ"  # Replace with your logo URL or local path

def generate_mac_icons(img, output_dir):
    iconset_dir = os.path.join(output_dir, "AppIcon.iconset")
    os.makedirs(iconset_dir, exist_ok=True)

    sizes = [16, 32, 64, 128, 256, 512, 1024]

    for size in sizes:
        resized_img = img.resize((size, size), Image.Resampling.LANCZOS)
        resized_img.save(os.path.join(iconset_dir, f"icon_{size}x{size}.png"))

    icns_path = os.path.join(output_dir, "app.icns")
    subprocess.run(["iconutil", "-c", "icns", iconset_dir, "-o", icns_path])

def generate_windows_icons(img, output_dir):
    sizes = [16, 32, 48, 64, 128, 256]

    icon_images = []
    for size in sizes:
        resized_img = img.resize((size, size), Image.Resampling.LANCZOS)
        icon_images.append(resized_img)

    ico_path = os.path.join(output_dir, "app.ico")
    icon_images[0].save(ico_path, format="ICO", sizes=[(size, size) for size in sizes])

def generate_linux_icons(img, output_dir):
    sizes = [16, 24, 32, 48, 64, 128, 256, 512]

    for size in sizes:
        resized_img = img.resize((size, size), Image.Resampling.LANCZOS)
        img_dir = os.path.join(output_dir, f"{size}x{size}/apps")
        os.makedirs(img_dir, exist_ok=True)
        resized_img.save(os.path.join(img_dir, "app.png"))

def main():
    logo_path = get_logo_paths()

    if logo_path.startswith("http://") or logo_path.startswith("https://"):
        img = download_image_from_url(logo_path)
    elif os.path.exists(logo_path):
        img = Image.open(logo_path)
    else:
        print(f"Logo file not found: {logo_path}")
        sys.exit(1)

    output_dir = "output_icons"
    os.makedirs(output_dir, exist_ok=True)

    generate_mac_icons(img, os.path.join(output_dir, "macos"))
    generate_windows_icons(img, os.path.join(output_dir, "windows"))
    generate_linux_icons(img, os.path.join(output_dir, "linux"))

    print("Icon generation completed!")

if __name__ == "__main__":
    main()
