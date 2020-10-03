#include <Arduino.h>
#include <PololuMaestro.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#define WALL_MIN_POSITION 736l
#define WALL_MID_POSITION 1673l
#define WALL_MAX_POSITION 2640l
#define CLAW_OPEN_POSITION 2640l
#define CLAW_CLOSED_POSITION 1792l
#define CLAW_SPEED 0
#define CLAW_ACC 0
#define WALL_SPEED 100 //300
#define WALL_ACC 40 
#define CLAW_OPEN 0 
#define CLAW_CLOSED 1
#define WALL_MIN 0
#define WALL_MID 1 
#define WALL_MAX 2

/**
 * R, R', R2, L, L', L2, F, F', F2, B, B', B2
 * X, X', X2, Y, Y', Y2
 * All rotations will be assuming F is on arm 1
 * MAX --> MIN is clockwise
 **/

MiniMaestro maestro(Serial1);

typedef struct arm_t{
  int id;
  char face;
  char absFace;
  int clawPin;
  int wallPin;
  int wallState;
  int clawState;
  arm_t *oppositeArm;
  arm_t *leftArm;
  arm_t *rightArm;
} arm;

typedef struct cube_t{
  int f[], b[], l[], r[], u[], d[];
  char up, down, front, back, left, right; 
} rubiksCube;

void moveServo(int channel, int pos);
void moveClaw(arm *a, int pos);
void moveWall(arm *a, int pos);
void moveWalls(arm *a, uint16_t *pos);
void moveClaws(arm *a, int pos);
void testAllRom();
void wait();
void setSpeeds();
void loadInCube();
void executeSequence(char* seq);
void executeMove(char* move);
int numMoves(char* seq);
void parse(char* seq, char** moveArr);
void printArmStates();
void defineAdjacencies();
void executeRotation(arm *a, int num);
void executeTurn(arm *a, int num);
void defineCube();
void translateMove(char *in, char *out);

static arm arms[4];
static rubiksCube cube;
arm declareArm(int id, int clawChannel, int wallChannel, char face);

void setup() {
  //initializing serial
  Serial.begin(9600);
  Serial1.begin(115200);
  //Initializing arm structs
  arms[0] = declareArm(1, 0, 6, 'F');
  arms[1] = declareArm(2, 2, 8, 'L');
  arms[2] = declareArm(3, 1, 7, 'B');
  arms[3] = declareArm(4, 3, 9, 'R');
  defineAdjacencies();
  //initializing cube
  defineCube();
  //moving servos
  setSpeeds();
  moveWall(&arms[0], WALL_MAX);
  moveWall(&arms[2], WALL_MAX);
  moveWall(&arms[1], WALL_MAX);
  moveWall(&arms[3], WALL_MAX);
  moveWall(&arms[0], WALL_MID);
  moveWall(&arms[2], WALL_MID);
  for(int i=0; i<4; i++){
    moveClaw(&arms[i], CLAW_OPEN);
  }
  Serial.println("SERVOS SET");
  Serial.println("SERIAL READY");
  loadInCube();
  Serial.println("CUBE INSERTED");
  printArmStates();
  Serial.flush();
  char sequence[] = "D B D R U2 F' D R U' L' U' R2 U2 F2 L2 B2 U' B2 D B2 RC";
  executeSequence(sequence);
}

void loop() {
  return;
}

void moveServo(int channel, int pos){
  wait();
  maestro.setTarget(channel, pos*4);
}

void moveServos(int channel, uint16_t* pos){
  wait(); 
  pos[0] *= 4;
  pos[1] *= 4;
  maestro.setMultiTarget(2, channel, pos);
}

void moveClaw(arm *a, int pos){
  if(pos == CLAW_OPEN){
    moveServo(a->clawPin, CLAW_OPEN_POSITION);
    a->clawState = CLAW_OPEN;
  }else{
    moveServo(a->clawPin, CLAW_CLOSED_POSITION);
    a->clawState = CLAW_CLOSED;
  }
}

