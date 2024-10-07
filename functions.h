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
#define WATER 'W'

// Time intervals
#define FIRE_VERIFICATION_INTERVAL 1
#define FIRE_DURATION 3
#define FIRE_GENERATION_INTERVAL 1
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

// Functions
void initialize_forest(Forest *forest); // Initialize the forest matrix
void place_sensors(Forest *forest); // Place sensors in random positions

void *sensor_thread(void *arg); // Sensor thread, this manages the sensors in the forest, one thread per sensor
void *fire_generator_thread(void *arg); // Fire generator thread, this generates fire in the forest, only one thread for all the forest
void *central_control_thread(void *arg); // Central control thread, this manages the forest, one thread for the whole forest
void display_forest_and_log(Forest *forest); // Display the forest matrix
void saveIntoLogs(const char* log, Forest* forest); // Save a log into the logs array
int check_fire_nearby(Forest* forest, int x, int y); // Check if there is fire nearby a sensor
void clear_console(); // Clear the console
void fight_all_fires(Forest* forest); // Fight all the fires in the forest
void fight_fire(Forest* forest, int x, int y); // Fight the fire in a cell
FireLocation where_fire_nearby(Forest* forest, int x, int y) ; // Check where is the fire nearby a sensor

#endif //FUNCTIONS_H
