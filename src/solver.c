#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include "cube.h"

typedef struct move_t{
    char self[3];
    char inverse[3];
    char dbl[3];
}move;

move moves[19];
char solution[100][3];
char rightEdgeInsert[] = "U R U R' U' F' U' F";
char leftEdgeInsert[] = "U' L' U' L U F U F'";

void updateSolution(const char* in);

void initSolver(){
    char selfs[18][3] = {"R","L","U","D","F","B","R'","L'","U'","D'","F'","B'","R2","L2","U2","D2","F2","B2"};
    char inverses[18][3] = {"R'","L'","U'","D'","F'","B'","R","L","U","D","F","B","R2","L2","U2","D2","F2","B2"};
    char dbls[18][3]={"R2","L2","U2","D2","F2","B2","R2","L2","U2","D2","F2","B2","R","L","U","D","F","B"};
    for(int i=0; i<18; i++){
        move a;
        strcpy(a.self, selfs[i]);
        strcpy(a.inverse, inverses[i]);
        strcpy(a.dbl, dbls[i]);
        moves[i] = a;
    }
    move a;
    strcpy(a.self, "N");
    strcpy(a.inverse, "N");
    strcpy(a.dbl, "N");
    moves[18] = a;
    for(int i=0; i<100; i++){
        strcpy(solution[i], "~~");
    }
}

bool matchCondition(char* condition){
    int i=0;
    for(int f = 0; f<6; f++){
        for(int r = 0; r<3; r++){
            for(int c = 0; c<3; c++){
                if(condition[i] != '~' && cube[f][r][c] != condition[i]){
                    return false;
                }
                i++;
            }
        }
    }
    return true;
}

int genSeqs(int d, char condition[], move seq[], int moveNum, int solLen){
    /**printf("CURRENT SEQUENCE:");
    for(int i=0; i<solLen; i++){
        printf("%s ", seq[i].self);
    }
    printf(".\n");**/
    if(matchCondition(condition)){
        //printf("FOUND A SOLUTION: "); 
        int firstIndex = 0;
        while(solution[firstIndex][0] != '~'){
            firstIndex++;
        }
        for(int i=0; i<solLen; i++){
            if(strcmp(seq[i].self, "N")){
                strcpy(solution[firstIndex+i], seq[i].self);
                //printf("%s", seq[i].self);
            }
        }
        //printf(".\n");
        return moveNum;
    }else if (d==0){
        return -1;
    }else{
        for(int f=0; f<18; f++){
            //check move redundnacy here
            if(moveNum == 0 || (strcmp(moves[f].dbl, seq[moveNum-1].self)!= 0 
                &&strcmp(moves[f].inverse, seq[moveNum-1].self)!= 0 
                &&strcmp(moves[f].self, seq[moveNum-1].self)!= 0)){
                turn(moves[f].self);
                seq[moveNum] = moves[f];
                if(genSeqs(d-1, condition, seq, moveNum+1,solLen) > 0){
                    return moveNum;
                }
                turn(moves[f].inverse);
            }
        }
        return -1;
    }
}

void solveCross(int solLen){
    char solved[] = {"YYYYYYYYYGGGGGGGGGOOOOOOOOOBBBBBBBBBRRRRRRRRRWWWWWWWWW"};
    char oneCross[] =  {"~~~~Y~~~~~~~~G~~~~~~~~O~~O~~~~~B~~~~~~~~R~~~~~W~~W~~~~"};
    char twoCross[] =  {"~~~~Y~~~~~~~~G~~~~~~~~O~~O~~~~~B~~~~~~~~R~~R~~W~~W~~W~"};
    char threeCross[] ={"~~~~Y~~~~~~~~G~~G~~~~~O~~O~~~~~B~~~~~~~~R~~R~~W~WW~~W~"};
    char fullCross[] = {"~~~~Y~~~~~~~~G~~G~~~~~O~~O~~~~~B~~B~~~~~R~~R~~W~WWW~W~"};
    move arr[solLen];
    int moveNum = 0;
    for(int i=0; i<100; i++){
        strcpy(solution[i], "~~");
    }
    //Clear arr
    for(int i=0; i<solLen; i++){
        move a;
        strcpy(a.self, "N");
        arr[i] = a;
    }
    //Solve the first cross peices
    int a = genSeqs(solLen, oneCross, arr, 0,solLen);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum]);
        moveNum++;
    }
    //printCube();
    //Clear arr
    for(int i=0; i<solLen; i++){
        move a;
        strcpy(a.self, "N");
        arr[i] = a;
    }
    //solves second cross peice
    a = genSeqs(solLen, twoCross, arr, 0,solLen);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum]);
        moveNum++;
    }
    //printCube();
    //Clear arr
    for(int i=0; i<solLen; i++){
        move a;
        strcpy(a.self, "N");
        arr[i] = a;
    }
    //solves third cross peice
    a = genSeqs(solLen, threeCross, arr, 0,solLen);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum]);
        moveNum++;
    }
    //printCube();
    //Clear arr
    for(int i=0; i<solLen; i++){
        move a;
        strcpy(a.self, "N");
        arr[i] = a;
    }
    //solves full cross 
    a = genSeqs(solLen, fullCross, arr, 0,solLen);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum]);
        moveNum++;
    }
}

