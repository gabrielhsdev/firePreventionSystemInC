#include "functions.h"

// Global variable to save logs of recent events
char logs[MAX_LOGS][100];
int logs_count = 0;

void* sensor_thread(void* arg) {
    Forest* forest = (Forest*)arg;

    while (1) {
        pthread_mutex_lock(&forest->mutex);

        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (forest->forest[i][j] == SENSOR) {
                    if (check_fire_nearby(forest, i, j)) {
                        // Get where the fire is
                        FireLocation fire_location = where_fire_nearby(forest, i, j);

                        // If we are on a border, continue
                        if (fire_location.x == SIZE - 1 || fire_location.y == SIZE - 1 || fire_location.x == 0 || fire_location.y == 0) {
                            continue;
                        }

                        char str[100];
                        sprintf(str, "Sensor em (%d, %d) detectou fogo!", i, j);
                        saveIntoLogs(str, forest);

                        // Fight the fire
                        fight_fire(forest, fire_location.x, fire_location.y);

                        // Display the forest and logs after detecting the fire
                        display_forest_and_log(forest);
                    }
                }
            }
        }
        pthread_mutex_unlock(&forest->mutex);

        // Wait before the next verification
        sleep(FIRE_VERIFICATION_INTERVAL);
    }

    return NULL;
}

void* fire_generator_thread(void* arg) {
    Forest* forest = (Forest*)arg;

    while (1) {
        pthread_mutex_lock(&forest->mutex);

        // Gerar fogo em uma posição aleatória
        int x = rand() % SIZE;
        int y = rand() % SIZE;

        if (forest->forest[x][y] == EMPTY) {
            forest->forest[x][y] = FIRE; // Colocar fogo ativo

            char str[100];
            sprintf(str, "Fogo gerado em (%d, %d)", x, y);
            saveIntoLogs(str, forest);

            display_forest_and_log(forest);
        } else {
            // In order to skip the message if the cell is not empty we need to unlock the mutex
            pthread_mutex_unlock(&forest->mutex);
            continue;
        }

        pthread_mutex_unlock(&forest->mutex);

        /*
        // This part of the code would be used if we wanted to keep the fire active for a certain time, then change it to burnt
        sleep(FIRE_DURATION); // Fire is active for this time

        // Change the state of the fire to burnt
        pthread_mutex_lock(&forest->mutex);
        if (forest->forest[x][y] == FIRE) {
            forest->forest[x][y] = BURNT; // Mudar para célula queimada

            char str[100];
            sprintf(str, "Fogo em (%d, %d) agora está queimado", x, y);
            saveIntoLogs(str, forest);

            display_forest_and_log(forest);  // Exibir a matriz após queimar fogo
        }
        pthread_mutex_unlock(&forest->mutex);
        */

        sleep(FIRE_GENERATION_INTERVAL); // Tempo entre gerações de fogo
    }

    return NULL;
}

void *central_control_thread(void *arg) {
    Forest *forest = (Forest *)arg;

    while (1) {
        //  If there is a fire on the corners fight all fires
        if (forest->forest[0][0] == FIRE || forest->forest[0][SIZE - 1] == FIRE ||
            forest->forest[SIZE - 1][0] == FIRE || forest->forest[SIZE - 1][SIZE - 1] == FIRE) {
            fight_all_fires(forest);
            display_forest_and_log(forest);
        }

        // If there is a fire on the edges fight all fires
        for (int i = 1; i < SIZE - 1; i++) {
            if (forest->forest[i][0] == FIRE || forest->forest[i][SIZE - 1] == FIRE ||
                forest->forest[0][i] == FIRE || forest->forest[SIZE - 1][i] == FIRE) {
                fight_all_fires(forest);
                display_forest_and_log(forest);
                break;
            }
        }

        sleep(FIRE_VERIFICATION_INTERVAL);  // Check every second
    }

    return NULL;
}

void* check_free_cells_thread(void* arg) {
    Forest* forest = (Forest*)arg;

    while (1) {
        pthread_mutex_lock(&forest->mutex);
        if (!check_for_free_cells_bool(forest)) {
            // No free cells, signal threads to terminate
            printf("No free cells left. Exiting...\n");
            pthread_mutex_unlock(&forest->mutex);
            exit(0); // End the program
        }
        pthread_mutex_unlock(&forest->mutex);

        sleep(FIRE_VERIFICATION_INTERVAL); // Check every second or as needed
    }

    return NULL;
}

