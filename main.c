// main.c
#include "functions.h"

int main(void)
{
    Forest forest;
    pthread_t sensors[NUM_SENSORS];
    pthread_t fire_generator;
    pthread_t central_control;
    pthread_t check_free_cells;

    // Initialize forest
    initialize_forest(&forest);

    // Initialize sensors
    place_sensors(&forest);

    // Initialize mutex and condition
    pthread_mutex_init(&forest.mutex, NULL);
    pthread_mutex_init(&forest.log_mutex, NULL);
    pthread_cond_init(&forest.cond, NULL);

    // Create threads
    for (int i = 0; i < NUM_SENSORS; i++) {
        pthread_create(&sensors[i], NULL, sensor_thread, (void *)&forest);
    }
    pthread_create(&fire_generator, NULL, fire_generator_thread, (void *)&forest);
    pthread_create(&central_control, NULL, central_control_thread, (void *)&forest);
    pthread_create(&check_free_cells, NULL, check_free_cells_thread, (void *)&forest);

    // Wait for threads
    for (int i = 0; i < NUM_SENSORS; i++) {
        pthread_join(sensors[i], NULL);
    }
    pthread_join(fire_generator, NULL);
    pthread_join(central_control, NULL);

    // Destroy mutex and condition
    pthread_mutex_destroy(&forest.mutex);
    pthread_cond_destroy(&forest.cond);
    pthread_mutex_destroy(&forest.log_mutex);

    return 0;
}
