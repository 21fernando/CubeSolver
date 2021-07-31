#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>

long cube[6];

enum color {W,Y,G,B,O,R};

enum color colors[] = {W,Y,G,B,O,R};

char centers[] = {'W','Y','G','B','O','R'};

void initCube(){
    long face;
    for(int i=0; i<6; i++){
        face = 0;
        for(int j=0; j<8; j++){
            face += colors[i];
            face << 8;
        }
        cube[i] = face;
        printf("%d: %lu\n", i, face);
    }
}

void initCube(char* state_p){

}

int main(){
    printf("hello\n");
    printf("%lu", sizeof(long));
    initCube();
    return EXIT_SUCCESS;
}