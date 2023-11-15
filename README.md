[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-c66648af7eb3fe8bc4f294546bfd86ef473780cde1dea487d3c4ff354943c9ae.svg)](https://classroom.github.com/online_ide?assignment_repo_id=7692521&assignment_repo_type=AssignmentRepo)
# CECS 326 Lab 3: Semaphores and Message Passing

## Assignment Description

The goal of this assignment is to become familiar with [semaphores](https://man7.org/linux/man-pages/man7/sem_overview.7.html) in POSIX-based operating systems. You may have noticed that your fish might have acted a bit erratic! Your assignment is to modify `swim_mill` which you created with Lab #2 to include a semaphore to control access to critical resources.

Make sure that you have signal handing to terminate all processes, if needed. In case of abnormal termination, make sure to remove shared memory and semaphores as well. Use [semget(2)](https://man7.org/linux/man-pages/man2/semget.2.html), [semctl(2)](https://man7.org/linux/man-pages/man2/semctl.2.html), and [semop(2)](https://man7.org/linux/man-pages/man2/semop.2.html) to implement the [semaphore](https://man7.org/linux/man-pages/man7/sem_overview.7.html).

Your *semaphore* should receive the names of functions passed as parameters. Since different functions are required to access the critical resource, passing the functions as parameters will prove a clean option.

### Makefile

Like Lab #2, you need to include a [makefile](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor) to properly compile your assignment.

## Deliverables

I will require the following items for grading:

* Your `*.c` source code files
* Your *makefile*
* At least one screenshot of your executed code, in `*.png` or `*.jpg` format

Submit your files through your git repository. Your submission must follow the following rules, else *I will not grade it and you will receive a zero for the submission*:

* Do not use compression on your files
* Make sure that all significant code is *commented* with your own explanations
