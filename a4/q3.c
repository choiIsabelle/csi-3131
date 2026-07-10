/*
Last Names: Casey, Choi
Student Numbers: 300291133, 3003282805

This program makes two processes talk to each other using shared memory.
The parent process is the producer and the child process is the consumer.

The producer makes the first N Catalan numbers (N is typed on the command line)
and puts them in the shared memory.
The consumer reads them back out of the shared memory and prints them.

The Catalan number formula is: Cn = (2n)! / ((n+1)! * n!)
The first ones (starting at n=1) are: 1, 2, 5, 14, 42, 132, ...

how to compile: gcc q3.c -o q3 -lrt
how to run: ./q3 5      (this makes the first 5 Catalan numbers)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* the name of the shared memory object */
#define SHM_NAME "/catalan_shm"

/* the biggest amount of Catalan numbers we allow.
   after this the numbers get too big for the number type. */
#define MAX_COUNT 20

/* this is the box that lives in shared memory.
   both processes can see it. */
struct shm_region {
    int count;                            /* how many numbers there are */
    int ready;                            /* 0 = not done yet, 1 = done */
    unsigned long long numbers[MAX_COUNT];/* the actual Catalan numbers */
};


/* this function just calculates a factorial the simple way.
   for example factorial(4) = 4 * 3 * 2 * 1 = 24 */
unsigned long long factorial(int x) {
    unsigned long long result = 1;
    int i;
    for (i = 1; i <= x; i = i + 1) {
        result = result * i;
    }
    return result;
}


/* this function calculates one Catalan number using the formula
   Cn = (2n)! / ((n+1)! * n!) */
unsigned long long catalan(int n) {
    unsigned long long top;
    unsigned long long bottom;
    unsigned long long answer;

    top = factorial(2 * n);
    bottom = factorial(n + 1) * factorial(n);
    answer = top / bottom;

    return answer;
}


int main(int argc, char *argv[]) {

    /* first we check that the user gave us exactly one number */
    if (argc != 2) {
        printf("usage: %s <count>\n", argv[0]);
        return 1;
    }

    /* turn the text argument into a real number */
    int count = atoi(argv[1]);

    /* make sure the number makes sense */
    if (count <= 0) {
        printf("error: please give a positive number\n");
        return 1;
    }
    if (count > MAX_COUNT) {
        printf("error: please give a number that is %d or smaller\n", MAX_COUNT);
        return 1;
    }

    /* ---- set up the shared memory ---- */

    /* create the shared memory object */
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        printf("error: could not make shared memory\n");
        return 1;
    }

    /* make the shared memory big enough to hold our box */
    int size = sizeof(struct shm_region);
    ftruncate(fd, size);

    /* connect the shared memory into our program so we can use it */
    struct shm_region *shm;
    shm = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        printf("error: could not connect to shared memory\n");
        return 1;
    }

    /* at the start the data is not ready yet */
    shm->ready = 0;

    /* ---- split into two processes ---- */
    pid_t pid = fork();

    if (pid > 0) {
        /* ==================== PRODUCER (parent) ==================== */

        int n;
        /* go through each Catalan number we need to make */
        for (n = 1; n <= count; n = n + 1) {
            /* work out this Catalan number */
            unsigned long long value = catalan(n);
            /* store it in the shared memory.
               we use n-1 because arrays start at position 0 */
            shm->numbers[n - 1] = value;
        }

        /* remember how many we made */
        shm->count = count;

        /* now tell the consumer that everything is ready */
        shm->ready = 1;

        printf("producer: I made the first %d Catalan number(s)\n", count);

        /* wait for the child (consumer) to finish before we clean up */
        wait(NULL);

        /* clean up the shared memory */
        munmap(shm, size);
        close(fd);
        shm_unlink(SHM_NAME);

    } else {
        /* ==================== CONSUMER (child) ==================== */

        /* keep waiting until the producer says the data is ready */
        while (shm->ready == 0) {
            usleep(1000);
        }

        printf("consumer: I read %d Catalan number(s) from shared memory:\n",
               shm->count);

        /* print out every number one at a time */
        int i;
        for (i = 0; i < shm->count; i = i + 1) {
            printf("C%d = %llu\n", i + 1, shm->numbers[i]);
        }

        /* disconnect from the shared memory */
        munmap(shm, size);
        close(fd);
    }

    return 0;
}
