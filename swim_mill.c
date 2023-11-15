
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>

sem_t *swim_semaphore;

char (*grid)[10];  //2d array
void showGrid(char (*grid)[10]) {
    //making grid of swim mill of 10x10
    printf("\n------Swim mill grid------\n");
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j ++){
//            printf(" ");
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

//creating an output file named "out.txt", appending to new line.
void write_to_file(char (*grid)[10]){
    FILE *file = fopen("output.txt","a");
    for( int i = 0; i < 10; i++ ){
        for (int j = 0; j < 10; j++) {
            if(j % 10 == 0 ){
                fprintf(file, "\n%c", grid[i][j]);
            }
            else{
                fprintf(file, "%c", grid[i][j]);
            }
            if(i == 9 && j == 9) {
                fprintf(file, "\n\n");
            }
        }
    }
}

pid_t fish;
pid_t pellet;
int shm_id;

void interrupt(){
    printf("Interruption\n");

    //kill kid processes
    kill(fish, SIGINT);
    kill(pellet, SIGINT);

    //detach the grid from shared memory
    shmdt(grid);

    //delete the share memory
    shmctl(shm_id, IPC_RMID, 0);

    //wait for all process to terminate properly
    sleep(5);
    exit(0);
}
//
void terminate(){

    printf("Termination Here\n");
    //kill the kids
    kill(0,SIGTERM);

    //force wait until child process terminated
    while(wait(NULL) > 0);

    //detach the grid from shared memory
    shmdt(grid);

    //delete the share memory
    shmctl(shm_id, IPC_RMID, NULL);

    //closes the named semaphore. allowing any resources that system
    // has allocated to the calling process for this sem to be freed.
    sem_close(swim_semaphore);

    //remove the semaphore name by its string name
    sem_unlink("swim_semaphore");

    printf("Termination ends\n");

    //wait for all process to terminate properly
    sleep(5);

    exit(0);

}
int main(int argc, const char * argv[]) {
//    printf("-----Hello swimmill------");

    //create new semaphore with read and write permission
    //establish the connection between named semaphore and a process

    swim_semaphore = sem_open("/swim_semaphore", O_CREAT, 0666, 1);
    //check if the new semaphore fails
    if(swim_semaphore == SEM_FAILED){
        printf("fail to create semaphore");
        exit(1);
    }

    key_t key = 1335;
    int shmflg = 0666; //access permission read and write for everyone

    //this create a shared memory segment with key, size of the 2d array, and read and write permissions for all users. Returns an int of share memory id.
    shm_id = shmget(key, sizeof(char[10][10]), shmflg | IPC_CREAT );
    //printf("\nkey of shared memory is %d", shm_id);

    //attach grid to shared segment
    grid = shmat(shm_id, NULL,0);

    //convert shm_id as int to char
    char str[10];
    sprintf(str, "%d", shm_id);

    //checking for parent id
//    int parent_id = getpid();
//    printf("\nParent process attached at: %d\n", parent_id);

    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            grid[i][j] = '.';
        }
    }
    //make child processes using fork
    //printf("Create child process using fork\n");
    fish = fork();

    int timer = 0;
    if (fish == -1) {
        perror("fork error");
    } else if (fish == 0) {
        //printf("Im fish, child id is %d\n", getpid());

        //system call fish.c
        char *args[] = {"fish", str, NULL};
        execv("./fish", args);

    } else {
        while (timer < 30) {
            sleep(1);
            pellet = fork();

            if (pellet < 0) {
                perror("pellet");
                exit(1);
            }
            else if (pellet == 0) {
                //printf("Im pellet, child id is %d\n", getpid());
                char *args[] = {"pellet", str, NULL};
                execv("./pellet", args);

            }

            else {
                int status = 0;
                waitpid(pellet, &status, WNOHANG); //parent wait for child to finish
            }

            //decrement (lock) the semaphore to print to output
            sem_wait(swim_semaphore);
            showGrid(grid);
            write_to_file(grid);
            timer++;

            //increments (unlocks) the semaphore
            sem_post(swim_semaphore);

        }
    }
    terminate();
//    shmctl(shm_id, IPC_RMID,NULL);
    return 0;
}




