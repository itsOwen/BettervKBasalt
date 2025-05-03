# BettervkBasalt (Under developement Not ready)
BettervkBasalt is a fork of vkBasalt is a Vulkan post processing layer to enhance the visual graphics of games, now with a full ImGui-based runtime configuration interface similar to ReShade.

## Features

### Built-in Effects
- Contrast Adaptive Sharpening (CAS)
- Denoised Luma Sharpening (DLS)
- Fast Approximate Anti-Aliasing (FXAA)
- Enhanced Subpixel Morphological Anti-Aliasing (SMAA)
- 3D color LookUp Table (LUT)
- ReShade FX shader support

### New: Runtime Configuration with ImGui
- **Real-time effect management**: Add, remove, and reorder effects without restarting
- **Live parameter adjustment**: Modify effect settings with immediate visual feedback
- **Preset system**: Save and load effect configurations
- **User-friendly interface**: Press Insert key to toggle the configuration menu
- **Similar to ReShade**: Familiar interface for ReShade users

## Disclaimer
This is one of my first projects ever, so expect it to have bugs. Use it at your own risk.

## Building from Source

### Dependencies
Before building, you will need:
- GCC >= 9 (with C++17 support)
- X11 development files
- glslang
- SPIR-V Headers
- Vulkan Headers
- meson build system
- ninja build tool

### Additional Dependencies for ImGui Support
- Dear ImGui (automatically downloaded by setup script)
- C++17 filesystem support

### Building

**These instructions use `--prefix=/usr`, which is generally not recommened since vkBasalt will be installed in directories that are meant for the package manager. The alternative is not setting the prefix, it will then be installed in `/usr/local`. But you need to make sure that `ld` finds the library since /usr/local is very likely not in the default path.** 

In general, prefer using distro provided packages.

```
git clone https://github.com/DadSchoorse/vkBasalt.git
cd vkBasalt

# Set up ImGui (required for the new UI)
chmod +x setup_imgui.sh
./setup_imgui.sh
```

#### 64bit

```
meson setup --buildtype=release --prefix=/usr builddir
ninja -C builddir install
```
#### 32bit

Make sure that `PKG_CONFIG_PATH=/usr/lib32/pkgconfig` and `--libdir=lib32` are correct for your distro and change them if needed. On Debian based distros you need to replace `lib32` with `lib/i386-linux-gnu`, for example.
```
ASFLAGS=--32 CFLAGS=-m32 CXXFLAGS=-m32 PKG_CONFIG_PATH=/usr/lib32/pkgconfig meson setup --prefix=/usr --buildtype=release --libdir=lib32 -Dwith_json=false builddir.32
ninja -C builddir.32 install
```

## Packaging status

