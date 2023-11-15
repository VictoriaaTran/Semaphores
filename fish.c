

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <math.h>
#include <MacTypes.h>
#include <semaphore.h>
#include <signal.h>

const int fish_current_row = 9;
sem_t *swim_semaphore;
char(*grid)[10];

/*location the fish current position in the grid. return its position in the column*/
int fish_current_column(char(*grid)[10]){
//    sem_wait(swim_semaphore);
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
//    sem_post(swim_semaphore);
    return fish_col;

} //end of fish_current_pos()


/*
 * Function to find the closest pellet to fish
 */
void closest_pellet(int *closest_pell_row, int *closest_pell_col, char (*grid)[10]){
    double smallestDis = 0;
    double distance;
    int fish_current_col = fish_current_column(grid);
    Boolean isSmallest = false;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {

            //find the 1st pellet and set it as the smallest distance
            if (grid[i][j] == 'P' && isSmallest == false) {
                distance = sqrt(pow(i - fish_current_row, 2) + pow(j - fish_current_col, 2));
                smallestDis = distance;
                *closest_pell_row = i;
                *closest_pell_col = j;
                isSmallest = true;
            }
            //compare the closest distance. if it is the smallest, set it as the closest pellet so fish can move.
            if (grid[i][j] == 'P' && isSmallest){
                int temp_distance = sqrt(pow(i - fish_current_row, 2) + pow(j - fish_current_col, 2));
                if (temp_distance < smallestDis){
                    smallestDis = temp_distance;
                    *closest_pell_row = i;
                    *closest_pell_col = j;
                }

            }
        }
    }


} //end of closest_pellet()

/*
 * Function to move fish accordingly to the closest pellet position
 */
void move_fish(int closest_pell_row, int closest_pell_col, char(*grid)[10]){
    //decrements (locks) the semaphore for the fish to do its job
    //If the semaphore's value is greater than zero, then the decrement proceeds
    sem_wait(swim_semaphore);

    int fish_col = fish_current_column(grid);
    printf("%d", fish_col);
    printf("%d",closest_pell_col);
    if (fish_col < closest_pell_col){
        grid[fish_current_row][fish_col] = '.';
        grid[fish_current_row][fish_col+1] = 'F';

    }
    else if (fish_col > closest_pell_col){
        grid[fish_current_row][fish_col] = '.';
        grid[fish_current_row][fish_col-1] = 'F';
    }
    else{
        //fish stays as it is. pellet is above
    }

    //if pellet is on row 8. on top of the fish
    if ((fish_current_row - 1) == closest_pell_row){
        //eat the pellet
        grid[closest_pell_row][closest_pell_col] = '.';
    }

//  increments (unlocks) the semaphore
    sem_post(swim_semaphore);
}

//Interrupt and Terminate headers
void Interrupt();
void Terminate();

//void Interrupt(){
//    printf("Interruption..");
//    sem_close(swim_semaphore);
//
//    //detach grid from memory
//    shmdt(grid);
//    exit(0);
//}
//
//void Terminate(){
//    //close the semaphore and detach the grid from memory
//    printf("Termination..");
//    sem_close(swim_semaphore);
//    shmdt(grid);
//    exit(0);
//}

int main(int argc, char *argv[]) {

    char (*grid)[10];
    key_t key = 1335;
    int shmflg = 0666; //access permission read and write for everyone

//    printf("\n-------Hello fish--------\n");

    //convert char into int
    //sscanf(argv[1], "%d", &shm);
    //   printf("\nkey of shared mem: %d", shm);

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


    //put a fish at the bottom of the grid somewhere
    //check to make sure that we only create fish once
    Boolean fish_exist = false;
    for (int i = 0; i < 10; i++) {
        if (grid[fish_current_row][i] == 'F') {
            fish_exist = true;
        }
    }
    if (!fish_exist) {
        int fish_current_col = 6;
        grid[fish_current_row][fish_current_col] = 'F';
    }

    //hunt for the closest pellet and move the Fish accordingly
    while (true){
        sleep(1);
        int closest_pel_row;
        int closest_pel_col;
        closest_pellet(&closest_pel_row, &closest_pel_col, grid);
        move_fish(closest_pel_row, closest_pel_col, grid);
    }

    signal(SIGINT, Interrupt);
    signal(SIGTERM, Terminate);

    return 0;

} //end of main()