void solveCorners(int solLen){
    char solved[] = {"YYYYYYYYYGGGGGGGGGOOOOOOOOOBBBBBBBBBRRRRRRRRRWWWWWWWWW"};
    char oneCorner[] =  {"~~~~Y~~~~~~~~G~~GG~~~~O~OO~~~~~B~~B~~~~~R~~R~WW~WWW~W~"};
    char twoCorner[] =  {"~~~~Y~~~~~~~~G~~GG~~~~O~OOO~~~~B~BB~~~~~R~~R~WWWWWW~W~"};
    char threeCorner[] ={"~~~~Y~~~~~~~~G~GGG~~~~O~OOO~~~~B~BB~~~~~R~~RRWWWWWWWW~"};
    char fullCorner[] = {"~~~~Y~~~~~~~~G~GGG~~~~O~OOO~~~~B~BBB~~~~R~RRRWWWWWWWWW"};
    move arr[solLen];
    int moveNum = 0;
    //Clear arr
    for(int i=0; i<solLen; i++){
        move a;
        strcpy(a.self, "N");
        arr[i] = a;
    }
    //Solve the first cross peices
    int a = genSeqs(solLen, oneCorner, arr, 0,solLen);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum]);
        moveNum++;
    }
    //printCube();
    //Clear arr
    for(int i=0; i<solLen; i++){
        move a;
        strcpy(a.self, "N");
        arr[i] = a;
    }
    //solves second cross peice
    a = genSeqs(solLen, twoCorner, arr, 0,solLen);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum]);
        moveNum++;
    }
    //printCube();
    //Clear arr
    for(int i=0; i<solLen; i++){
        move a;
        strcpy(a.self, "N");
        arr[i] = a;
    }
    //solves third cross peice
    a = genSeqs(solLen, threeCorner, arr, 0,solLen);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum]);
        moveNum++;
    }
    //printCube();
    //Clear arr
    for(int i=0; i<solLen; i++){
        move a;
        strcpy(a.self, "N");
        arr[i] = a;
    }
    //solves full cross 
    a = genSeqs(solLen, fullCorner, arr, 0,solLen);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum]);
        moveNum++;
    }
}

int indexOf(char* str, char c){
    for(int i=0; i<strlen(str); i++){
        if(*(str+i) == c){
            return i;
        }
    }
    return -1;
}

char* rotateAlgorithm(char *alg, char f){
    char *algorithm = strdup(alg);
    int len = strlen(algorithm);
    for(int i=0; i< len; i++){
        if(algorithm[i] == 'L'){
            if(f == 'L'){
                algorithm[i] = 'B';
            }else if(f == 'R'){
                algorithm[i] = 'F';
            }else if(f == 'F'){
                algorithm[i] = 'L';
            }else if(f == 'B'){
                algorithm[i] = 'R';
            }
        }else if(algorithm[i] == 'R'){
            if(f == 'L'){
                algorithm[i] = 'F';
            }else if(f == 'R'){
                algorithm[i] = 'B';
            }else if(f == 'F'){
                algorithm[i] = 'R';
            }else if(f == 'B'){
                algorithm[i] = 'L';
            }
        }else if(algorithm[i] == 'F'){
            if(f == 'L'){
                algorithm[i] = 'L';
            }else if(f == 'R'){
                algorithm[i] = 'R';
            }else if(f == 'F'){
                algorithm[i] = 'F';
            }else if(f == 'B'){
                algorithm[i] = 'B';
            }
        }else if(algorithm[i] == 'B'){
            if(f == 'L'){
                algorithm[i] = 'R';
            }else if(f == 'R'){
                algorithm[i] = 'L';
            }else if(f == 'F'){
                algorithm[i] = 'B';
            }else if(f == 'B'){
                algorithm[i] = 'F';
            }
        }
    }
    return algorithm;
}

