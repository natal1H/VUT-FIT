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
#include<time.h>

#include "proj2.h"

/** Converts string to integer */
int convert_str_to_int(int *p_number, char *str) {
    char *e_ptr = NULL; /**< Part of string that is not number */
    *p_number = (int) strtol(str, &e_ptr, 10);
    return (strlen(e_ptr) == 0) ? 0 : 1; // Decide if conversion was successful
}

int get_arguments(int range, char **arr) {
    // prevent buffering of the output
    setbuf(stdout, NULL);

    // Check number of arguments
    if (range != 5) {
        fprintf(stderr, "Error! Wrong number of arguments.\n");
        return 1;
    }
    // Convert arguments to integer and check conversion
    int err_code = 0;
    err_code += convert_str_to_int(&R, arr[1]);
    err_code += convert_str_to_int(&C, arr[2]);
    err_code += convert_str_to_int(&ART, arr[3]);
    err_code += convert_str_to_int(&ABT, arr[4]);
    if (err_code > 0) {
        fprintf(stderr, "Error! Not an integer.\n");
        return 1;
    }
    // Check range
    if ((R <= 0) || (C <= 0) || (ART < 0 || ART > 1000) || (ABT < 0 || ABT > 1000)) {
        fprintf(stderr, "Error! Numbers out of range.\n");
        return 1;
    } else {
        return 0;
    }
}

