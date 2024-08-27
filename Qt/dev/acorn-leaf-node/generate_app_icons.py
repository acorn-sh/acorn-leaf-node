import os
from PIL import Image
import subprocess
import sys

def get_logo_paths():
    return "/Users/platon1/DEV/GITHUB/ACORN/acorn-leaf-node/Qt/dev/acorn-leaf-node/images/acorn_sh_logo.png"  # Replace with your logo path function

def generate_mac_icons(src_image_path, output_dir):
    iconset_dir = os.path.join(output_dir, "AppIcon.iconset")
    os.makedirs(iconset_dir, exist_ok=True)

    sizes = [16, 32, 64, 128, 256, 512, 1024]

    for size in sizes:
        img = Image.open(src_image_path)
        img = img.resize((size, size), Image.ANTIALIAS)
        img.save(os.path.join(iconset_dir, f"icon_{size}x{size}.png"))

    icns_path = os.path.join(output_dir, "app.icns")
    subprocess.run(["iconutil", "-c", "icns", iconset_dir, "-o", icns_path])

def generate_windows_icons(src_image_path, output_dir):
    sizes = [16, 32, 48, 64, 128, 256]

    icon_images = []
    for size in sizes:
        img = Image.open(src_image_path)
        img = img.resize((size, size), Image.ANTIALIAS)
        icon_images.append(img)

    ico_path = os.path.join(output_dir, "app.ico")
    icon_images[0].save(ico_path, format="ICO", sizes=[(size, size) for size in sizes])

def generate_linux_icons(src_image_path, output_dir):
    sizes = [16, 24, 32, 48, 64, 128, 256, 512]

    for size in sizes:
        img = Image.open(src_image_path)
        img = img.resize((size, size), Image.ANTIALIAS)
        img_dir = os.path.join(output_dir, f"{size}x{size}/apps")
        os.makedirs(img_dir, exist_ok=True)
        img.save(os.path.join(img_dir, "app.png"))

def main():
    logo_path = get_logo_paths()
    if not os.path.exists(logo_path):
        print(f"Logo file not found: {logo_path}")
        sys.exit(1)

    output_dir = "output_icons"
    os.makedirs(output_dir, exist_ok=True)

    generate_mac_icons(logo_path, os.path.join(output_dir, "macos"))
    generate_windows_icons(logo_path, os.path.join(output_dir, "windows"))
    generate_linux_icons(logo_path, os.path.join(output_dir, "linux"))

    print("Icon generation completed!")

if __name__ == "__main__":
    main()
