/**
 * CSI 3131 Assignment 3
 * 
 * Last Names: Casey, Choi
 * Student Numbers: 300291133, 3003282805
 * 
 * Description: 
 * This program will model a TA's office hours, taking the number of students <n> as an argument
 * There are two types of threads, the student threads and the TA threads.
 * The TA thread will nap until a student wakes it up for help, then the TA will help the student
 * for a random amount of time. After the TA is done helping the student, it will check 
 * if there are students waiting in the hallway. If there are, the TA will help the first student
 * in line, then it will check the hallway again. If there are no students, the TA will sleep again.
 * The student threads will each program for a random amount of time, then require help.
 * The student will check the TA. If the TA is sleeping, the student will wake it up and receive help. 
 * If the TA is helping another student, the student will go to the hallway. If one of the three chairs
 * is empty, the student will wait in it until the TA helps it. If none of the chairs are empty, the 
 * student will return to programming until it needs help again.
 * 
 * Usage: ./asg3 <n>
 */



 //four chairs, one in the office, one in the hallway, student knows to wake TA if all chairs are empty

 //the TA being woken up by a student should be done with a semaphore, according to the assignment description
 //pthread mutex locks are like a key that has to be passed between threads, only the thread that locks the mutex can unlock it
 //posix semaphores are like notifications, they can be released when none has been taken

//FULL WARNING I PROBABLY SPELLED SOMETHING WRONG 

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


//threads

sem_t TA_alarm_clock; //initiliase to 0, wakes the TA
sem_t TA_available; //initialise to 1, lets students know if the TA is free
sem_t assignment_help; //initialise to 0, lets students know when they're done receiving help
sem_t student_left_office; //initialise to 0, lets the TA know the student has emptied a chair
sem_t student_entered_office; //initialise to, lets the TA know a student has been chosen to receive help next

pthread_mutex_t chairs; //prevents chairs from being altered while being checked and preserves message order

int chairs_full = 0; //should only ever be between 0 and 4
int help_counter = 0; //allows program to end after a certain amount of help

int max_help = 5; //the certain amount of help that dictates the end of the program


void *TA_routine(){
    for (;;){
        sem_wait(&TA_alarm_clock); //waits for student to wake TA up

        pthread_mutex_lock(&chairs); //prevents the number of chairs filled changing while the TA checks
        printf("TA woken from nap\n");

        while(chairs_full > 0){ //while there are students needing help
            if (chairs_full == 1){
                printf("1 student requiring help from the TA\n");
                pthread_mutex_unlock(&chairs);

                int delay = (rand() % 6);
                sleep(delay); //helping student

                sem_post(&assignment_help); //tells student help is done
                sem_wait(&student_left_office); //wait until the student has vacated a chair
            }
            else if (chairs_full =< 4){
                printf("%d students requiring help from the TA\n", chairs_full);
                pthread_mutex_unlock(&chairs);

                sem_wait(&student_entered_office); //wait until a student is chosen to get help next

                int delay = (rand() % 6);
                sleep(delay); //helping the student

                sem_post(&assignment_help); //tells the student help is done
                sem_wait(&student_left_office); //wait until the student has vacated a chair
            }
            else { //more students than chairs
                pthread_mutex_unlock(&chairs);
                fprintf(stderr, "More students than chairs \n");
                exit(-1); //kill process
            }

            //all this happens everytime the TA finishes with one student
            sem_post(&TA_available); //tells any students in the hallway the office is empty
            //if none, allows student ability to wake TA later
            
            pthread_mutex_lock(&chairs);
            help_counter++;
            printf("TA has helped %d students\n", help_counter);

            if (help_counter == max_help){
                printf("TA has reached the end of office hours\n");
                exit(0); //kill process
            }
        }
        
        printf("No students requiring help from the TA. TA naps\n");
        pthread_mutex_unlock(&chairs);
    }
}

void *student_routine(void *student_no){
    for (;;){
        int delay = (srand(time(null)) % 8); //programming

        pthread_mutex_lock(&chairs); //needs help, looks at TA's office
        if (chairs_full == 0){ //no one being helped, TA asleep
            chairs_full++; //sit in chair
            printf("Student %d arrived and there were no students, so they awake the TA \n", student_no);
            pthread_mutex_unlock(&chairs);

            sem_wait(&TA_available); //wait until the TA is free
            //because its initialised to 1, the student can run right away
            sem_post(&TA_alarm_clock); //tell the TA to wake up

            sem_wait(&assignment_help); //wait for the TA to finish giving help

            pthread_mutex_lock(&chairs);
            printf("TA finished helping Student %d\n", student_no);
            chairs_full--; //exit chair
            sem_post(&student_left_office); //tell TA the chair is vacated
            pthread_mutex_unlock(&chairs);
        }

        else if (chairs_full < 4){ //there is at least one empty chair
            chairs_full++; //sit in chair
            printf("Student d% arrived and took a seat in the hallway\n", student_no);
            pthread_mutex_unlock(&chairs);

            sem_wait(&TA_available); //wait until the TA is free
            printf("Student %d enters the office\n", student_no);
            sem_post(&student_entered_office); //tell the TA the next student is ready to be helped

            sem_wait(&assignment_help); //wait for the TA to finish giving help

            pthread_mutex_lock(&chairs);
            printf("TA finished helping Student %d\n", student_no);
            chairs_full--; //vacate a chair
            sem_post(&student_left_office); //tell the TA the chair has been vacated
            pthread_mutex_unlock(&chairs);
        }

        else if (chairs_full == 4){ //there are no free chairs
            printf("Student %d arrived and there were no free chairs, so they left\n", student_no);
            pthread_mutex_unlock(&chairs);
            //goes to start of loop
        }

        else { //more students than chairs
            pthread_mutex_unlock(&chairs);
            fprintf(stderr, "More students than chairs \n");
            exit(-1); //kill process
        }
    }
}

int main (int ac, char **av) { 
    int number_of_students;

    if (ac == 2) {
        if (sscanf (av [1], "%d", &number_of_students) == 1) {
            args = av;
            printf("Number of students set to %d\n", number_of_students);
        }
        else fprintf(stderr, "Cannot translate argument\n");
    }
    else fprintf(stderr, "Invalid arguments\n");

    printf("The TA will run office hours until it has helped students %d times\n", max_help);

    //init semaphores
    sem_init(&TA_alarm_clock, 0, 0);
    sem_init (&TA_available, 0, 1);
    sem_init(&assignment_help, 0, 0);
    sem_init(&student_left_office, 0, 0);
    sem_init(&student_entered_office, 0);
    
    //init mutex lock
    pthread_mutex_init(&chairs, NULL);

    //create TA thread
    
    //create n student threads with 0-n as student id

    //run process until threads "end" (they will kill the process when one ends)
    pthread_join(TA_thread, NULL);

    for (int id = 0; id < number_of_students; id++){
        //join student threads
    }

    return (0);
}




