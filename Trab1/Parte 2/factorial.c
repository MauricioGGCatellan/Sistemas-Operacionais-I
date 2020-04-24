#include <stdlib.h>
#include <stdio.h>

//Defines the maximum length of the result.
#define MAX_SIZE 65536

/* 
 * Because n! can be very large, this program stores the partial results as an array of chars of size MAX_SIZE.
 */

//multiplies an array of chars by an integer, result is put into the char array, returns -1 if the multiplication overflowed.
int mult(int, char*, int);

int main(int argc, char** argv){
    if (argc == 1){
        fprintf(stderr, "usage: ./factorial arg\n\targ: integer from 0 to 17235\n");
        return -1;
    }

    long arg = strtol(argv[1], NULL, 10);
    char result[MAX_SIZE];

    //Initializes the array with zeroes
    for (int i = 0; i < MAX_SIZE; i++)
        result[i] = 0;

    //starts at 1, with -1 marking the end of the number (for optimization).
    result[MAX_SIZE - 1] = 1;
    result[MAX_SIZE - 2] = -1;

    int overflow = 0;
    //Calculates n! checking for overflow.
    for(int i =  2; i <= arg && overflow == 0; i++){
        overflow = mult(i, result, MAX_SIZE);
    }
    if (overflow != 0){
        printf("Overflow\n");
        return 1;
    }

    printf("result is: ");

    //prints the result, using the flag to skip the zeroes.
    int flag = 0;
    for (int i = 0; i < MAX_SIZE; i++){
        if(flag == 1)
            printf("%c", result[i] +48);
        if(result[i] == -1)
            flag = 1;
    }
    printf("\n");

    return 0;
}

int mult(int a, char* res, int size){

    int end = 0;
    int carry = 0;
    int temp_res;

    //multiplies each char in res by a, from right to left.
    for (int i = size - 1; i >= 0; i--){

        if (res[i] == -1){ //reached the end of res, will continue until carry is 0;
            end = 1;
            res[i] = 0;
        }
        if (carry == 0 && end == 1){ //Passed the end and carry is 0, breaks out of the for after marking the new end.
            res[i] = -1;
            break;
        }
        temp_res = ((a * res[i]) + carry) % 10;
        carry = ((a * res[i]) + carry) / 10;
        res[i] = temp_res;
    }
    //no overflow, carry is 0;
    if (carry == 0)
        return 0;
    else return -1; //overflow
}   


