//Zig - Zag Motion

#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define MOVE_INTERVAL 50000     // 50 ms interval between movements
#define DURATION 10             // Duration to run the bot movement in seconds
#define STEP_SIZE 50            // Step size for each movement
#define ZIGZAG_COUNT 10         // Number of steps in each zig or zag

int main() {
    Display *display;
    Window root;
    struct timespec start, now;
    int x = 100, y = 100;  // Starting position
    int direction = 1;     // Direction toggler for zigzag

    // Open display
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }
    root = DefaultRootWindow(display);

    // Get the start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    int zigzag_counter = 0;

    while (1) {
        // Move in a zigzag pattern
        x += STEP_SIZE * direction;
        zigzag_counter++;

        if (zigzag_counter >= ZIGZAG_COUNT) {
            // After a zig or zag, change the vertical position and toggle direction
            y += STEP_SIZE;
            direction *= -1;
            zigzag_counter = 0;
        }

        // Move cursor to the new position
        XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
        XFlush(display);

        // Sleep for the defined interval
        usleep(MOVE_INTERVAL);

        // Check elapsed time
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed_time = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
        if (elapsed_time >= DURATION) break;
    }

    XCloseDisplay(display);
    return 0;
    
}
