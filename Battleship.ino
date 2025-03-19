class Player;

enum class buttonsEnum {
  Up = 0,
  Down = 1,
  Left = 2,
  Right = 3,
  Select = 4,
  Rotate = 5
};

struct POINT{
  int x;
  int y;
  bool hit;
}point;

struct NODE{
  POINT tile;
  bool ship = false;
} node;

struct SHIP{
  int length;
  POINT tiles[2];
  bool alive;
} ship;

int convertX(int val){
  return val;
}
int convertY(int val){
  return 2 - val;
}

int status = 0;

class Player {
  private:
    int num;
    int currButton;
    int pin;
  
  public:
    int greenState[8];    
    bool redLit[3][3];
    bool greenLit[3][3];
    SHIP ships[2];
    NODE myBoard[3][3];
    bool enemyBoard[3][3] = {false};

    Player(int playerNum){
      if(playerNum == 0){
        pin = A0;
      }
      if(playerNum == 1){
        pin = A1;
      }
      num = playerNum;
    }

  int getNum(){
    return num;
  }

  int getPin(){
    return pin;
  }

  int returnButton(){
    return currButton;
  }

  void buttonSet(int val){
    currButton = val;
  }

  void resetLit(){
    for(int i = 0; i < 3; i++){
      for(int k = 0; k < 3; k++){
        redLit[convertY(k)][convertX(i)] = false;
      }
    }
  }

  void setupLit(POINT cursor, POINT cursor2, int size){
    for(int i = 0; i < 3; i++){
      for(int k = 0; k < 3; k++){
        if(myBoard[convertY(k)][convertX(i)].ship == false){
          redLit[convertY(k)][convertX(i)] = false;
          greenLit[convertY(k)][convertX(i)] = false;
        }
      }
    }
    redLit[convertY(cursor.y)][convertX(cursor.x)] = true;
    if(size == 2){
      redLit[convertY(cursor2.y)][convertX(cursor2.x)] = true;
    }
  }

void selectionLit(POINT cursor){
  for(int i = 0; i < 3; i++){
    for(int k = 0; k < 3; k++){
      if(enemyBoard[convertY(k)][convertX(i)] == true) {
        redLit[convertY(k)][convertX(i)] = true;
      } else {
        redLit[convertY(k)][convertX(i)] = false;
      }
    }
  }
  redLit[convertY(cursor.y)][convertX(cursor.x)] = true;
  delay(300);
}

void updateRedLit(){
  for(int i = 0; i < 3; i++){
    for(int k = 0; k < 3; k++){
      int pin = (2 - k) * 3 + i + 2;
      if(redLit[convertY(k)][convertX(i)]){
        digitalWrite(pin, HIGH);
      } else {
        digitalWrite(pin, LOW);
      }
    }
  }
}

};

Player player1(0);
Player player2(1);
bool team1Set = false;
bool team2Set = false;

