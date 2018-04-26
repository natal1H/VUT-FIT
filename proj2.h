// Arguments from command line
int R;
int C;
int ART;
int ABT;

// Semaphore names
#define SEM_ACTION_COUNTER "/xholko02-sem-action_counter"
#define SEM_MUTEX "/xholko02-sem-mutex"
#define SEM_BUS_ARRIVED "/xholko02-sem-bus-arrived"


// Semaphores
sem_t *sem_action_counter;
sem_t *sem_mutex;
sem_t *sem_bus_arrived; // ???

// Shared memory - variables
int *riders_counter; // Number of processes rider currently on bus stop
int *action_counter; // Action number
int *I; // Internal identifier of process


// Shared memory - variable id
int shm_riders_counter_id;
int shm_action_counter_id;
int shm_I_id;


// Log file
#define LOG_FILE_NAME "proj2.out"
FILE *log_file;


/**
 * @brief Converts string to integer
 *
 * @param p_number
 * @param str
 * @return Error code
 */
int convert_str_to_int(int *p_number, char *str);

/**
 *
 * @param range
 * @param arr
 * @return Error code
 */
int get_arguments(int range, char **arr);