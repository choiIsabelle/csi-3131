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



 //the chairs will be a shared three-person array with a pointer to the front and back of the line
 //if there are empty chairs the back of the line will point to an empty chair

 //the TA being woken up by a student should be done with a semaphore, according to the assignment description
 //the TA could release the semaphore when asleep, a student wanting help could take the semaphore, wake the TA up,
 //the student would release the semaphore to the TA, TA will take it until it can sleep again