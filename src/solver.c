#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include "cube.h"

int solLen = 4;

typedef struct move_t{
    char self[3];
    char inverse[3];
    char dbl[3];
}move;

move moves[19];
move solution[100];

void initMoveStructs(){
    char selfs[18][3] = {"R","L","U","D","F","B","R'","L'","U'","D'","F'","B'","R2","L2","U2","D2","F2","B2"};
    char inverses[18][3] = {"R'","L'","U'","D'","F'","B'","R","L","U","D","F","B","R2","L2","U2","D2","F2","B2"};
    char dbls[18][3]={"R2","L2","U2","D2","F2","B2","R2","L2","U2","D2","F2","B2","N","N","N","N","N","N"};
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
    for(int i=0; i<solLen; i++){
        solution[i] = moves[18];
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

int genSeqs(int d, char condition[], move seq[], int moveNum){
    /**printf("CURRENT SEQUENCE:");
    for(int i=0; i<solLen; i++){
        printf("%s ", seq[i].self);
    }
    printf(".\n");**/
    if(matchCondition(condition)){
        printf("FOUND A SOLUTION: "); 
        /**int firstIndex = 0;
        while(solution[firstIndex].self[0] != 'N'){
            firstIndex++;
        }
        printf("FIRST INDEX: %d", firstIndex);
        for(int i=0; i<solLen; i++){
            if(strcmp(seq[i].self, "N")){
                solution[firstIndex+i] = seq[i];
                printf("%s", seq[i].self);
            }else{
                printf("SOMETHING IS SERIOUSLY WRONG");
            }  
        }
        printf(".\n");**/
        return moveNum;
    }else if (d==0){
        return -1;
    }else{
        for(int f=0; f<18; f++){
            //check move redundnacy here
            turn(moves[f].self);
            seq[moveNum] = moves[f];
            if(genSeqs(d-1, condition, seq, moveNum+1) > 0){
                return moveNum;
            }
            turn(moves[f].inverse);
        }
        return -1;
    }
}

int main(){
    initCube();
    initMoveStructs();
    char sequence[] = "B2 D' U2 B2 R' L' U' R L' F B' D2 F R B2 D2 R' D2 F2 R2 L F' R L2 U'";
    //Scramble the cube
    executeSequence(sequence);
    printCube();
    char solved[] = {"YYYYYYYYYGGGGGGGGGOOOOOOOOOBBBBBBBBBRRRRRRRRRWWWWWWWWW"};
    char oneCross[] =  {"~~~~Y~~~~~~~~G~~~~~~~~O~~O~~~~~B~~~~~~~~R~~~~~W~~W~~~~"};
    char twoCross[] =  {"~~~~Y~~~~~~~~G~~~~~~~~O~~O~~~~~B~~~~~~~~R~~R~~W~~W~~W~"};
    char threeCross[] ={"~~~~Y~~~~~~~~G~~G~~~~~O~~O~~~~~B~~~~~~~~R~~R~~W~WW~~W~"};
    char fullCross[] = {"~~~~Y~~~~~~~~G~~G~~~~~O~~O~~~~~B~~B~~~~~R~~R~~W~WWW~W~"};
    move arr[solLen];
    int moveNum = 0;
    //Clear arr
    for(int i=0; i<solLen; i++){
        arr[i] = moves[18];
    }
    //Solve the first cross peices
    int a = genSeqs(solLen, oneCross, arr, 0);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum].self);
        moveNum++;
    }
    printCube();
    //Clear arr
    for(int i=0; i<solLen; i++){
        arr[i] = moves[18];
    }
    //solves second cross peice
    a = genSeqs(solLen, twoCross, arr, 0);
    //Executes those moves
    for(int i=0; i<a; i++){
        turn(solution[moveNum].self);
        moveNum++;
    }
    printCube();
    
    for(int i=0; i<solLen; i++){
        arr[i] = moves[18];
    }
    genSeqs(solLen, threeCross, arr, 0);
    for(int i=0; i<solLen; i++){
        arr[i] = moves[18];
    }
    genSeqs(solLen, fullCross, arr, 0);
    return EXIT_SUCCESS;
}