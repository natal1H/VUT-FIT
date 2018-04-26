// Arguments from command line
int R;
int C;
int ART;
int ABT;

// Semaphore names
#define SEM_ACTION_COUNTER "/xholko02-sem-action_counter"
#define SEM_MUTEX "/xholko02-sem-mutex"
#define SEM_BUS_ARRIVED "/xholko02-sem-bus-arrived"
#define SEM_BUS "/xholko02-sem-bus"
#define SEM_INTERNAL_COUNTER "/xholko02-sem-internal-counter"
#define SEM_MULTIPLEX "/xholko02-sem-multiplex"
#define SEM_RIDERS_COUNTER "/xholko02-sem-riders-counter"

// Semaphores
sem_t *sem_action_counter;
sem_t *sem_mutex;
sem_t *sem_bus_arrived; // ???
sem_t *sem_bus;
sem_t *sem_internal_counter;
sem_t *sem_riders_counter;
sem_t *sem_multiplex;

// Shared memory - variables
int *riders_counter; // Number of processes rider currently on bus stop
int *action_counter; // Action number
int *internal_counter; // Internal identifier of process


// Shared memory - variable id
int shm_riders_counter_id;
int shm_action_counter_id;
int shm_internal_counter_id;


pid_t PID_1;
pid_t PID_3;

pid_t id;
pid_t id3;


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