void moveWall(arm *a, int pos){
  if(pos == 0){
    moveServo(a->wallPin, WALL_MIN_POSITION);
    a->wallState = WALL_MIN;
  }else if(pos == 1){
    moveServo(a->wallPin, WALL_MID_POSITION);
    a->wallState = WALL_MID;
  }else{
    moveServo(a->wallPin, WALL_MAX_POSITION);
    a->wallState = WALL_MAX;
  }
}

void moveWalls(arm *a, uint16_t *pos){
  a->wallState = pos[0];
  a->oppositeArm->wallState = pos[1];
  if(pos[0] == WALL_MIN){
    pos[0] = WALL_MIN_POSITION;
  }else if (pos[0] == WALL_MID){
    pos[0] = WALL_MID_POSITION;
  }else{
    pos[0] = WALL_MAX_POSITION;
  }
  if(pos[1] == WALL_MIN){
    pos[1] = WALL_MIN_POSITION;
  }else if (pos[1] == WALL_MID){
    pos[1] = WALL_MID_POSITION;
  }else{
    pos[1] = WALL_MAX_POSITION;
  }
  moveServos(a->wallPin, pos);
}

void moveClaws(arm *a , int pos){
  uint16_t positions[2];
  if(pos == CLAW_CLOSED){
    positions[0] = CLAW_CLOSED_POSITION;
    positions[1] = CLAW_CLOSED_POSITION;
    a->clawState = CLAW_CLOSED;
    a->oppositeArm->clawState = CLAW_CLOSED;
  }else{
    positions[0] = CLAW_OPEN_POSITION;
    positions[1] = CLAW_OPEN_POSITION;
    a->clawState = CLAW_OPEN;
    a->oppositeArm->clawState = CLAW_OPEN;
  }
  moveServos(a->clawPin, positions);
}

void wait(){
  while(maestro.getMovingState() == 1){
    //Serial.println("Waiting");
  }
  //Serial.println("Movement Complete, Moving to next step");
  delay(500);
  
}

void setSpeeds(){
  for(int i=0; i<4; i++){
    maestro.setSpeed(arms[i].clawPin, CLAW_SPEED);
    maestro.setAcceleration(arms[i].clawPin, CLAW_ACC);
    maestro.setSpeed(arms[i].wallPin, WALL_SPEED);
    maestro.setAcceleration(arms[i].wallPin, WALL_ACC);
  }
}

arm declareArm(int i, int c, int w, char f){
  arm temp;
  temp.id = i;
  temp.clawPin = c;
  temp.wallPin = w;
  temp.face = f;
  temp.absFace = f;
  return temp;
}
  
int numMoves(char* seq){
  int i =0;
  int length = 1;
  while(seq[i] != '\0'){
    if(seq[i] == ' ' && seq[i+1] != '\0' && seq[i+1] != ' '){
      length++;
    }
    i++;
  }
  return length;
}

void parse(char* seq, char** moveArr){
  int i=0;
  char *p = strtok(seq, " ");
  while(p != NULL){
    moveArr[i++] = p;
    p = strtok(NULL, " ");
  }
}

void loadInCube(){
  int num = 5;
  char input;
  int current = 0;
  while(current<num){
    if(Serial.available()){
      input = Serial.read();
      switch(current){
        case 0:
          moveClaw(&arms[0], CLAW_CLOSED);
          break;
        case 1:
          moveClaw(&arms[2], CLAW_CLOSED);
          break;
        case 2:{
            uint16_t pos[2] = {WALL_MAX, WALL_MIN};
            moveWalls(&arms[0], pos);
          }
          break;
        case 3:
          moveClaws(&arms[1], CLAW_CLOSED);
          break;
      }
      current ++;
    }
  }
}

