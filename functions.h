#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// Matrix settings
#define SIZE 10
#define NUM_SENSORS 5

// Logs
#define MAX_LOGS 100
#define MAX_LOGS_DISPLAY 5

// Cells
#define EMPTY '-'
#define SENSOR 'T'
#define FIRE '@'
#define BURNT '/'

// Time intervals
#define DEFAULT_GENERATION_VERIFICATION_INTERVAL 1
#define FIRE_VERIFICATION_INTERVAL DEFAULT_GENERATION_VERIFICATION_INTERVAL // Should be the same as the generation interval
#define FIRE_GENERATION_INTERVAL DEFAULT_GENERATION_VERIFICATION_INTERVAL // Should be the same as the verification interval
#define FIRE_DURATION 3
#include <stdbool.h>
#include <stdlib.h>

// Forest struct
typedef struct {
    char forest[SIZE][SIZE]; // Forest Matrix
    pthread_mutex_t mutex;   // Mutex for matrix access
    pthread_mutex_t log_mutex; // Mutex for log access
    pthread_cond_t cond;     // Condition for communication
} Forest;

typedef struct {
    int x;
    int y;
} FireLocation;

// Forest Initialization and Setup
void initialize_forest(Forest *forest); // Initialize the forest matrix
void place_sensors(Forest *forest); // Place sensors in random positions

// Sensor Management
void *sensor_thread(void *arg); // Sensor thread, manages sensors in the forest
void *check_free_cells_thread(void *arg); // Check for free cells in the forest

// Fire Management
void *fire_generator_thread(void *arg); // Fire generator thread for the entire forest
void *central_control_thread(void *arg); // Central control thread for managing the forest
void fight_all_fires(Forest* forest); // Fight all fires in the forest
void fight_fire(Forest* forest, int x, int y); // Fight the fire in a specific cell
int check_fire_nearby(Forest* forest, int x, int y); // Check if there is fire near a sensor

// Utility Functions
void display_forest_and_log(Forest *forest); // Display the forest matrix
void saveIntoLogs(const char* log, Forest* forest); // Save a log into the logs array
bool check_for_free_cells_bool(Forest* forest); // Check if there are free cells in the forest
FireLocation where_fire_nearby(Forest* forest, int x, int y); // Find where fire is near a sensor


#endif //FUNCTIONS_H