void setup() {
  Serial.begin(9600);
  for(int i = 2; i <= 13; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  digitalWrite(A2, LOW);

  digitalWrite(12, LOW);
  shiftOut(13, 11, MSBFIRST, 0b00000000);
  digitalWrite(12, HIGH);
}

int checkButton(int pin){
  int value = analogRead(pin);
  if(value > 900){
    if(value > 1017){
      return 4;
    } else if(value > 1010){
      return 3;
    } else if(value > 1000){
      return 0;
    } else if(value > 970){
      return 2;
    } else {
      return 1;
    }
  } else {
    return -1;
  }
}

void shipSetup(Player &thisPlayer){
  bool setup = false;
  bool setup2 = false;
  POINT cursor = {0, 0};
  POINT cursor1 = {1, 0};

  while(!setup){
    while(!setup2){
      thisPlayer.setupLit(cursor, cursor1, 2);
      thisPlayer.updateRedLit();
      if(checkButton(thisPlayer.getNum()) == 4){
        thisPlayer.myBoard[convertY(cursor.y)][convertX(cursor.x)].ship = true;
        thisPlayer.myBoard[convertY(cursor1.y)][convertX(cursor1.x)].ship = true;
        POINT tempCursor = cursor;
        POINT tempCursor1 = cursor1;
        SHIP thisShip = {2, {tempCursor, tempCursor1}, true};
        thisPlayer.ships[0] = thisShip;
        setup2 = true;
        cursor.x = 0;
        cursor.y = 0;
      }
      shift2(cursor, checkButton(thisPlayer.getNum()), cursor1);
      Serial.print("cursor: ");
      Serial.print(cursor.x);
      Serial.print(" ");
      Serial.println(cursor.y);
      delay(300);
    }
    thisPlayer.setupLit(cursor, cursor1, 1);
    thisPlayer.updateRedLit();
    delay(300);
    if(checkButton(thisPlayer.getNum()) == 4){
      thisPlayer.myBoard[convertY(cursor.y)][convertX(cursor.x)].ship = true;
      SHIP tsShip = {1, {cursor}, true};
      thisPlayer.ships[1] = tsShip;
      setup = true;
    }
    shift(cursor, checkButton(thisPlayer.getNum()));
  }
}

int convertToShift(int x, int y){
    if (x == 0 && y == 2) return -1;  
    if (x == 1 && y == 2) return 7;
    if (x == 2 && y == 2) return 6;
    if (x == 0 && y == 1) return 5;
    if (x == 1 && y == 1) return 4;
    if (x == 2 && y == 1) return 3;
    if (x == 0 && y == 0) return 2;
    if (x == 1 && y == 0) return 1;
    if (x == 2 && y == 0) return 0;

    return -1;  
}

void setGreen(Player &thisPlayer){
  for(int i = 0; i < 3; i++){
    for(int k = 0; k < 3; k++){
      if(thisPlayer.myBoard[convertY(k)][convertX(i)].ship){
        thisPlayer.greenLit[convertX(i)][convertY(k)] = true;
      } else {
        thisPlayer.greenLit[convertX(i)][convertY(k)] = false;
      }
    }
  }
}

void updateGreenLit(Player &thisPlayer){
  digitalWrite(A2, LOW);
  for(int i = 0; i < 8; i++){
    thisPlayer.greenState[i] = 0;
  }
  for(int i = 0; i < 3; i++){
    for(int k = 0; k < 3; k++){
      if(thisPlayer.greenLit[convertY(k)][convertX(i)]){
        if(!(k == 2 && i == 0)){
          Serial.print("setting: ");
          Serial.print(convertY(k));
          Serial.print(" ");
          Serial.println(convertY(i));
          thisPlayer.greenState[convertToShift(convertY(k), convertY(i))] = 1;
          delay(300);
        } else {
          digitalWrite(A2, HIGH);
        }
      } 
    }
  }
  Serial.print("green state val: ");
  uint8_t greenVal = 0;
  for(int i = 0; i < 8; i++){
    Serial.print(thisPlayer.greenState[i]);
    Serial.print(" ");
    if(thisPlayer.greenState[i] == 1){
      greenVal += (1 << i);
    }
  }
  Serial.println("");
  Serial.println(greenVal);
  digitalWrite(12, LOW);
  shiftOut(13, 11, MSBFIRST, greenVal);
  digitalWrite(12, HIGH);
  
}

int hit(Player &thisPlayer, Player &opponent, POINT cursor){
  //power some LED or smth
  opponent.myBoard[convertY(cursor.y)][convertX(cursor.x)].ship = false;
  opponent.greenLit[convertY(cursor.y)][convertX(cursor.x)] = false;
  Serial.println("Hit!");
  digitalWrite(A3, HIGH);
  delay(200);
  if(opponent.ships[1].tiles[0].x == cursor.x && opponent.ships[1].tiles[0].y == cursor.y){
    opponent.ships[1].tiles[0].hit = true;
    opponent.ships[1].alive = false;
    Serial.println("ship destroyed");
    //ship destroyed LCD smth? 

  } else if(opponent.ships[0].tiles[0].x == cursor.x && opponent.ships[0].tiles[0].y == cursor.y){
    opponent.ships[0].tiles[0].hit = true;
    if(opponent.ships[0].tiles[1].hit == true){
      opponent.ships[0].alive = false;
      Serial.println("ship destroyed");
      //ship destoryed LCD smth
    }
  } else {
    opponent.ships[0].tiles[1].hit = true;
    if(opponent.ships[0].tiles[0].hit == true){
      opponent.ships[0].alive = false;
      Serial.println("ship destroyed");
      //ship destroyed LCD smth
    }
  }
  bool isAlive = false;
  for(int i = 0; i < 2; i++){
    if(opponent.ships[i].alive == true){
      Serial.println("ship alive");
      isAlive = true;
    }
  }
  if(isAlive == false){
    Serial.println("game over");
    return -1;
  }
  return 0;
}

void selectTarget(Player &thisPlayer, Player &opponent){
  bool selection = false;
  bool greenLitB = false;
  POINT cursor = {0, 0};
  while(!selection){
    thisPlayer.selectionLit(cursor);
    thisPlayer.updateRedLit();
    if(greenLitB == false){
      setGreen(thisPlayer);
      updateGreenLit(thisPlayer);
      greenLitB = true;
    }
    if(checkButton(thisPlayer.getNum()) == 4){
      thisPlayer.enemyBoard[convertY(cursor.y)][convertX(cursor.x)] = true;
      if(opponent.myBoard[convertY(cursor.y)][convertX(cursor.x)].ship == true){
        Serial.println("true");
        status = hit(thisPlayer, opponent, cursor);
      }
      selection = true;
    }
    shift(cursor, checkButton(thisPlayer.getNum()));
  }
  delay(300);
  digitalWrite(A3, LOW);
}


void shift(POINT &cursor, int dir){
  if(dir == 3 && cursor.x < 2){
    cursor.x++;
  }
  if(dir == 2 && cursor.x > 0){
    cursor.x--;
  }
  if(dir == 0 && cursor.y < 2){
    cursor.y++;
  }
  if(dir == 1 && cursor.y > 0){
    cursor.y--;
  }
}

void shift2(POINT &cursor, int dir, POINT &cursor2){
  if(dir == 3 && cursor.x < 1 && cursor2.x < 2){
    cursor.x++;
    cursor2.x++;
  }
  if(dir == 2 && cursor.x > 0 && cursor2.x > 1){
    cursor.x--;
    cursor2.x--;
  }
  if(dir == 0 && cursor.y <= 1 && cursor2.y < 2){
    cursor.y++;
    cursor2.y++;
  }
  if(dir == 1 && cursor.y > 0 && cursor2.y >= 1){
    cursor.y--;
    cursor2.y--;
  }
}

void loop() {
  digitalWrite(12, LOW);
  shiftOut(13, A2, MSBFIRST, 0);
  digitalWrite(12, HIGH);

  player1.buttonSet(checkButton(player1.getNum()));
  player2.buttonSet(checkButton(player2.getNum()));

  digitalWrite(A5, HIGH);
  if(!team1Set){
    digitalWrite(A4, HIGH);
    shipSetup(player1);
    team1Set = true;
    Serial.print("ship 1: ");
    Serial.print(player1.ships[0].tiles[0].x);
    Serial.print(" ");
    Serial.print(player1.ships[0].tiles[0].y);
    Serial.print(" ");
    Serial.print(player1.ships[0].tiles[1].x);
    Serial.print(" ");
    Serial.print(player1.ships[0].tiles[1].y);
    Serial.println(" ");
    Serial.print(player1.ships[1].tiles[0].x);
    Serial.print(" ");
    Serial.println(player1.ships[1].tiles[0].y);
  }
  digitalWrite(A4, LOW);
  delay(3000);
  if(!team2Set){
    shipSetup(player2);
    team2Set = true;
    Serial.print("ship 1: ");
    Serial.print(player2.ships[0].tiles[0].x);
    Serial.print(" ");
    Serial.print(player2.ships[0].tiles[0].y);
    Serial.print(" ");
    Serial.print(player2.ships[0].tiles[1].x);
    Serial.print(" ");
    Serial.print(player2.ships[0].tiles[1].y);
    Serial.println(" ");
    Serial.print("Ship 2: ");
    Serial.print(player2.ships[1].tiles[0].x);
    Serial.print(" ");
    Serial.println(player2.ships[1].tiles[0].y);
  }
  digitalWrite(A5, LOW);
  player1.resetLit();
  player2.resetLit();
  player1.updateRedLit();
  player2.updateRedLit();
  setGreen(player1);
  setGreen(player2);

  while(status != -1){
    digitalWrite(A4, HIGH);
    selectTarget(player1, player2);
    delay(3000);
    digitalWrite(A4, LOW);
    selectTarget(player2, player1);
  }
  
  


}