void executeSequence(char* seq){
  int num = numMoves(seq);
  char *moves[num];
  parse(seq, moves);
  char input;
  int current = 0;
  while(current<num){
    if(Serial.available()){
      input = Serial.read();
      char adjusted[5];
      translateMove(moves[current], adjusted);
      int num = numMoves(adjusted);
      char formatted[100];
      sprintf(formatted, "Adjusted: %s, Num moves: %d", adjusted, num);
      Serial.print(formatted);
      char *adjustedMoves[num];
      parse(adjusted, adjustedMoves);
      if(num == 1){
        executeMove(adjustedMoves[0]);
      }else{
        executeMove(adjustedMoves[0]);
        printArmStates();
        executeMove(adjustedMoves[1]);
      }
      current ++;
      printArmStates();
    }
  }
}

void executeMove(char *move){
  if(!strncmp(move, "R", 2)){
    Serial.println("EXECUTING: R");
    executeTurn(&arms[3], 1);
  }else if(!strncmp(move, "F", 2)){
    Serial.println("EXECUTING: F");
    executeTurn(&arms[0], 1);
  }else if(!strncmp(move, "L", 2)){
    Serial.println("EXECUTING: L");
    executeTurn(&arms[1], 1);
  }else if(!strncmp(move, "B", 2)){
    Serial.println("EXECUTING: B");
    executeTurn(&arms[2], 1);
  }else if(!strncmp(move, "X", 2)){
    Serial.println("EXECUTING: X");
    //clockwise turn with F and B claws
    executeRotation(&arms[0],1);
  }else if(!strncmp(move, "Y", 2)){
    Serial.println("EXECUTING: Y");
    executeRotation(&arms[1],1);
  }else if(!strncmp(move, "R'", 2)){
    Serial.println("EXECUTING: R'"); 
    executeTurn(&arms[3], -1);
  }else if(!strncmp(move, "F'", 2)){
    Serial.println("EXECUTING: F'");
    executeTurn(&arms[0], -1);
  }else if(!strncmp(move, "L'", 2)){
    Serial.println("EXECUTING: L'");
    executeTurn(&arms[1], -1);
  }else if(!strncmp(move, "B'", 2)){
    Serial.println("EXECUTING: B'");
    executeTurn(&arms[2], -1);
  }else if(!strncmp(move, "X'", 2)){
    Serial.println("EXECUTING: X'");
    executeRotation(&arms[0],-1);
  }else if(!strncmp(move, "Y'", 2)){
    Serial.println("EXECUTING: Y'");
    executeRotation(&arms[1],-1);
  }else if(!strncmp(move, "R2", 2)){
    Serial.println("EXECUTING: R2");
    executeTurn(&arms[3], 2);
  }else if(!strncmp(move, "F2", 2)){
    Serial.println("EXECUTING: F2");
    executeTurn(&arms[0], 2);
  }else if(!strncmp(move, "L2", 2)){
    Serial.println("EXECUTING: L2");
    executeTurn(&arms[1], 2);
  }else if(!strncmp(move, "B2", 2)){
    Serial.println("EXECUTING: B2");
    executeTurn(&arms[2], 2);
  }else if(!strncmp(move, "X2", 2)){
    Serial.println("EXECUTING: X2");
    executeRotation(&arms[0],2);
  }else if(!strncmp(move, "Y2", 2)){
    Serial.println("EXECUTING: Y2");
    executeRotation(&arms[1],2);
  }else if(!strncmp(move, "RC", 2)){
    Serial.println("RELEASING CUBE");
    for(int i=0; i<4; i++){
      moveClaw(&arms[i], CLAW_OPEN);
    }
    Serial.println("SEQUENCE COMPLETE");
  }
}

void printArmStates(){
  for(int i=0; i<4; i++){
    Serial.print("Arm ");
    Serial.print(arms[i].id);
    if(arms[i].clawState == CLAW_OPEN){
      Serial.print(": claw is open, ");
    }else{
      Serial.print(": claw is closed, ");
    }
    if(arms[i].wallState == WALL_MIN){
      Serial.println("wall is min");
    }else if (arms[i].wallState == WALL_MID){
      Serial.println("wall is mid");
    }else if (arms[i].wallState == WALL_MAX){
      Serial.println("wall is max");
    }
  }
}

