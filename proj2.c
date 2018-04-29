#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>

// Struct to store cmd line args
typedef struct {
    int R;
    int C;
    int ART;
    int ABT;
} param_t;

// Semaphore names
#define SEM_ACTION_COUNTER "/xholko02-IOS-proj2-sem-action_counter"
#define SEM_INTERNAL_COUNTER "/xholko02-IOS-proj2-sem-internal_counter"
#define SEM_MUTEX "/xholko02-IOS-proj2-sem-mutex"
#define SEM_RIDERS_COUNTER "/xholko02-IOS-proj2-sem-riders_counter"
#define SEM_BUS_ARRIVED "/xholko02-IOS-proj2-sem-bus-arrived"
#define SEM_RIDER_BOARDED "/xholko02-IOS-proj2-sem-rider-boarded"
#define SEM_ALL_GENERATED "/xholko02-IOS-proj2-sem-all-boarded"
#define SEM_NUM_BOARDED "/xholko02-IOS-proj2-sem-num-boarded"

// Semaphores
sem_t *sem_action_counter = NULL;
sem_t *sem_internal_counter = NULL;
sem_t *sem_mutex = NULL;
sem_t *sem_riders_counter = NULL;
sem_t *sem_bus_arrived = NULL;
sem_t *sem_rider_boarded = NULL;
sem_t *sem_num_boarded;

// Shared memory - variables
int *action_counter;
int *internal_counter;
int *riders_counter;
int *num_boarded;

// Log file
#define LOG_FILE_NAME "proj2.out"
FILE *log_file;

#define MMAP(ptr) {(ptr) = mmap(NULL, sizeof(*(ptr)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define UNMAP(ptr) {munmap((ptr), sizeof((ptr)));}

/** Converts string to integer */
int convert_str_to_int(int *p_number, char *str) {
    char *e_ptr = NULL; /**< Part of string that is not number */
    *p_number = (int) strtol(str, &e_ptr, 10);
    return (strlen(e_ptr) == 0) ? 0 : 1; // Decide if conversion was successful
}

int get_arguments(int range, char **arr, param_t *param) {

    // Check number of arguments
    if (range != 5) {
        fprintf(stderr, "Error! Wrong number of arguments.\n");
        return 1;
    }
    // Convert arguments to integer and check conversion
    int err_code = 0;
    err_code += convert_str_to_int(&param->R, arr[1]);
    err_code += convert_str_to_int(&param->C, arr[2]);
    err_code += convert_str_to_int(&param->ART, arr[3]);
    err_code += convert_str_to_int(&param->ABT, arr[4]);
    if (err_code > 0) {
        fprintf(stderr, "Error! Not an integer.\n");
        return 1;
    }
    // Check range
    if ((param->R <= 0) || (param->C <= 0) || (param->ART < 0 || param->ART > 1000) || (param->ABT < 0 || param->ABT > 1000)) {
        fprintf(stderr, "Error! Numbers out of range.\n");
        return 1;
    } else
        return 0;
}

int create_shm() {
    // ACTION COUNTER
    MMAP(action_counter);
    *action_counter = 1;

    // INTERNAL RIDER ID COUNTER
    MMAP(internal_counter);
    *internal_counter = 0;

    // RIDER AT BUS STOP COUNTER
    MMAP(riders_counter);
    *riders_counter = 0;

    // NUM BOARDED
    MMAP(num_boarded);
    *num_boarded = 0;

    return 0;
}

