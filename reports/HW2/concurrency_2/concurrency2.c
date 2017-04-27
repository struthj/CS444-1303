#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_PHILOSOPHERS 5
//globals for 5 philosophers, names, and forks
pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_t threads[NUM_PHILOSOPHERS];
char *names[NUM_PHILOSOPHERS] = {"Clavicus Vile",
                                  "Molag Mol",
                                  "Lydia",
                                 "Paarthurnax",
                                 "Kanye"
                                };


//philosopher will think before eating
void think(int profNum)
{
    //wait for 1-20 seconds while thinking
    int thinkTime = rand() % 20 + 1;
    printf("%s is Thinking for %d seconds\n", names[profNum], thinkTime);
    sleep(thinkTime);
}

//philosopher will eat for a number of seconds
void eat(int profNum)
{
    //wait for 2-9 seconds while eating
    int eatTime = rand() % 7 + 2;
    printf("%s is Eating for %d seconds\n", names[profNum], eatTime);
    sleep(eatTime);
    //finished eating
    printf("%s is Full!\n", names[profNum]);
}

//handle philosopher process eating and thinking
void* philosopher(void* n)
{
    int profNum = *((int*)n);
    while(1){
        //think before eating
        think(profNum);
        //grab forks from left or right to eliminate deadlock
        if(profNum % 2){
            //secure fork
            pthread_mutex_lock(&forks[profNum]);
            //secure adjacent fork
            pthread_mutex_lock(&forks[(profNum + 1) % NUM_PHILOSOPHERS]);
        } else {
            //secure adjacent fork
            pthread_mutex_lock(&forks[(profNum + 1) % NUM_PHILOSOPHERS]);
            //secure fork
            pthread_mutex_lock(&forks[profNum]);
        }
        //eat for a number of seconds
        eat(profNum);
        //give up fork
        pthread_mutex_unlock(&forks[(profNum + 1) % NUM_PHILOSOPHERS]);
        //give up other fork
        pthread_mutex_unlock(&forks[profNum]);
    }
}

int main()
{
    int i = 0;
    void* return_val;
    //initialize forks
    for(i = 0;i < NUM_PHILOSOPHERS;i++)
    {
        pthread_mutex_init(&forks[i], NULL);
    }
    //create philosophers
    for(i = 0;i < NUM_PHILOSOPHERS;i++)
    {
        pthread_create(&threads[i], NULL, philosopher, &i);
    }
    //join philosopher processes
    for(i = 0;i < NUM_PHILOSOPHERS;i++)
    {
        pthread_join(&threads[i], &return_val);
    }

    return 0;
}
