/*#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/time.h>

#define MIN_MOVEMENT_DISTANCE 1           // Minimum movement to consider (in pixels)
#define TRACK_DURATION 10                 // Track data for 10 seconds
#define STRAIGHT_LINE_THRESHOLD 0.0001    // Threshold for determining if movement is in a straight line
#define VELOCITY_VARIANCE_THRESHOLD 4000 // Threshold for detecting constant velocity
#define STRAIGHT_LINE_COUNT_THRESHOLD 50  // If the number of straight-line intervals exceeds this, it's a bot
#define TRACK_INTERVAL_US 50000           // Interval for tracking movement in microseconds (50ms)

// Function to calculate the distance
double distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Function to get current time in microseconds
long get_current_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec; // Convert seconds to microseconds
}

// Function to normalize angles between 0 and 2*PI
double normalize_angle(double angle) {
    while (angle < 0) angle += 2 * M_PI;
    while (angle >= 2 * M_PI) angle -= 2 * M_PI;
    return angle;
}

// Function to calculate angular difference between two angles
double angular_difference(double angle1, double angle2) {
    double diff = normalize_angle(angle1) - normalize_angle(angle2);
    return fabs(normalize_angle(diff));
}

// Function to calculate the direction (angle) between two points
double calculate_direction(int x1, int y1, int x2, int y2) {
    return atan2(y2 - y1, x2 - x1); // atan2 returns angle in radians
}

// Function to calculate movement velocity
double movement_velocity(int x1, int y1, int x2, int y2, double time_diff_us) {
    // Calculate total pixel movement (in x and y directions)
    int deltaX = abs(x2 - x1);
    int deltaY = abs(y2 - y1);

    // Return the total movement in pixels per time interval
    return (deltaX + deltaY) / (time_diff_us / 1000000.0); // Convert time to seconds
}

int main() {
    Display *display;
    Window rootWindow;
    XEvent event;
    int lastX = 0, lastY = 0;
    int currX, currY;
    long lastTime, currentTime;
    int straightLineCount = 0;  // Count of straight-line intervals
    int totalIntervals = 0;     // Total number of intervals
    double velocitySum = 0.0;   // Sum of velocities for variance calculation
    double velocitySquaredSum = 0.0; // Sum of squared velocities for variance
    int isBot = 0;

    // Open connection to X server
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    rootWindow = DefaultRootWindow(display);

    lastTime = get_current_time_us();  // Track starting time for 10 seconds
    long startTime = get_current_time_us();  // Start time for 10-second tracking

    double lastDirection = 0.0; // Initial direction is undefined
    int isFirstMovement = 1;    // To check if it's the first movement

    while (1) {
        // Query current mouse position
        XQueryPointer(display, rootWindow, &event.xbutton.root, &event.xbutton.subwindow,
                      &currX, &currY, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

        // Get current time in microseconds
        currentTime = get_current_time_us();

        // Check if we've been tracking for 10 seconds
        if (currentTime - startTime >= TRACK_DURATION * 1000000) { // Convert seconds to microseconds
            printf("Tracking completed for 10 seconds.\n");

            // Calculate velocity variance
            double meanVelocity = velocitySum / totalIntervals;
            double variance = (velocitySquaredSum / totalIntervals) - (meanVelocity * meanVelocity);

            // Check bot criteria
            if (straightLineCount >= STRAIGHT_LINE_COUNT_THRESHOLD && variance < VELOCITY_VARIANCE_THRESHOLD) {
                isBot = 1;
                printf("Bot detected: Straight-line intervals: %d, Velocity variance: %.5f\n", straightLineCount, variance);
            } else {
                printf("No bot detected. Straight-line intervals: %d, Velocity variance: %.5f\n", straightLineCount, variance);
            }
            break; // Exit after 10 seconds
        }

        // Track movement at 50,000 microseconds (50ms) intervals
        if (currentTime - lastTime >= TRACK_INTERVAL_US) {
            totalIntervals++;

            // If the cursor has moved significantly
            if (abs(currX - lastX) > MIN_MOVEMENT_DISTANCE || abs(currY - lastY) > MIN_MOVEMENT_DISTANCE) {
                // Calculate the direction of the movement
                double currDirection = calculate_direction(lastX, lastY, currX, currY);
                // Calculate velocity
                double velocity = movement_velocity(lastX, lastY, currX, currY, currentTime - lastTime);

                // Track velocity for variance calculation
                velocitySum += velocity;
                velocitySquaredSum += velocity * velocity;

                // If this is not the first movement, check the difference in direction
                if (!isFirstMovement) {
                    double directionDifference = angular_difference(currDirection, lastDirection);
                    // Dynamic threshold based on movement distance
                    double dynamicThreshold = STRAIGHT_LINE_THRESHOLD * log(distance(lastX, lastY, currX, currY) + 1);
                    // Check if the direction doesn't differ much
                    if (directionDifference < dynamicThreshold) {
                        straightLineCount++;
                    }
                }

                // Update last position and time
                lastX = currX;
                lastY = currY;
                lastTime = currentTime;
                lastDirection = currDirection;  // Update the direction

                isFirstMovement = 0;  // After the first movement, we can start comparing directions
            }
        }
    }

    // Clean up and close the X connection
    XCloseDisplay(display);

    return 0;
}*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/time.h>
#include <unistd.h>

#define MIN_MOVEMENT_DISTANCE 1           // Minimum movement to consider (in pixels)
#define TRACK_DURATION 10                 // Track data for 10 seconds
#define STRAIGHT_LINE_THRESHOLD 0.0001    // Threshold for determining if movement is in a straight line
#define VELOCITY_VARIANCE_THRESHOLD 1500000  // Threshold for detecting constant velocity
#define STRAIGHT_LINE_COUNT_THRESHOLD 50  // If the number of straight-line intervals exceeds this, it's a bot
#define TRACK_INTERVAL_US 100000           // Interval for tracking movement in microseconds (50ms)

// Mutex lock for shared resources
pthread_mutex_t data_lock = PTHREAD_MUTEX_INITIALIZER;
int terminate = 0;

typedef struct {
    int straightLineCount;  // Count of straight-line intervals
    int totalIntervals;     // Total number of intervals
    double velocitySum;     // Sum of velocities for variance calculation
    double velocitySquaredSum; // Sum of squared velocities for variance
    int isBot;              // Flag to indicate bot detection
} SharedData;

SharedData sharedData = {0, 0, 0.0, 0.0, 0}; // Initialize shared data

// Function to calculate the distance
double distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Function to get current time in microseconds
long get_current_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec; // Convert seconds to microseconds
}

// Function to normalize angles between 0 and 2*PI
double normalize_angle(double angle) {
    while (angle < 0) angle += 2 * M_PI;
    while (angle >= 2 * M_PI) angle -= 2 * M_PI;
    return angle;
}

// Function to calculate angular difference between two angles
double angular_difference(double angle1, double angle2) {
    double diff = normalize_angle(angle1) - normalize_angle(angle2);
    return fabs(normalize_angle(diff));
}

// Function to calculate the direction (angle) between two points
double calculate_direction(int x1, int y1, int x2, int y2) {
    return atan2(y2 - y1, x2 - x1); // atan2 returns angle in radians
}

// Function to calculate movement velocity
double movement_velocity(int x1, int y1, int x2, int y2, double time_diff_us) {
    int deltaX = abs(x2 - x1);
    int deltaY = abs(y2 - y1);

    // Return the total movement in pixels per time interval
    return (deltaX + deltaY) / (time_diff_us / 1000000.0); // Convert time to seconds// Convert time_diff_us to seconds
 // Convert time to seconds
}

// Function to detect bot activity
void *bot_detection(void *arg) {
    long startTime = get_current_time_us();

    while (1) {
        pthread_mutex_lock(&data_lock); // Lock shared data
        long currentTime = get_current_time_us();

        if (currentTime - startTime >= TRACK_DURATION * 1000000) { // Check if tracking is complete
            printf("Tracking completed for 10 seconds.\n");

            // Calculate velocity variance
            double meanVelocity = sharedData.velocitySum / sharedData.totalIntervals;
            double variance = (sharedData.velocitySquaredSum / sharedData.totalIntervals) - (meanVelocity * meanVelocity);

            // Check bot criteria
            if (sharedData.straightLineCount >= STRAIGHT_LINE_COUNT_THRESHOLD && variance < VELOCITY_VARIANCE_THRESHOLD) {
                sharedData.isBot = 1;
                printf("Bot detected: Straight-line intervals: %d, Velocity variance: %.5f\n",
                       sharedData.straightLineCount, variance);
            } else {
                printf("No bot detected. Straight-line intervals: %d, Velocity variance: %.5f\n",
                       sharedData.straightLineCount, variance);
            }

            terminate = 1; // Signal the main thread to stop
            pthread_mutex_unlock(&data_lock); // Unlock before exiting
            break;
        }

        pthread_mutex_unlock(&data_lock); // Unlock shared data
        usleep(500000); // Sleep for 500ms to reduce CPU usage
    }

    return NULL;
}


// Function to track mouse movement
void track_movement(Display *display, Window rootWindow) {
    XEvent event;
    int lastX = 0, lastY = 0;
    long lastTime = get_current_time_us();
    double lastDirection = 0.0;
    int isFirstMovement = 1;

    while (!terminate) { // Check termination flag
        // Query current mouse position
        int currX, currY;
        XQueryPointer(display, rootWindow, &event.xbutton.root, &event.xbutton.subwindow,
                      &currX, &currY, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

        // Get current time
        long currentTime = get_current_time_us();

        // Track movement at defined intervals
        if (currentTime - lastTime >= TRACK_INTERVAL_US) {
            pthread_mutex_lock(&data_lock); // Lock shared data

            sharedData.totalIntervals++;

            if (abs(currX - lastX) > MIN_MOVEMENT_DISTANCE || abs(currY - lastY) > MIN_MOVEMENT_DISTANCE) {
                double currDirection = calculate_direction(lastX, lastY, currX, currY);
                double velocity = movement_velocity(lastX, lastY, currX, currY, currentTime - lastTime);

                sharedData.velocitySum += velocity;
                sharedData.velocitySquaredSum += velocity * velocity;

                if (!isFirstMovement) {
                    double directionDifference = angular_difference(currDirection, lastDirection);
                    double dynamicThreshold = STRAIGHT_LINE_THRESHOLD * log(distance(lastX, lastY, currX, currY) + 1);
                    if (directionDifference < dynamicThreshold) {
                        sharedData.straightLineCount++;
                    }
                }

                lastX = currX;
                lastY = currY;
                lastDirection = currDirection;
                isFirstMovement = 0;
            }

            lastTime = currentTime;
            pthread_mutex_unlock(&data_lock); // Unlock shared data
        }
    }
}


int main() {
    Display *display;
    Window rootWindow;

    // Open connection to X server
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }
    rootWindow = DefaultRootWindow(display);

    pthread_t detectionThread;

    // Create a thread for bot detection
    if (pthread_create(&detectionThread, NULL, bot_detection, NULL) != 0) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    // Main thread tracks mouse movement
    track_movement(display, rootWindow);

    // Wait for the bot detection thread to finish
    pthread_join(detectionThread, NULL);
    
    pthread_mutex_destroy(&data_lock);
    

    // Clean up and close the X connection
    XCloseDisplay(display);

    return 0;
}












