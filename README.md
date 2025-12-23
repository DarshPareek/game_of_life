# Conway's Game of Life

A high-performance implementation of the Game of Life using **C++** and **Raylib**.

Instead of drawing thousands of rectangles (which is slow), this project writes cell data directly to a texture and uses a custom **fragment shader** to render the grid lines and cell shapes. This keeps the framerate silky smooth even at high resolutions.

It also features a **Prediction Mode**—a "ghost" overlay that shows you exactly what the next generation will look like while you are still editing.

## How to Build

You'll need a C++ compiler and Raylib installed.

```bash
g++ main.cpp -o game -lraylib -GL -lm -lpthread -ldl -lrt -lX11
./game

```

## Controls

### General

| Key | Action |
| --- | --- |
| **Space** | Toggle between **Edit** and **Run** mode |
| **Scroll** | Zoom In / Out |
| **Shift + Drag** | Pan Camera |
| **S** | Toggle Bloom Shader |
| **Q** | Quit |

### Edit Mode

| Key | Action |
| --- | --- |
| **Left Click** | Draw Cells |
| **Right Click** | Erase Cells |
| **Enter** | Spawn Random Cells |
| **R** | Clear Grid |
| **T** | Toggle **Prediction Overlay** (See the future!) |

### Run Mode

| Key | Action |
| --- | --- |
| **Up Arrow** | Increase Speed |
| **Down Arrow** | Decrease Speed |

### Shader Tweaking

*You can tweak the visual look in real-time:*

* **G + Up/Down**: Adjust grid gap thickness.
* **R + Up/Down**: Adjust cell radius.
* **B + S + Up/Down**: Bloom samples.