void solveEdges(){    
    //Red Blue
    //if it is not solved
    if(cube[face('R')][1][0] != 'R' || cube[face('B')][1][2] != 'B'){
        char seq[20];
        seq[0] = '~';
        //if peice is on left face
        if((cube[1][1][0] == 'R' && cube[4][1][2] == 'B') || (cube[1][1][0] == 'B' && cube[4][1][2] == 'R')){
            strcpy(seq,rotateAlgorithm(leftEdgeInsert, 'L'));
        }
        //if peice is on front face
        if((cube[2][1][0] == 'R' && cube[1][1][2] == 'B') || (cube[2][1][0] == 'B' && cube[1][1][2] == 'R')){
            strcpy(seq,rotateAlgorithm(leftEdgeInsert, 'F'));
        }
        //if peice is on left face
        if((cube[3][1][0] == 'R' && cube[2][1][2] == 'B') || (cube[3][1][0] == 'B' && cube[2][1][2] == 'R')){
            strcpy(seq,rotateAlgorithm(leftEdgeInsert, 'R'));
        }
        //if the edge piece is in there but flipped:
        if(cube[4][1][0] == 'B' && cube[3][1][2] == 'R' ){
            strcpy(seq,rotateAlgorithm(leftEdgeInsert, 'B'));
        }
        if(seq[0] != '~'){
            executeSequence(seq);
            updateSolution(seq);
        }
        //Now edge is on top, determine how to align and insert
        char alignMove[3];
        bool isLeftInsert;
        if(cube[0][1][0] == 'R' && cube[1][0][1] == 'B'){
            strcpy(alignMove, "U2");
            isLeftInsert = false;
        }else if(cube[0][1][0] == 'B' && cube[1][0][1] == 'R'){
            strcpy(alignMove, "U");
            isLeftInsert = true;
        }else if(cube[0][2][1] == 'R' && cube[2][0][1] == 'B'){
            strcpy(alignMove, "U'");
            isLeftInsert = false;
        }else if(cube[0][2][1] == 'B' && cube[2][0][1] == 'R'){
            strcpy(alignMove, "U2");
            isLeftInsert = true;
        }else if(cube[0][1][2] == 'R' && cube[3][0][1] == 'B'){
            strcpy(alignMove, "N");
            isLeftInsert = false;
        }else if(cube[0][1][2] == 'B' && cube[3][0][1] == 'R'){
            strcpy(alignMove, "U'");
            isLeftInsert = true;
        }else if(cube[0][0][1] == 'R' && cube[4][0][1] == 'B'){
            strcpy(alignMove, "U");
            isLeftInsert = false;
        }else if(cube[0][0][1] == 'B' && cube[4][0][1] == 'R'){
            strcpy(alignMove, "N");
            isLeftInsert = true;
        }
        //if it isnt already aligned
        if(strncmp(alignMove,"N", 1)){
            turn(alignMove);
            updateSolution(alignMove);
        }
        if(isLeftInsert){
            strcpy(seq,rotateAlgorithm(leftEdgeInsert,'B'));
        }else{
            strcpy(seq,rotateAlgorithm(leftEdgeInsert,'R'));
        }
        executeSequence(seq);
        updateSolution(seq);
    }
}

void updateSolution(const char* in){
    char *input;
    input = strdup(in);
    int firstIndex = 0;
    while(solution[firstIndex][0] != '~'){
        firstIndex++;
    }
    int c=0;
    char* m = strtok(input, " ");
    while(m  != NULL){
        strcpy(solution[firstIndex+c], m);
        c++;
        m = strtok(NULL, " ");
    }
}

void printSolution(){
    int c=0;
    printf("Solution: ");
    while(solution[c][0] != '~'){
        printf("%s, ", solution[c]);
        c++;
    }
    printf(".\n");
}

int main(){
    initCube();
    //char state[] = "YYYYYYYYYGGGGGGGGGOOOOOOOOOBBBBBBBBBRRRRRRRRRWWWWWWWWW";
    //initCubeFromString(state);
    initSolver();
    printCube();
    char scramble[] = "F' U2 R2 L D R2 F2 D2 B U' L D U' R' L' F' L2 D' U R' D2 R F2 R' F";
    executeSequence(scramble);
    printCube();
    solveCross(4);
    printSolution();
    //printSolution();
    solveCorners(6);
    printSolution();
    solveEdges();
    printSolution();
    return EXIT_SUCCESS;
}