int create_semaphores() {
    if ((sem_action_counter = sem_open(SEM_ACTION_COUNTER, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening action counter semaphore failed.\n");
        return 1;
    }
    if ((sem_internal_counter = sem_open(SEM_INTERNAL_COUNTER, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening internal counter semaphore failed.\n");
        return 1;
    }
    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening mutex semaphore failed.\n");
        return 1;
    }
    if ((sem_riders_counter = sem_open(SEM_RIDERS_COUNTER, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening riders counter semaphore failed.\n");
        return 1;
    }
    if ((sem_bus_arrived = sem_open(SEM_BUS_ARRIVED, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening bus arrived semaphore failed.\n");
        return 1;
    }
    if ((sem_rider_boarded = sem_open(SEM_RIDER_BOARDED, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening rider boarded semaphore failed.\n");
        return 1;
    }
    if ((sem_num_boarded= sem_open(SEM_NUM_BOARDED, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening num boarded semaphore failed.\n");
        return 1;
    }

    return 0;
}

int init_resources() {
    int err_code = 0;
    err_code += create_shm();
    err_code += create_semaphores();
    return err_code;
}

void clean_shm() {
    UNMAP(action_counter);
    UNMAP(internal_counter);
    UNMAP(riders_counter);
    UNMAP(num_boarded);
    //UNMAP(all_generated);
}

void clean_semaphores() {
    // Close semaphores
    sem_close(sem_action_counter);
    sem_close(sem_internal_counter);
    sem_close(sem_mutex);
    sem_close(sem_riders_counter);
    sem_close(sem_bus_arrived);
    sem_close(sem_rider_boarded);
    sem_close(sem_num_boarded);

    // Unlink semaphores
    sem_unlink(SEM_ACTION_COUNTER);
    sem_unlink(SEM_INTERNAL_COUNTER);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_RIDERS_COUNTER);
    sem_unlink(SEM_BUS_ARRIVED);
    sem_unlink(SEM_RIDER_BOARDED);
    sem_unlink(SEM_NUM_BOARDED);
}

void clean_resources() {
    clean_shm();
    clean_semaphores();
}

void inc_shm(int *var, sem_t *sem) {
    sem_wait(sem);
    *var += 1;
    sem_post(sem);
}

void depart(param_t param) {
    // Log depart + inc action counter
    fprintf(log_file, "%d\t: BUS\t: depart\n", *action_counter);
    inc_shm(action_counter, sem_action_counter);

    int sleep_time = (rand() % param.ABT);
    if (sleep_time != 0) usleep(sleep_time );

    // Log end (bus finished ride - woke up from sleep) + inc action counter
    fprintf(log_file, "%d\t: BUS\t: end\n", *action_counter);
    inc_shm(action_counter, sem_action_counter);
}

void arrive(param_t param) {
    sem_wait(sem_mutex);

    // Log arrival + inc action counter
    fprintf(log_file, "%d\t: BUS\t: arrival\n", *action_counter);
    inc_shm(action_counter, sem_action_counter);

    if (*riders_counter == 0) {
        sem_post(sem_mutex);
        depart(param);
    }
    else {
        // Log start boarding + inc action counter
        fprintf(log_file, "%d\t: BUS\t: start boarding: %d\n", *action_counter, *riders_counter);
        inc_shm(action_counter, sem_action_counter);

        int boarding = (*riders_counter > param.C) ? param.C : *riders_counter;
        for (int i = 0; i < boarding; i++) {
            sem_post(sem_bus_arrived);
            sem_wait(sem_rider_boarded);
        }

        // Log end boarding + inc action counter
        fprintf(log_file, "%d\t: BUS\t: end boarding: %d\n", *action_counter, *riders_counter);
        inc_shm(action_counter, sem_action_counter);

        sem_post(sem_mutex);
        depart(param);
    }
}

void bus_process(param_t param) {
    // Log start of bus + inc action counter
    fprintf(log_file, "%d\t: BUS\t: start\n", *action_counter);
    inc_shm(action_counter, sem_action_counter);

    while (*num_boarded < param.R || *riders_counter != 0) {
        // BUS arrived
        arrive(param);
    }

    // Log bus finish + inc action counter
    fprintf(log_file, "%d\t: BUS\t: finish\n", *action_counter);
    inc_shm(action_counter, sem_action_counter);

    exit(0);
}

void board(int RID) {
    // Log boarding of rider RID + inc action counter
    fprintf(log_file, "%d\t: RIDER %d\t: boarding\n", *action_counter, RID);
    inc_shm(action_counter, sem_action_counter);

    inc_shm(num_boarded, sem_num_boarded);

    // Rider boarded - decrease number of riders on bus stop
    sem_wait(sem_riders_counter);
    *riders_counter -= 1;
    sem_post(sem_riders_counter);

}

void rider_process(int RID) {
    // Log start of rider RID + inc action counter
    fprintf(log_file, "%d\t: RIDER %d\t: start\n", *action_counter, RID);
    inc_shm(action_counter, sem_action_counter);

    // Inc riders at bus stop counter
    inc_shm(riders_counter, sem_riders_counter);

    // Log arrival of rider to bus stop
    fprintf(log_file, "%d\t: RIDER %d\t: enter: %d\n", *action_counter, RID, *riders_counter);
    inc_shm(action_counter, sem_action_counter);

    // Pass back mutex
    sem_post(sem_mutex);

    // Wait until bus arrived
    sem_wait(sem_bus_arrived);

    board(RID);

    sem_post(sem_rider_boarded);

    // Log finish of rider RID + inc action counter
    fprintf(log_file, "%d\t: RIDER %d\t: finish\n", *action_counter, RID);
    inc_shm(action_counter, sem_action_counter);

    exit(0);
}

void rider_generator_process(param_t param) {

    for (int i = 0; i < param.R; i++) {

        // Sleep before creating RIDER process
        int sleep_time = (rand() % param.ART);
        if (sleep_time != 0) usleep(sleep_time );

        pid_t rider_process_id = fork();

        if (rider_process_id == 0) {
            // Child process - rider process
            sem_wait(sem_mutex);

            inc_shm(internal_counter, sem_internal_counter);

            rider_process(*internal_counter);
        }
        else if (rider_process_id == -1) {
            // Handle error
            fprintf(stderr, "Error! Fork failed.\n");
            clean_resources();
            exit(1);
        }
    }


    exit(0);
}

int main(int argc, char **argv) {
    // Open log file
    log_file = fopen(LOG_FILE_NAME, "w");

    // Prevent buffering of the output
    setbuf(log_file, NULL);
    setbuf(stdout, NULL);

    // Randomize
    srand(time(0));

    // Parse arguments
    param_t param;
    if (get_arguments(argc, argv, &param) != 0) return 1;

    // Initialize resources
    if (init_resources() != 0) {
        // Error during initialization
        clean_resources();
        exit(1);
    }

    pid_t bus_id = fork();

    if (bus_id == 0) {
        // Child process - bus process
        bus_process(param);
    }
    else if (bus_id > 0) {
        // Parent process

        pid_t rider_generator_id = fork();

        if (rider_generator_id == 0) {
            // Child process - rider generator process
            rider_generator_process(param);
        }
        else if (rider_generator_id == -1) {
            // Handle error
            fprintf(stderr, "Error! Fork failed.\n");
            clean_resources();
            exit(1);
        }
    }
    else if (bus_id == -1) {
        // Handle error
        fprintf(stderr, "Error! Fork failed.\n");
        clean_resources();
        exit(1);
    }


    // End of program - clean up, close log
    clean_resources();
    fclose(log_file);
    return 0;
}
