#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h> /* abort */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>

// TODO ohranicit generovanie rider process pom mutex

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

// Semaphores
sem_t *sem_action_counter;
sem_t *sem_internal_counter;
sem_t *sem_mutex;

// Shared memory - variables
int *action_counter;
int *internal_counter;

// Shared memory - variable id
int shm_action_counter_id;
int shm_internal_counter_id;

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
}

void clean_semaphores() {
    // Close semaphores
    sem_close(sem_action_counter);
    sem_close(sem_internal_counter);
    sem_close(sem_mutex);

    // Unlink semaphores
    sem_unlink(SEM_ACTION_COUNTER);
    sem_unlink(SEM_INTERNAL_COUNTER);
    sem_unlink(SEM_MUTEX);
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

void bus_process() {
    // Log start of bus
    fprintf(log_file, "%d\t: BUS\t: start\n", *action_counter);
    inc_shm(action_counter, sem_action_counter);

    // Log bus finish
    fprintf(log_file, "%d\t: BUS\t: finish\n", *action_counter);
    inc_shm(action_counter, sem_action_counter);

    exit(0);
}

void rider_process(int RID) {
    // Log start of rider RID
    fprintf(log_file, "%d\t: RIDER %d\t: start\n", *action_counter, RID);
    inc_shm(action_counter, sem_action_counter);

    sem_post(sem_mutex);

    exit(0);
}

void rider_generator_process(int R, int delay_time) {

    for (int i = 0; i < R; i++) {
        pid_t rider_process_id = fork();
        if (rider_process_id == 0) {
            // Child process - rider process
            sem_wait(sem_mutex);

            inc_shm(internal_counter, sem_internal_counter);
            printf("Rider process %d\n", *internal_counter);
            rider_process(*internal_counter);
        }
        else if (rider_process_id == -1) {
            // Handle error
            fprintf(stderr, "Error! Fork failed.\n");
            clean_resources();
            exit(1);
        }
        int sleep_time = (rand() % delay_time);
        printf("Sleeptime: %d\n", sleep_time);
        if (sleep_time != 0) usleep(sleep_time );
        printf("Rider %d woke up\n", i);
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
        // Child process
        printf("Child process - bus process\n");

        // Bus process
        bus_process();
    }
    else if (bus_id > 0) {
        // Parent process
        printf("Parent of bus process\n");

        pid_t rider_generator_id = fork();
        if (rider_generator_id == 0) {
            // Child process - rider generator process
            printf("Child process - rider generator process\n");
            rider_generator_process(param.R, param.ART);
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
