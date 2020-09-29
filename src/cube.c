#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include "cube.h"

int cube[6][3][3];

void printArray(int* in_p, int l){
    printf("[");
    for(int i=0; i<l; i++){
        printf("%d,", *(in_p+i));
    }
    printf("]\n");
}

void printCube(){
   printf("     _______\n");
   for(int r=0; r<3; r++){
       printf("     |%c,%c,%c|\n", cube[0][r][0],cube[0][r][1],cube[0][r][2]);
   } 
   printf(" _______________________\n");
   for(int r =0; r<3; r++){
       printf("|");
       for(int f=1; f<5; f++){
            if(f!=4){
                printf("%c,%c,%c|", cube[f][r][0], cube[f][r][1],cube[f][r][2]);
            }else{
                printf("%c,%c,%c|\n", cube[f][r][0], cube[f][r][1],cube[f][r][2]);
            }
       }
   }
   printf(" _______________________\n");
   for(int r=0; r<3; r++){
       printf("     |%c,%c,%c|\n", cube[5][r][0],cube[5][r][1],cube[5][r][2]);
   } 
   printf("     _______\n");
}

void initCube(){
    char colors[] = "YGOBRW";
    for(int i=0; i<6; i++){
        for(int r = 0; r<3; r++){
            for(int c=0; c<3; c++){
                cube[i][r][c] = *(colors + i);
            }
        }
    }
}

void initCubeFromString(char* input){
    int i = 0; 
    for(int f=0; f<6; f++){
        for(int r = 0; r<3; r++){
            for(int c=0; c<3; c++){
                cube[f][r][c] = *(input + i);
                i++;
            }
        }
    }
}

int face(char c){
    int face = -1;
    char colors[] = "ULFRBD";
    for(int i=0; i<6 && face == -1; i++){
        if(c == colors[i]){
            face = i;
        }
    }
    return face;
}

void rotateFace(char f, bool prime){
    int fint = face(f);
    if(prime){
        char temp = cube[fint][0][0];
        //corners
        cube[fint][0][0] = cube[fint][0][2];
        cube[fint][0][2] = cube[fint][2][2];
        cube[fint][2][2] = cube[fint][2][0];
        cube[fint][2][0] = temp;
        //edges
        temp = cube[fint][0][1];
        cube[fint][0][1] = cube[fint][1][2];
        cube[fint][1][2] = cube[fint][2][1];
        cube[fint][2][1] = cube[fint][1][0];
        cube[fint][1][0] = temp;

    }else{
        char temp = cube[fint][0][0];
        //corners
        cube[fint][0][0] = cube[fint][2][0];
        cube[fint][2][0] = cube[fint][2][2];
        cube[fint][2][2] = cube[fint][0][2];
        cube[fint][0][2] = temp;
        //edges
        temp = cube[fint][0][1];
        cube[fint][0][1] = cube[fint][1][0];
        cube[fint][1][0] = cube[fint][2][1];
        cube[fint][2][1] = cube[fint][1][2];
        cube[fint][1][2] = temp;
    }
}

int *getCol(char f, int col){
    int fint = face(f);
    int *col_p = malloc(sizeof(3));
    for(int i=0; i<3; i++){
        *(col_p+i) = cube[fint][i][col];
    }
    return col_p;
}

int *getRow(char f, int row){
    int fint = face(f);
    int *row_p = malloc(sizeof(3));
    for(int i=0; i<3; i++){
        *(row_p+i) = cube[fint][row][i];
    }
    return row_p;
}

void setRow(char f, int row, int*val){
    int fint = face(f);
    for(int i=0; i<3; i++){
        cube[fint][row][i] = *(val+i);
    }
}

void setCol(char f, int col, int*val){
    int fint = face(f);
    for(int i=0; i<3; i++){
        cube[fint][i][col] = val[i];
    }
}

int *invert(int *in){
    int temp = *in;
    *in = *(in+2);
    *(in+2) = temp;
    return in;
}