[Debian](https://tracker.debian.org/pkg/vkbasalt) `sudo apt install vkbasalt`

[Fedora](https://src.fedoraproject.org/rpms/vkBasalt) `sudo dnf install vkBasalt`

[Void Linux](https://github.com/void-linux/void-packages/blob/master/srcpkgs/vkBasalt/template) `sudo xbps-install vkBasalt`

## Usage

### Quick Start
1. Enable vkBasalt: `ENABLE_VKBASALT=1 yourgame`
2. Press **Insert** key in-game to open the configuration menu
3. Use the menu to add/remove effects and adjust settings in real-time
4. Save your configuration as a preset for future use

### Environment Variables
Basic usage with environment variable:
```bash
ENABLE_VKBASALT=1 yourgame
```

With debug output:
```bash
VKBASALT_LOG_LEVEL=debug ENABLE_VKBASALT=1 yourgame
```

### Lutris
With Lutris, follow these steps below:
1. Right click on a game, and press `configure`.
2. Go to the `System options` tab and scroll down to `Environment variables`.
3. Press on `Add`, and add `ENABLE_VKBASALT` under `Key`, and add `1` under `Value`.

### Steam
With Steam, edit your launch options and add:
```ini
ENABLE_VKBASALT=1 %command% 
```

## Configuration

### Runtime Configuration (New!)
With the ImGui integration, you can now configure vkBasalt in real-time:
1. Press **Insert** key to open the menu
2. Navigate through the tabs:
   - **Effects**: Add, remove, reorder, and configure effects
   - **General Settings**: Configure global options
   - **Presets**: Save and load effect configurations
3. Changes are applied immediately

### Static Configuration
Traditional configuration files are still supported and will be loaded at startup.
The config file will be searched for in the following locations:
* a file set with the environment variable`VKBASALT_CONFIG_FILE=/path/to/vkBasalt.conf`
* `vkBasalt.conf` in the working directory of the game
* `$XDG_CONFIG_HOME/vkBasalt/vkBasalt.conf` or `~/.config/vkBasalt/vkBasalt.conf` if `XDG_CONFIG_HOME` is not set
* `$XDG_DATA_HOME/vkBasalt/vkBasalt.conf` or `~/.local/share/vkBasalt/vkBasalt.conf` if `XDG_DATA_HOME` is not set
* `/etc/vkBasalt.conf`
* `/etc/vkBasalt/vkBasalt.conf`
* `/usr/share/vkBasalt/vkBasalt.conf`

### Preset System
Presets are saved in `~/.config/vkBasalt/presets/` and can be:
- Created through the UI
- Shared between users
- Loaded per-game or globally

#### Reshade Fx shaders

To run reshade fx shaders e.g. shaders from the [reshade repo](https://github.com/crosire/reshade-shaders), you have to set `reshadeTexturePath` and `reshadeIncludePath` to the matching dirctories from the repo. To then use a specific shader you need to set a custom effect name to the shader path and then add that effect name to `effects` like every other effect.

```ini
effects = colorfulness:denoise

colorfulness = /home/user/reshade-shaders/Shaders/Colourfulness.fx
denoise = /home/user/reshade-shaders/Shaders/Denoise.fx
reshadeTexturePath = /home/user/reshade-shaders/Textures
reshadeIncludePath = /home/user/reshade-shaders/Shaders
```

### Keyboard Shortcuts

| Key | Function |
|-----|----------|
| Insert | Toggle configuration menu |
| Home | Toggle effects on/off |

Both keys can be customized in the configuration menu or config file.

**Note**: Keyboard input is based on X11 and won't work on pure Wayland. It should not crash without X11.


#### Debug Output

The amount of debug output can be set with the `VKBASALT_LOG_LEVEL` env var, e.g. `VKBASALT_LOG_LEVEL=debug`. Possible values are: `trace, debug, info, warn, error, none`.

By default the logger outputs to stderr, a file as output location can be set with the `VKBASALT_LOG_FILE` env var, e.g. `VKBASALT_LOG_FILE="vkBasalt.log"`.


## FAQ

#### Why is it called vkBasalt?
It's a joke: vulkan post processing → after vulcan → basalt

#### Does vkBasalt work with DXVK and VKD3D?
Yes, vkBasalt works with both DXVK (DirectX 9/10/11 to Vulkan) and VKD3D (DirectX 12 to Vulkan).

#### Will vkBasalt get me banned?
Maybe. To my knowledge this hasn't happened yet but don't blame me if your frog dies.

#### Will there be an OpenGL version?
No. OpenGL has no layer system like Vulkan.

#### Does vkBasalt have a GUI now?
Yes! Press Insert key in-game to access the full configuration interface.

#### How does vkBasalt compare to ReShade?
vkBasalt now offers a similar experience to ReShade with:
- Runtime effect configuration
- Live parameter adjustment
- Preset system
- ReShade FX shader support

However, vkBasalt is Vulkan-specific and may have different performance characteristics.

#### Can I use my ReShade presets?
ReShade FX shaders are supported, but preset formats are not directly compatible. You'll need to recreate your presets using the vkBasalt interface.
#### Does every reshade shader work?
No. Shaders that need multiple techniques do not work, there might still be problems with stencil and blending and depth buffer access isn't ready yet.
#### Does depth buffer access work?
There is a WIP version that you can enable with `depthCapture = on`. It may lead to problems especially on non-NVIDIA hardware. The selected depth buffer isn't always the one you would want.

#### Can I change settings for ReShade shaders?
Yes! With the new ImGui interface, you can adjust ReShade shader parameters in real-time, just like in ReShade itself.

#### How do I report issues?
Please include:
1. Your system specifications
2. The game you're testing
3. vkBasalt version
4. Debug log (`VKBASALT_LOG_LEVEL=debug`)
5. Steps to reproduce the issue
