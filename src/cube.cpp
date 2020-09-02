#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cube[6][3][3];

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
    char colors[] = "RBWGYO";
    for(int i=0; i<6; i++){
        for(int r = 0; r<3; r++){
            for(int c=0; c<3; c++){
                cube[i][r][c] = *(colors + i);
            }
        }
    }
}

void move(char* move){
    if(!strncmp(move, "R", 2)){
    
    }else if(!strncmp(move, "F", 2)){
       
    }else if(!strncmp(move, "L", 2)){
        
    }else if(!strncmp(move, "B", 2)){
        
    }else if(!strncmp(move, "U", 2)){
        
    }else if(!strncmp(move, "D", 2)){
       
    }else if(!strncmp(move, "R'", 2)){
        
    }else if(!strncmp(move, "F'", 2)){
        
    }else if(!strncmp(move, "L'", 2)){
        
    }else if(!strncmp(move, "B'", 2)){
        
    }else if(!strncmp(move, "U'", 2)){
        
    }else if(!strncmp(move, "D'", 2)){
        
    }else if(!strncmp(move, "R2", 2)){
        
    }else if(!strncmp(move, "F2", 2)){
        
    }else if(!strncmp(move, "L2", 2)){
        
    }else if(!strncmp(move, "B2", 2)){
        
    }else if(!strncmp(move, "U2", 2)){
        
    }else if(!strncmp(move, "D2", 2)){
        
    }
}



int main(){
    initCube();
    printCube();
}