int check_fire_nearby(Forest* forest, int x, int y) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;

            int new_x = x + i;
            int new_y = y + j;
            if (new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
                if (forest->forest[new_x][new_y] == FIRE) {
                    return 1; // Fire detected nearby
                }
            }
        }
    }
    return 0; // No fire detected nearby
}

FireLocation where_fire_nearby(Forest* forest, int x, int y) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;

            int new_x = x + i;
            int new_y = y + j;
            if (new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
                if (forest->forest[new_x][new_y] == FIRE) {
                    return (FireLocation){new_x, new_y}; // Fire detected nearby
                }
            }
        }
    }
    return (FireLocation){-1, -1}; // No fire detected nearby
}


// Update the function saveIntoLogs
void saveIntoLogs(const char* log, Forest* forest) {
    // Lock the log mutex
    pthread_mutex_lock(&forest->log_mutex);

    // Check if there is not another log with the same message
    for (int i = 0; i < MAX_LOGS; i++) {
        if (strcmp(logs[i], log) == 0) {
            pthread_mutex_unlock(&forest->log_mutex);
            return; // Duplicate log, exit
        }
    }

    // Shift logs to make space for the new one
    for (int i = MAX_LOGS - 1; i > 0; i--) {
        strcpy(logs[i], logs[i - 1]);
    }
    strcpy(logs[0], log);
    logs_count++;
    if (logs_count > MAX_LOGS) {
        logs_count = MAX_LOGS;
    }

    // Unlock the log mutex
    pthread_mutex_unlock(&forest->log_mutex);
}

void display_forest_and_log(Forest *forest) {

    // Display X-axis labels
    printf("    "); // Space for Y-axis label
    for (int j = 0; j < SIZE; j++) {
        printf("%2d ", j); // Print column numbers (X-axis)
    }
    printf("\n");

    // Display the forest matrix with Y-axis labels
    for (int i = 0; i < SIZE; i++) {
        printf("%2d  ", i); // Print row numbers (Y-axis)
        for (int j = 0; j < SIZE; j++) {
            printf("%c  ", forest->forest[i][j]); // Print forest matrix elements
        }
        printf("\n");
    }
    printf("\n");

    // Display the logs with log numbers
    printf("- Logs -\n");
    for (int i = 0; i < MAX_LOGS_DISPLAY; i++) {
        printf("Log %d: %s\n", i + 1, logs[i]); // Add log number next to each log entry
    }
    printf("\n");
}



void initialize_forest(Forest *forest) {
    // Initialize the forest matrix
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            forest->forest[i][j] = EMPTY; // Initially free area
        }
    }
}

void place_sensors(Forest *forest) {
    // Place sensors in random positions
    int sensors_placed = 0;
    while (sensors_placed < NUM_SENSORS) {
        int x = rand() % SIZE;  // Generate a random position
        int y = rand() % SIZE;  // Generate a random position
        if (forest->forest[x][y] == EMPTY) { // Check if the position is free
            forest->forest[x][y] = SENSOR; // Place sensor
            sensors_placed++;
        }
    }
}

void fight_fire(Forest* forest, int x, int y) {
    // Change to burnt cell
    forest->forest[x][y] = BURNT;

    // Log
    char str[100];
    sprintf(str, "Fogo em (%d, %d) foi apagado", x, y);
    saveIntoLogs(str, forest);
}

void fight_all_fires(Forest* forest) {
    // Lock
    pthread_mutex_lock(&forest->mutex);

    // Add a log to say that all fires are being fought
    // Timestamp for the log hh:mm:ss + todos os fogos estão sendo apagados
    char str[100];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(str, "%02d:%02d:%02d - Todos os fogos estão sendo apagados", tm.tm_hour, tm.tm_min, tm.tm_sec);
    saveIntoLogs(str, forest);

    // Check all cells
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (forest->forest[i][j] == FIRE) {
                // Change to burnt cell
                forest->forest[i][j] = BURNT;
                // Log
                char str[100];
                sprintf(str, "Fogo em (%d, %d) foi apagado", i, j);
                saveIntoLogs(str, forest);
            }
        }
    }
    // Unlock
    pthread_mutex_unlock(&forest->mutex);
}

bool check_for_free_cells_bool(Forest* forest) {
    // Check if there are free cells in the forest
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (forest->forest[i][j] == EMPTY) {
                return true; // There are free cells
            }
        }
    }
    return false; // There are no free cells
}