void turn(char* turn){
    int *t = NULL;
    int *temp = NULL;
    if(!strncmp(turn, "R", 2)){
        rotateFace('R', false);
        temp = getCol('F',2);
        t = getCol('D', 2);
        setCol('F', 2, t);
        free(t);
        t=invert(getCol('B', 0));
        setCol('D', 2, t);
        free(t);
        t= invert(getCol('U', 2));
        setCol('B', 0, invert(getCol('U', 2)));
        free(t);
        setCol('U', 2, temp);
    }else if(!strncmp(turn, "F", 2)){
        rotateFace('F', false);
        temp = getRow('U',2);
        t = invert(getCol('L', 2));
        setRow('U', 2,t );
        free(t);
        t = getRow('D', 0);
        setCol('L', 2, t);
        free(t);
        t = invert(getCol('R', 0));
        setRow('D', 0, t);
        free(t);
        setCol('R', 0, temp);
    }else if(!strncmp(turn, "L", 2)){
        rotateFace('L', false);
        temp = getCol('F',0);
        t = getCol('U', 0);
        setCol('F', 0, t);
        free(t);
        t = invert(getCol('B', 2));
        setCol('U', 0, t);
        free(t);
        t = invert(getCol('D', 0));
        setCol('B', 2, t);
        free(t);
        setCol('D', 0, temp);
    }else if(!strncmp(turn, "B", 2)){
        rotateFace('B', false);
        temp = getRow('U',0);
        t = getCol('R', 2);
        setRow('U', 0, t);
        free(t);
        t = invert(getRow('D', 2));
        setCol('R', 2, t);
        free(t);
        t = getCol('L', 0);
        setRow('D', 2, t);
        free(t);
        setCol('L', 0, invert(temp));
    }else if(!strncmp(turn, "U", 2)){
        rotateFace('U', false);
        temp = getRow('F',0);
        t = getRow('R', 0);
        setRow('F', 0, t);
        free(t);
        t = getRow('B', 0);
        setRow('R', 0, t);
        free(t);
        t = getRow('L', 0);
        setRow('B', 0, t);
        free(t);
        setRow('L', 0, temp);
    }else if(!strncmp(turn, "D", 2)){
        rotateFace('D', false);
        temp = getRow('F',2);
        t = getRow('L', 2);
        setRow('F', 2, t);
        free(t);
        t = getRow('B', 2);
        setRow('L', 2, t);
        free(t);
        t = getRow('R', 2);
        setRow('B', 2, getRow('R', 2));
        free(t);
        setRow('R', 2, temp);
    }
    //Prime
    else if(!strncmp(turn, "R'", 2)){
        rotateFace('R', true);
        temp = getCol('F',2);
        t = getCol('U', 2);
        setCol('F', 2, t);
        free(t);
        t = invert(getCol('B', 0));
        setCol('U', 2, t);
        free(t);
        t = invert(getCol('D', 2));
        setCol('B', 0, t);
        free(t);
        setCol('D', 2, temp);
    }else if(!strncmp(turn, "F'", 2)){
        rotateFace('F', true);
        temp = getRow('U',2);
        t = getCol('R', 0);
        setRow('U', 2, t);
        free(t);
        t = invert(getRow('D', 0));
        setCol('R', 0, t);
        free(t);
        t = getCol('L', 2);
        setRow('D', 0, t);
        free(t);
        setCol('L', 2, invert(temp));
    }else if(!strncmp(turn, "L'", 2)){
        rotateFace('L', true);
        temp = getCol('F',0);
        t = getCol('D', 0);
        setCol('F', 0, t);
        free(t);
        t = invert(getCol('B', 2));
        setCol('D', 0, t);
        free(t);
        t = invert(getCol('U', 0));
        setCol('B', 2, t);
        free(t);
        setCol('U', 0, temp);   
    }else if(!strncmp(turn, "B'", 2)){
        rotateFace('B', true);
        temp = getRow('U',0);
        t = invert(getCol('L', 0));
        setRow('U', 0, t);
        free(t);
        t = getRow('D', 2);
        setCol('L', 0, t);
        free(t);
        t = invert(getCol('R', 2));
        setRow('D', 2, t);
        free(t);
        setCol('R', 2, temp);
    }else if(!strncmp(turn, "U'", 2)){
        rotateFace('U', true);
        temp = getRow('F',0);
        t = getRow('L', 0);
        setRow('F', 0, t);
        free(t);
        t = getRow('B', 0);
        setRow('L', 0, t);
        free(t);
        t = getRow('R', 0);
        setRow('B', 0, t);
        free(t);
        setRow('R', 0, temp);   
    }else if(!strncmp(turn, "D'", 2)){
        rotateFace('D', true);
        temp = getRow('F',2);
        t = getRow('R', 2);
        setRow('F', 2, t);
        free(t);
        t = getRow('B', 2);
        setRow('R', 2, t);
        free(t);
        t = getRow('L', 2);
        setRow('B', 2, t);
        free(t);
        setRow('L', 2, temp);
    }
    //2x
    else if(!strncmp(turn, "R2", 2)){
        rotateFace('R', true);
        rotateFace('R', true);
        temp = getCol('U', 2);
        t = getCol('D', 2);
        setCol('U', 2, t);
        free(t);
        setCol('D', 2, temp);
        temp = getCol('F',2);
        t = invert(getCol('B', 0));
        setCol('F', 2, t);
        free(t);
        setCol('B', 0, invert(temp));
    }else if(!strncmp(turn, "F2", 2)){
        rotateFace('F', true);
        rotateFace('F', true);
        temp = getRow('U', 2);
        t = invert(getRow('D', 0));
        setRow('U', 2, t);
        free(t);
        setRow('D', 0, invert(temp));
        temp = getCol('L',2);
        t = invert(getCol('R', 0));
        setCol('L', 2, t);
        free(t);
        setCol('R', 0, invert(temp));
    }else if(!strncmp(turn, "L2", 2)){
        rotateFace('L', true);
        rotateFace('L', true);
        temp = getCol('U', 0);
        t = getCol('D', 0);
        setCol('U', 0, t);
        free(t);
        setCol('D', 0, temp);
        temp = getCol('F',0);
        t = invert(getCol('B', 2));
        setCol('F', 0, t);
        free(t);
        setCol('B', 2, invert(temp));
    }else if(!strncmp(turn, "B2", 2)){
        rotateFace('B', true);
        rotateFace('B', true);
        temp = getRow('U', 0);
        t = invert(getRow('D', 2));
        setRow('U', 0, t);
        free(t);
        setRow('D', 2, invert(temp));
        temp = getCol('L',0);
        t = invert(getCol('R', 2));
        setCol('L', 0, t);
        free(t);
        setCol('R', 2, invert(temp));
    }else if(!strncmp(turn, "U2", 2)){
        rotateFace('U', true);
        rotateFace('U', true);
        temp = getRow('F', 0);
        t = getRow('B', 0);
        setRow('F', 0, t);
        free(t);
        setRow('B', 0, temp);
        temp = getRow('L',0);
        t = getRow('R', 0);
        setRow('L', 0, t);
        free(t);
        setRow('R', 0, temp);
    }else if(!strncmp(turn, "D2", 2)){
        rotateFace('D', true);
        rotateFace('D', true);
        temp = getRow('F', 2);
        t = getRow('B', 2);
        setRow('F', 2, t);
        free(t);
        setRow('B', 2, temp);
        temp = getRow('L',2);
        t = getRow('R', 2);
        setRow('L', 2, t);
        free(t);
        setRow('R', 2, temp);
    }
    free(temp);
}

void executeSequence(char* s){
    char *seq = strdup(s);
    char* m = strtok(seq, " ");
    while(m  != NULL){
        turn(m);
        m = strtok(NULL, " ");
    }
    free(seq);
}

/**int main(){
    initCube();
    char sequence[] = "L2 D' U2 F' R' D2 F2 B2 L2 D U' R2 U2 L' U2 D' R2 U L2 R' U2 F' R2 F' R";
    executeSequence(sequence);
    printf("DONE\n");
    return EXIT_SUCCESS;
}**/