int create_shm() {
    printf("CREATE SHM\n");

    // ACTION COUNTER
    if ((shm_action_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        // handle error
        fprintf(stderr, "Error! Could not create shm_action_counter.\n");
        return 1;
    }
    if ((action_counter = shmat(shm_action_counter_id, NULL, 0)) == NULL) {
        // handle error
        fprintf(stderr, "Error! Could not create shm_action_counter.\n");
        return 1;
    }
    *action_counter = 1;

    // RIDERS AT BUS STOP COUNTER
    if ((shm_riders_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        // handle error
        fprintf(stderr, "Error! Could not create shm_riders_counter.\n");
        return 1;
    }
    if ((riders_counter = shmat(shm_riders_counter_id, NULL, 0)) == NULL) {
        // handle error
        fprintf(stderr, "Error! Could not create shm_riders_counter.\n");
        return 1;
    }
    *riders_counter = 0;

    // INTERNAL RIDER ID COUNTER
    if ((shm_internal_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        // handle error
        fprintf(stderr, "Error! Could not create shm_internal_counter.\n");
        return 1;
    }
    if ((internal_counter = shmat(shm_riders_counter_id, NULL, 0)) == NULL) {
        // handle error
        fprintf(stderr, "Error! Could not create shm_internal_counter.\n");
        return 1;
    }
    *internal_counter = 1;

    return 0;
}

int create_semaphores() {
    printf("CREATE SEMAPHORES\n");
    if ((sem_action_counter = sem_open(SEM_ACTION_COUNTER, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening action counter semaphore failed.\n");
        return 1;
    }
    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening mutex semaphore failed.\n");
        return 1;
    }
    if ((sem_bus = sem_open(SEM_BUS, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening bus semaphore failed.\n");
        return 1;
    }
    if ((sem_internal_counter = sem_open(SEM_INTERNAL_COUNTER, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening internal counter semaphore failed.\n");
        return 1;
    }
    if ((sem_multiplex = sem_open(SEM_MULTIPLEX, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, C)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening multiplex semaphore failed.\n");
        return 1;
    }
    if ((sem_riders_counter = sem_open(SEM_RIDERS_COUNTER, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        // handle error
        fprintf(stderr, "Error! Opening riders counter semaphore failed.\n");
        return 1;
    }

    return 0;
}

void clean_shm() {
    printf("CLEAN SHM\n");
    shmctl(shm_action_counter_id, IPC_RMID, NULL);
    shmctl(shm_riders_counter_id, IPC_RMID, NULL);
    shmctl(shm_internal_counter_id, IPC_RMID, NULL);
}

void clean_semaphores() {
    printf("CLOSE SEMAPHORES\n");
    // Closes semaphores
    sem_close(sem_action_counter);
    sem_close(sem_mutex);
    sem_close(sem_bus);
    sem_close(sem_internal_counter);
    sem_close(sem_multiplex);
    sem_close(sem_riders_counter);

    // Removes semaphores
    sem_unlink(SEM_ACTION_COUNTER);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_BUS);
    sem_unlink(SEM_INTERNAL_COUNTER);
    sem_unlink(SEM_MULTIPLEX);
    sem_unlink(SEM_RIDERS_COUNTER);
}

void depart() {
    printf(";;;;DEPART\n");

    // Make bus process sleep
    // get random number from <0, ABT>

    int sleep_time = (rand() % ABT) * 1000; // generated time + convert
    printf("TIME for BUS generated: %d\n", sleep_time);
    if (sleep_time != 0) {
        printf("BUS GOING to sleep\n");
        usleep(sleep_time);
        printf("BUS waking up\n");
    }

    // after waking up print to log
    fprintf(log_file, "%d\t: BUS\t: end\n", *action_counter);
    // Increase action counter
    sem_wait(sem_action_counter);
    *action_counter = *action_counter + 1;
    sem_post(sem_action_counter);
    printf("NEW action counter after end: %d\n", *action_counter);

    printf(";;;\n");
}

void arrive() {
    sem_wait(sem_mutex);
    printf("!!!ARRIVING\n");

    // Print bus arrived
    fprintf(log_file, "%d\t: BUS\t: arrival\n", *action_counter);
    // Increase action counter
    sem_wait(sem_action_counter);
    *action_counter = *action_counter + 1;
    sem_post(sem_action_counter);
    printf("NEW action counter after arrive: %d\n", *action_counter);

    // Check if someone on bus stop
    if (*riders_counter > 0) {
        // Start boarding
    }
    else {
        // Leave immediately
        // Print bus departed
        fprintf(log_file, "%d\t: BUS\t: depart\n", *action_counter);
        // Increase action counter
        sem_wait(sem_action_counter);
        *action_counter = *action_counter + 1;
        sem_post(sem_action_counter);
        printf("NEW action counter after depart: %d\n", *action_counter);

    }

    printf("BUS IS GOING TO DEPART\n");
    depart();

    printf("!!!\n");
    sem_post(sem_mutex);
}

void doBusStuff() {
    printf("###\nDoing bus stuff:\n");

    // TODO doplnit podmienku ukoncenie jazdy (vsetci riders vygenerovani a nik necaka)
    while (*internal_counter < R) {
        arrive();
    }
    printf("###\n");
}

int main(int argc, char **argv) {
    clean_shm();
    clean_semaphores();
	printf("SENATE BUS PROBLEM\n");
/*
    srand(time(0));

    if (get_arguments(argc, argv))
        return 1;

    // Open log file
    log_file = fopen(LOG_FILE_NAME, "w");
    setbuf(log_file, NULL);

    if (create_semaphores())  {
        clean_semaphores();
        return 1;
    }
    if (create_shm()) {
        clean_semaphores();
        return 1;
    }

    pid_t bus_pid;
    pid_t rider_process;
    bus_pid = fork();

    if(bus_pid == -1) { // error
        perror("Error! Could not create bus process.\n");
        abort();
    }
    else if(bus_pid == 0) {
        / * child process * /
 / *       printf("===\nCHILD, bus_id: %d, getid: %d\n", bus_pid, getpid());
        printf("===\n");
*/
        // Generate RIDERS
 //      for (int i = 1; i < R+1; i++) {
//            sem_wait(sem_mutex);



//            int sleep_time = (rand() % ART) * 1000; // generated time + convert
//            printf("TIME for RIDER generated: %d\n", sleep_time);
//            if (sleep_time != 0) {
//                printf("RIDER GOING to sleep\n");
//                usleep(sleep_time);
//                printf("RIDER waking up - time to generate\n");
//            }
            // fork rider process
 /*          sem_wait(sem_mutex);
           printf("<<<GENERATING RIDERS %d\n", i);
            rider_process = fork();
            if (rider_process == 0) { // child
                printf("---RIDER PROCESS (%d) generated with id: %d, getid: %d---\n", i, rider_process, getpid());
                // Increase riders counter at bus stop

            }
            sem_post(sem_mutex);
*/
            ////////
//            rider_process = fork();
//            if (rider_process == 0) {
//                printf("MASTER RIDER\n");
                //_exit(0);
//            }
//            else {
//                printf("CHILD RIDER\n");
//                _exit(0);
//            }
            ////////

            // Increase riders counter
//            fprintf(log_file, "%d\t: RIDER %d\t: start\n", *action_counter, i);
//            sem_wait(sem_action_counter);
//            *action_counter = *action_counter + 1;
//            sem_post(sem_action_counter);
//            printf("NEW action counter after generating: %d\n", *action_counter);

//            sem_post(sem_mutex);
  //     }
//       printf("EXITING CHILD\n");
        //clean_semaphores();
        //clean_shm();
       //_exit(0);
 //   }
//    else if (bus_pid > 0) {
        /* MASTER */
//        printf("===\nMASTER, bus_pid: %d, getpid: %d\n", bus_pid, getpid());

        // BUS start -> log to log
//        fprintf(log_file, "%d\t: BUS\t: start\n", *action_counter);

        // Increase action counter
//        sem_wait(sem_action_counter);
//        *action_counter = *action_counter + 1;
//        sem_post(sem_action_counter);
//        printf("NEW action counter: %d\n", *action_counter);

//        doBusStuff();

/*        printf("===\n");

        fclose(log_file);
        clean_semaphores();
        clean_shm();
    }
*/
//    fclose(log_file);
//    clean_semaphores();
//    clean_shm();
//	return 0;

    log_file = fopen(LOG_FILE_NAME, "w");

    setbuf(log_file,NULL);
    setbuf(stderr,NULL);

    srand(time(0));

    if (get_arguments(argc, argv))
        exit(1);

/*  Making semaphores and creating shared memory segments  */

    create_semaphores();
    create_shm();

    int err_check;

    if ( (id = fork()) < 0) {
        fprintf(stderr, "Error: fork()\n");
        clean_semaphores();
        clean_shm();
        exit(2);
    }

    if (id == 0) { // Child
        // Generate riders
        int sleep_time = rand(); // generated time + convert
        printf("RAND: %d, mod: %d, times: %d\n", sleep_time, sleep_time % ART, (sleep_time % ART)*1000);
        printf("TIME for RIDER generated: %d\n", (sleep_time % ART)*1000);
        sleep_time = (sleep_time % ART)*1000;
        for (int i = 1; i < R+1; i++) {
            // SEM MULTIPLEX - generate processes only if capacity at bus stop
            sem_wait(sem_multiplex);
            if ( (id3 = fork()) == 0) {

                // Rider
                err_check = sem_wait(sem_mutex);
                if (err_check == -1) {
                    fprintf(stderr, "System call error\n");
                    clean_semaphores(); clean_shm();
                    exit(-1);
                }

                if (sleep_time != 0) {
                    printf("RIDER GOING to sleep\n");
                    usleep(sleep_time);
                    printf("RIDER waking up - time to generate\n");
                }

                // WARNING!!! CURSED CODE - SOMEHOW INCREASES ITSELF
                // Increase internal counter
                printf(">I before: %d\n", *internal_counter);
                sem_wait(sem_internal_counter);
                //*internal_counter = *internal_counter + 1;
                //(*internal_counter)++;
                sem_post(sem_internal_counter);
                printf(">I before: %d\n", *internal_counter);


                printf("Rider %d\n", *internal_counter);

                fprintf(log_file, "%d\t: RIDER %d\t: start\n", *action_counter, *internal_counter);
                // Increase action counter
                sem_wait(sem_action_counter);
                *action_counter = *action_counter + 1;
                sem_post(sem_action_counter);
                printf("NEW action counter after generating: %d\n", *action_counter);

                // Increase riders on bus stop counter
                sem_wait(sem_riders_counter);
                *riders_counter = *riders_counter + 1;
                sem_post(sem_riders_counter);
                printf("RIDERS na zastavke: %d\n", *riders_counter);

                err_check = sem_post(sem_mutex);
                if (err_check == -1) {
                    fprintf(stderr, "System call error\n");
                    clean_semaphores(); clean_shm();
                    exit(-1);
                }
/*
                check = sem_wait(sem_1);
                if (check == -1) { fprintf(stderr, "System call error\n"); res_clean(); exit(2); }

                fprintf(file, "%-6d: A %-4d: started\n", ++(*counter), i);

                check = sem_post(sem_1);
                if (check == -1) { fprintf(stderr, "System call error\n"); res_clean(); exit(2); }

                adult_fun(i, A, AWT, file);

                if ( (*finish_var) == (A+C) ) {
                    for (int h=0; h<C; h++) {
                        check = sem_post(sem_2);
                        if (check == -1) { fprintf(stderr, "System call error\n"); res_clean(); exit(2); }
                    }
                    for (int z=0; z<A; z++) {
                        check = sem_post(sem_3);
                        if (check == -1) { fprintf(stderr, "System call error\n"); res_clean(); exit(2); }
                    }
                }

                check = sem_wait(sem_3);
                if (check == -1) { fprintf(stderr, "System call error\n"); res_clean(); exit(2); }

                check = sem_wait(sem_1);
                if (check == -1) { fprintf(stderr, "System call error\n"); res_clean(); exit(2); }

                fprintf(file, "%-6d: A %-4d: finished\n", ++(*counter), i);

                check = sem_post(sem_1);
                if (check == -1) { fprintf(stderr, "System call error\n"); res_clean(); exit(2); }
*/
                exit(i);
            } // if

            else if ( id3 < 0) {
                fprintf(stderr, "Error: fork()\n");
                //res_clean();
                clean_semaphores();
                clean_shm();
                exit(2);
            }

            else {
                PID_3 = id3;
            }
            sem_post(sem_multiplex);

            //usleep(AGT);
        } // for

        waitpid(PID_3, NULL, 0);
        exit(0);
    } // if

    // BUS process
    if (id > 0 ) {
        PID_1 = id;

        printf("===\nMASTER\n");

        // BUS start -> log to log
        fprintf(log_file, "%d\t: BUS\t: start\n", *action_counter);

        // Increase action counter
        sem_wait(sem_action_counter);
        *action_counter = *action_counter + 1;
        sem_post(sem_action_counter);
        //doBusStuff();

        printf("===\n");
    }

    waitpid(PID_1, NULL, 0);

    clean_semaphores();
    clean_shm();

    fclose(log_file);
    exit(0);
}
