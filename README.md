# Humanoid-Detection-Using-Cursor-Movements

## Overview

This project implements a **Bot Movement Detector** that monitors mouse movements on a computer and analyzes the movement patterns to detect bot-like behavior. It uses **C** with **Xlib** for mouse tracking on X11 systems, and **pthreads** for concurrency. The detection is based on analyzing movement velocity, direction, and straight-line patterns.

## Components

### 1. **Detector.c**
The primary program that tracks mouse movement and detects bot activity. Key features include:
- Tracks the cursor's movement for **10 seconds**.
- Calculates **velocity**, **angular direction**, and **directional changes**.
- Detects straight-line intervals and velocity variance.
- Uses thresholds to classify behavior as **human** or **bot**.
- Multi-threaded implementation using `pthread` to perform movement tracking and bot detection concurrently.

### 2. **Bot Movement Simulations**
Three programs simulate bot-like mouse movements:
- **Bot_Movement1.c**: Implements a zigzag pattern with adjustable step sizes and intervals.
- **Bot_Movement2.c**: Moves in a square pattern with sharp turns after every few steps.
- **Bot_Movement3.c**: Moves in a modified straight pattern with slight variations.

### 3. **Detect_Bot.sh**
A bash script to run the **Detector** program along with one of the simulated bot programs. It orchestrates the detection process and allows switching between different bot movement simulations.

---

## How It Works

1. **Tracking**:
   - The detector program queries the current cursor position using **XQueryPointer**.
   - Captures data at **intervals of 50-100ms** to measure movement metrics.
   
2. **Metrics Calculation**:
   - **Velocity**: Measures pixels moved per second.
   - **Angular Difference**: Tracks changes in movement direction.
   - **Straight-Line Detection**: Counts intervals where movement is close to a straight line.

3. **Bot Detection**:
   - Uses the following criteria:
     - **Straight-Line Count**: High counts suggest automated movement.
     - **Velocity Variance**: Low variance indicates consistent, bot-like movement.

4. **Simulations**:
   - Simulated bots generate controlled movement patterns to test the detector.

---

## Installation

### Prerequisites
- A Linux system with X11 installed.
- GCC compiler.
- **pthread** library (should come pre-installed on most Linux systems).

### Build Instructions
1. Clone the repository:
   ```bash
   git clone <repository_url>
   cd bot-movement-detector
   ```
2. Compile all programs:
   ```bash
   gcc Detector.c -o Detector -lm -lX11 -lpthread
   gcc Bot_Movement1.c -o Bot_Movement1 -lX11
   gcc Bot_Movement2.c -o Bot_Movement2 -lX11
   gcc Bot_Movement3.c -o Bot_Movement3 -lX11
   ```
3. Make the shell script executable:
   ```bash
   chmod +x Detect_Bot.sh
   ```

---

## Usage

### Run the Detector with Simulations
1. Execute the shell script to start detection and bot simulation:
   ```bash
   ./Detect_Bot.sh
   ```
   By default, **Bot_Movement2** is enabled. Uncomment other simulations in the script to test different patterns.

### Run the Programs Independently
- Run the **Detector**:
  ```bash
  ./Detector
  ```
- Run a bot movement simulation:
  ```bash
  ./Bot_Movement1
  ```

---

## Configuration

- Adjust detection parameters in `Detector.c`:
  - `MIN_MOVEMENT_DISTANCE`: Minimum pixels considered as movement.
  - `TRACK_DURATION`: Duration for tracking (in seconds).
  - `STRAIGHT_LINE_THRESHOLD`: Tolerance for detecting straight-line movement.
  - `VELOCITY_VARIANCE_THRESHOLD`: Variance threshold for bot detection.
  - `TRACK_INTERVAL_US`: Interval between position checks (in microseconds).

- Modify bot simulation parameters in `Bot_Movement*.c`:
  - `STEP_SIZE`: Movement step size.
  - `MOVE_INTERVAL`: Delay between steps.
  - `DURATION`: Total duration of movement simulation.

---

## Limitations
- Designed for X11 systems; not compatible with Wayland or non-Linux systems.
- Thresholds may need tuning for different environments or movement patterns.

---