void defineAdjacencies(){
  arms[0].oppositeArm = & arms[2];
  arms[0].leftArm = & arms[1];
  arms[0].rightArm = & arms[3];

  arms[1].oppositeArm = & arms[3];
  arms[1].leftArm = & arms[2];
  arms[1].rightArm = & arms[0];

  arms[2].oppositeArm = & arms[0];
  arms[2].leftArm = & arms[3];
  arms[2].rightArm = & arms[1];

  arms[3].oppositeArm = & arms[1];
  arms[3].leftArm = & arms[0];
  arms[3].rightArm = & arms[2];
}

void defineCube(){
  cube.front = 'F';
  cube.back = 'B';
  cube.left = 'L';
  cube.right = 'R';
  cube.up = 'U';
  cube.down = 'D'; 
}

void executeRotation(arm *a, int num){
  int closed[] = {0,0,0,0};
  for(int i=0; i<4; i++){
    closed[i] = arms[i].clawState;
  }
  //If the left arm will interfere, meaning both arm and opposite ar vertical and right is unknown
  if(a->leftArm->wallState == WALL_MID){
    //If claw needs to be closed before releasing left, do so
    if((closed[0] + closed[1] + closed[2] + closed[3] - closed[a->leftArm->id-1]) == 1){
      if(a->clawState == CLAW_OPEN){
        moveClaw(a, CLAW_CLOSED);
      }else{
        moveClaw(a->oppositeArm, CLAW_CLOSED);
      }
    }
    moveClaw(a->leftArm, CLAW_OPEN);
    moveWall(a->leftArm, WALL_MIN);
  }
  if(a->leftArm->clawState == CLAW_OPEN){
    moveClaw(a->leftArm, CLAW_CLOSED);
  }
  //If the right arm will interfere
  if(a->rightArm->wallState == WALL_MID){
    if((closed[0] + closed[1] + closed[2] + closed[3] - closed[a->rightArm->id-1]) == 1){
      if(a->clawState == CLAW_OPEN){
        moveClaw(a, CLAW_CLOSED);
      }else{
        moveClaw(a->oppositeArm, CLAW_CLOSED);
      }
    }
    moveClaw(a->rightArm, CLAW_OPEN);
    moveWall(a->rightArm, WALL_MIN);
    moveClaw(a->rightArm, CLAW_CLOSED);
  }
  if(a->rightArm->clawState == CLAW_OPEN){
    moveClaw(a->leftArm, CLAW_CLOSED);
  }
  //now both left and right arms are vertical and closed
  uint16_t positions[2];
  //preparing ROM
  if(num == 1){
    //if either arm does not have the ROM to execute 1 rotation, move that arm back 1 state
    if(a->wallState == WALL_MIN){
      if(a->clawState == CLAW_CLOSED){
        moveClaw(a, CLAW_OPEN);
      }
      moveWall(a, WALL_MID);
      moveClaw(a, CLAW_CLOSED);
    }
    if(a->oppositeArm-> wallState == WALL_MAX){
      if(a->oppositeArm->clawState == CLAW_CLOSED){
        moveClaw(a->oppositeArm, CLAW_OPEN);
      }
      moveWall(a->oppositeArm, WALL_MID);
      moveClaw(a->oppositeArm, CLAW_CLOSED);
    }
    //now that arms have enough ROM, send them to the right respective positions
    if(a->wallState == WALL_MID){
      positions[0] = WALL_MIN;
    }
    else{
      positions[0] = WALL_MID;
    }
    if(a->oppositeArm->wallState == WALL_MID){
      positions[1] = WALL_MAX;
    }
    else{
      positions[1] = WALL_MID;
    }
  }
  else if(num == 2){
    if(a->wallState == WALL_MID){
      //If a is flat and b is not flat, move a to match b
      if(a->oppositeArm->wallState != WALL_MID){
        if(a->clawState == CLAW_CLOSED){
          moveClaw(a, CLAW_OPEN);
        }
        moveWall(a, (a->oppositeArm->wallState == WALL_MIN)? WALL_MAX:WALL_MIN);
        moveClaw(a, CLAW_CLOSED);
      }
      //If both are flat make a to WALL_MIN and b to WALL_MAX
      else{
        if(a->clawState == CLAW_CLOSED){
          moveClaw(a, CLAW_OPEN);
        }
        moveWall(a, WALL_MIN);
        moveClaw(a, CLAW_CLOSED);
        if(a->oppositeArm->clawState == CLAW_CLOSED){
          moveClaw(a->oppositeArm, CLAW_OPEN);
        }
        moveWall(a->oppositeArm, WALL_MAX);
        moveClaw(a->oppositeArm, CLAW_CLOSED);
      }
    }//If A is vertival move claw b to the opposite
    else if((a->wallState == WALL_MAX && a->oppositeArm->wallState != WALL_MIN) || (a->wallState == WALL_MIN && a->oppositeArm->wallState != WALL_MAX)){
      if(a->oppositeArm->clawState == CLAW_CLOSED){
        moveClaw(a->oppositeArm, CLAW_OPEN);
      }
      moveWall(a->oppositeArm, (a->wallState == WALL_MAX)? WALL_MIN: WALL_MAX);
      moveClaw(a->oppositeArm, CLAW_CLOSED);
    }
    //Now that both arms have the right ROM, set position array
    if(a->wallState == WALL_MAX){
      positions[0] = WALL_MIN;
      positions[1] = WALL_MAX;
    }
    if(a->wallState == WALL_MIN){
      positions[0] = WALL_MAX;
      positions[1] = WALL_MIN;
    }
  }  
  else if(num == -1){
    //if either arm does not have the ROM to execute -1 rotation, move that arm back 1 state
     if(a->wallState == WALL_MAX){
        if(a->clawState == CLAW_CLOSED){
         moveClaw(a, CLAW_OPEN);
        }
        moveWall(a, WALL_MID);
        moveClaw(a, CLAW_CLOSED);
     }
     if(a->oppositeArm-> wallState == WALL_MIN){
        if(a->oppositeArm->clawState == CLAW_CLOSED){
         moveClaw(a->oppositeArm, CLAW_OPEN);
        }
        moveWall(a->oppositeArm, WALL_MID);
        moveClaw(a->oppositeArm, CLAW_CLOSED);
     }
    //now that arms have enough ROM, send them to the right respective positions
    if(a->wallState == WALL_MID){
      positions[0] = WALL_MAX;
    }else{
      positions[0] = WALL_MID;
    }
    if(a->oppositeArm->wallState == WALL_MID){
      positions[1] = WALL_MIN;
    }else{
      positions[1] = WALL_MID;
    }
  }
  //final checks
  if(a->clawState == CLAW_OPEN){
    moveClaw(a, CLAW_CLOSED);
  }
  if(a->oppositeArm->clawState == CLAW_OPEN){
    moveClaw(a->oppositeArm, CLAW_CLOSED);
  }
  if(a->leftArm->clawState == CLAW_CLOSED){
    moveClaw(a->leftArm, CLAW_OPEN);
  }
  if(a->rightArm->clawState == CLAW_CLOSED){
    moveClaw(a->rightArm, CLAW_OPEN);
  }
  moveWalls(a, positions);
}

