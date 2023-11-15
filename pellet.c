
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include "unistd.h"
#include <stdlib.h>
#include <MacTypes.h>
#include <semaphore.h>

const int fish_current_row = 9;
sem_t *swim_semaphore;
char(*grid)[10];

//Interrupt and Terminate headers
void Interrupt();
void Terminate();

void Interrupt(){
    printf("Interruption..");
    sem_close(swim_semaphore);

    //detach grid from memory
    shmdt(grid);
    exit(0);
}

void Terminate(){
    printf("Termination..");
    sem_close(swim_semaphore);
    shmdt(grid);
    exit(0);
}

int fish_current_column(char(*grid)[10]){
    int fish_col = 0;
    Boolean done = false;
    while(!done){
        if (grid[fish_current_row][fish_col] != 'F'){
            fish_col += 1;
        }
        else{
            done = true;
        }
    }
    return fish_col;

} //end of fish_current_pos()

//scan from bottom to top to move pellets
void move_pellet(char (*grid)[10]){
    time_t t;

    //creating a pellet at random position
    srand((unsigned) time(&t));
    int rand_pell_row = rand() % 9; //rand num 0 to 8. ignore last row.
    int rand_pell_col = rand() % 10; //ran num 0 to 9

    grid[rand_pell_row][rand_pell_col] = 'P';

    Boolean fish_exist = false;
    //if pellet is not at the last row. move the pellet downstream
    while (rand_pell_row < 9){
        //lock semaphore
        sem_wait(swim_semaphore);

        grid[rand_pell_row][rand_pell_col] = '.'; //set the location to . and move down 1 row
        rand_pell_row++;

        //if the pellet at the last row. check if fish also exist at the last row.
        if (rand_pell_row == 9){
            if (grid[rand_pell_row][rand_pell_col] == 'F'){ //if the fish is also at the bottom

                fish_exist = true; //fish and pellet collide here
//                printf("\nPELLET ID: %d", getpid());
//                printf("\nLocation: (%d,%d)", rand_pell_row, rand_pell_col);
                printf("\nPellet Eaten!");
            }
            else {
                grid[rand_pell_row][rand_pell_col] = 'P';
            }
        }
        // if the grid is not at the last row. mark them as P
        else{
            grid[rand_pell_row][rand_pell_col] = 'P';

        }
        //unlock semaphore after pellet made its move
        sem_post(swim_semaphore);
        sleep(1);


    }
    // the fish missed the pellet. Print out that the pellet is missed.
    if (!fish_exist){
        sem_wait(swim_semaphore);
        grid[rand_pell_row][rand_pell_col] = '.';
        printf("\nPellet Missed!");
//        printf("\nPELLET ID: %d", getpid());
//        printf("\nLocation: (%d,%d)", rand_pell_row, rand_pell_col);

    }
    sem_post(swim_semaphore);
    shmdt(grid);

}

int main(int argc, const char * argv[]) {

//    printf("\n-------Hello pellet-------\n");
    char (*grid)[10];
    key_t key = 1335;
    int shmflg = 0666; //access permission read and write for everyone


   // sscanf(argv[1], "%d", &shm);
   //get the shared memory segment id
    int shm_id = shmget(key, sizeof(char[10][10]), shmflg);

    //attach grid to the shared memory segment
    grid = shmat(shm_id, NULL, 0);

    //create new semaphore
    swim_semaphore = sem_open("/swim_semaphore", 0);
    //check if the new semaphore fails
    if(swim_semaphore == SEM_FAILED){
        printf("fail to create semaphore");
        exit(1);
    }

    //call function to move pellets
    move_pellet(grid);
//
//    signal(SIGINT, Interrupt);
//    signal(SIGTERM, Terminate);
    return 0;
}
