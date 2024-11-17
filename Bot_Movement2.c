// Square Loop Movement
#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define MOVE_INTERVAL 100000    // 100 ms interval between movements
#define DURATION 10            // Duration to run the bot movement in seconds
#define STEP_SIZE 50           // Number of pixels to move in each step

// Bot movement patterns: each direction is either horizontal or vertical
int directions[4][2] = {
    {1, 0},  // Right
    {0, 1},  // Down
    {-1, 0}, // Left
    {0, -1}  // Up
};

int main() {
    Display *display;
    Window root;
    int x = 500, y = 500;  // Starting position for the cursor
    struct timespec start, now;
    int direction_index = 0;

    // Open display
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }
    root = DefaultRootWindow(display);

    // Get the start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (1) {
        // Calculate new position based on the current direction
        x += STEP_SIZE * directions[direction_index][0];
        y += STEP_SIZE * directions[direction_index][1];

        // Move cursor to the new position
        XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
        XFlush(display);

        // Sleep for the defined interval
        usleep(MOVE_INTERVAL);

        // Every 5 steps, make a sharp 90-degree turn
        if ((x / STEP_SIZE + y / STEP_SIZE) % 5 == 0) {
            direction_index = (direction_index + 1) % 4;
        }

        // Check elapsed time
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed_time = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
        if (elapsed_time >= DURATION) break;
    }

    XCloseDisplay(display);
    return 0;
}