void executeTurn(arm* a, int num){
  int closed = 0;
  for(int i=0; i<4; i++){
    closed += arms[i].clawState;
  }
  //If left arm will interfere
  if(a->leftArm->wallState == WALL_MID){
    //If an arm needs to be closed before removing leftArm
    if(closed - a->leftArm->clawState < 2){
      if(a->clawState == CLAW_OPEN){
        moveClaw(a, CLAW_CLOSED);
      }else if (a->oppositeArm->clawState == CLAW_OPEN){
        moveClaw(a->oppositeArm, CLAW_CLOSED);
      }
    }
    //Move left arm
    moveClaw(a->leftArm, CLAW_OPEN);
    moveWall(a->leftArm, WALL_MIN);
  }
  if(a->leftArm->clawState == CLAW_OPEN){
    moveClaw(a->leftArm, CLAW_CLOSED);
  }
  closed = 0;
  for(int i=0; i<4; i++){
    closed += arms[i].clawState;
  }
  //If right arm will interfere
  if(a->rightArm->wallState == WALL_MID){
    //If an arm needs to be closed before removing rightArm
    if(closed - a->rightArm->clawState < 2){
      if(a->clawState == CLAW_OPEN){
        moveClaw(a, CLAW_CLOSED);
      }else if (a->oppositeArm->clawState == CLAW_OPEN){
        moveClaw(a->oppositeArm, CLAW_CLOSED);
      }
    }
    //Move right arm
    moveClaw(a->rightArm, CLAW_OPEN);
    moveWall(a->rightArm, WALL_MIN);
    moveClaw(a->rightArm, CLAW_CLOSED);
  }
  if(a->leftArm->clawState == CLAW_OPEN){
    moveClaw(a->leftArm, CLAW_CLOSED);
  }
  //now both adjacent claws are vertical and closed
  int position;
  //clockwise single turn
  if(num == 1){
    if(a->wallState == WALL_MIN){
      if(a->clawState == CLAW_CLOSED){
        moveClaw(a, CLAW_OPEN);
      }
      moveWall(a, WALL_MID);
    }
    if(a->clawState == CLAW_OPEN){
      moveClaw(a, CLAW_CLOSED);
    }
    position = (a->wallState == WALL_MID)? WALL_MIN: WALL_MID;
  }
  //double turn 
  else if (num == 2){
    if(a->wallState == WALL_MID){
      if(a->clawState == CLAW_CLOSED){
        moveClaw(a, CLAW_OPEN);
      }
      moveWall(a, WALL_MIN);
    }
    if(a->clawState == CLAW_OPEN){
      moveClaw(a, CLAW_CLOSED);
    }
    position = (a->wallState == WALL_MAX)? WALL_MIN: WALL_MAX;
  }
  //counterclockwise turn
  else if (num == -1){
    if(a->wallState == WALL_MAX){
      if(a->clawState == CLAW_CLOSED){
        moveClaw(a, CLAW_OPEN);
      }
      moveWall(a, WALL_MID);
    }
    if(a->clawState == CLAW_OPEN){
      moveClaw(a, CLAW_CLOSED);
    }
    position = (a->wallState == WALL_MID)? WALL_MAX: WALL_MID;
  }
  //safety but probably unecessary
  if(a->clawState == CLAW_OPEN){
    moveClaw(a, CLAW_CLOSED);
  }
  moveWall(a, position);
}

void translateMove(char *in, char *result){
  if(!strncmp(in, "RC", 2)){
    strcpy(result, in);
  }
  //now for the real moves
  //cube roations dont get translated
  if(strstr("2X'2Y'", in) != NULL){
    strcpy(result, in);
  }
  char move[5];
  move[4] = '\0';
  for(int i=0; i<4; i++){
    move[i] = ' ';
  }
  bool rotationNeeded = true;
  for(int i=0; i<4; i++){
    if(arms[i].face == *in){
      rotationNeeded = false;
      move[0] = arms[i].absFace;
      if(in[1] != '\0'){
        move[1] = in[1];
      }
    }
  }
  if(rotationNeeded){
    move[0] = 'X';
    move[1] = ' ';
    //shift everything an X in the cube struct
    char temp;
    temp = cube.up;
    cube.up = cube.left;
    cube.left = cube.down;
    cube.down = cube.right;
    cube.right = temp;
    arms[1].face = cube.left;
    arms[3].face = cube.right;
    for(int i=0; i<4; i++){
      if(arms[i].face == *in){
        move[2] = arms[i].absFace;
        if(in[1] != '\0'){
          move[3] = in[1];
        }
      }
    } 
  }
  strcpy(result, move);
}

void scanCube(){
  
}