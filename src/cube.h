#ifndef CUBE_H_
#define CUBE_H_

void printCube();
void initCube();
void initCubeFromString(char* input);
void rotateFace(char face, bool prime);
int *getCol(char face, int col);
int *getRow(char face, int row);
void setRow(char face, int row, int*val);
void setCol(char face, int col, int*val);
int *invert(int *in);
void turn(char* move);
int face(char f);
void executeSequence(char* seq);

int cube[6][3][3];

#endif