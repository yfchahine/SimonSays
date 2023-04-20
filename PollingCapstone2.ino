#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 10 and 11 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 11; // Connects to module's RX 
static const uint8_t PIN_MP3_RX = 10; // Connects to module's TX 
SoftwareSerial softwareSerial(PIN_MP3_TX, PIN_MP3_RX);

//initialize buttons and LED ports
const uint8_t blueButton = 2;
const uint8_t redButton = 3;
const uint8_t greenButton = 4;
const uint8_t yellowButton = 5;
const uint8_t blueLed = 9;
const uint8_t redLed = 6;
const uint8_t greenLed = 7;
const uint8_t yellowLed = 8;
const uint8_t modeSwitch = 13;
const uint8_t volumeControl = 12;
int currentModeState;
int previousModeState;

int gameIndex;
#define MAX_LENGTH 1000
uint8_t gameMode = 0;
uint8_t user = 1;

uint8_t gameSequence[MAX_LENGTH];
uint8_t currentRound = 0;

int breakVariable = 1;

int volume = 20;
int volumeFlag = 1;
//Set up the Arduino board

// Create the Player object
DFRobotDFPlayerMini player;

void setup() {
  Serial.begin(9600);
  //set up inputs and outputs
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redButton, INPUT_PULLUP);
  pinMode(yellowButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(modeSwitch, INPUT_PULLUP);
  pinMode(volumeControl, INPUT_PULLUP);

  gameIndex = 0;
  gameSequence[MAX_LENGTH] = {0};
 

  // The following line primes the random number generator.
  // It assumes pin A3 is floating (disconnected):
  randomSeed(analogRead(A3));

  if (digitalRead(modeSwitch) == LOW) {
      Serial.print("Single Player\n");
      currentModeState = 0;
      gameMode = 0;
  }
  if (digitalRead(modeSwitch) == HIGH) {
      Serial.print("Multiplayer\n");
      currentModeState = 1;
      gameMode = 1;
  }
  previousModeState = currentModeState;

  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
   Serial.println("OK");

    // Set volume to maximum (0 to 30).
    player.volume(20);
    // Play the first MP3 file on the SD card
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

//Light LED sequence
void lightLed(int ledIndex) {
  currentModeState = digitalRead(modeSwitch);
  if (currentModeState != previousModeState) {
    GameMode_Switch();
  }
  if (digitalRead(volumeControl) == HIGH) {
    Volume();
  }
  if (gameMode == 0) {
    switch (ledIndex){
    case 2:
      digitalWrite(blueLed, HIGH);
      player.play(2);
      delay(1000);
      digitalWrite(blueLed, LOW);
    break;
    case 3:
      digitalWrite(redLed, HIGH);
      player.play(3);
      delay(1000);
      digitalWrite(redLed, LOW);
    break;
    case 4:
      digitalWrite(greenLed, HIGH);
      player.play(4);
      delay(1000);
      digitalWrite(greenLed, LOW);
    break;
    case 5:
      digitalWrite(yellowLed, HIGH);
      player.play(5);
      delay(1000);
      digitalWrite(yellowLed, LOW);
    break;
    default:
    break;
    }
  }
  if (gameMode == 1) {
    switch (ledIndex){
      case 2:
        digitalWrite(blueLed, HIGH);
        player.play(2);
        delay(1000);
        digitalWrite(blueLed, LOW);
      break;
      case 3:
        digitalWrite(redLed, HIGH);
        player.play(3);
        delay(1000);
        digitalWrite(redLed, LOW);
      break;
      case 4:
        digitalWrite(greenLed, HIGH);
        player.play(4);
        delay(1000);
        digitalWrite(greenLed, LOW);
      break;
      case 5:
        digitalWrite(yellowLed, HIGH);
        player.play(5);
        delay(1000);
        digitalWrite(yellowLed, LOW);
      break;
      default:
      break;
    }
  }
  volumeFlag = 1;
}

//Lights the proper LEDs
void playSequence() {
  delay(500);
  for (int i = 0; i < gameIndex; i++) {
    int currentLed = gameSequence[i];
    lightLed(currentLed);
    delay(375);
  }
}

//wait for button read from user
int readButtons() {
  int i = 0;
  while(true) {
    currentModeState = digitalRead(modeSwitch);
    if (currentModeState != previousModeState) {
    GameMode_Switch();
    }
    if (digitalRead(volumeControl) == HIGH && volumeFlag == 1) {
      Volume();
      volumeFlag = 0;
    } else if (digitalRead(volumeControl) == LOW && volumeFlag == 0) {
      volumeFlag = 1;
    }
    if (digitalRead(blueButton) == HIGH) {
      i = 2;
    Serial.println(i);
    return i;
    }
    else if (digitalRead(redButton) == HIGH) {
      i = 3;
    Serial.println(i);
    return i;
    }
    else if (digitalRead(greenButton) == HIGH) {
      i = 4;
    Serial.println(i);
    return i;
    }
    else if (digitalRead(yellowButton) == HIGH) {
      i = 5;
    Serial.println(i);
    return i;
    }
  }
  delay(1);
}

//flash LEDs all on when game is over and report score to console
void gameOver() {
  currentModeState = digitalRead(modeSwitch);
  if (currentModeState != previousModeState) {
    GameMode_Switch();
  }
    if (digitalRead(volumeControl) == HIGH && volumeFlag == 1) {
      Volume();
      volumeFlag = 0;
    } else if (digitalRead(volumeControl) == LOW && volumeFlag == 0) {
      volumeFlag = 1;
    }
  if (gameMode == 1) {
    Serial.print("Game over! Your score: ");
    Serial.println(gameIndex - 1);
  }
  if (gameMode == 0) {
    Serial.print("Game over! Your score: ");
    Serial.println(gameIndex);
  }
  gameIndex = 0;
  delay(200);
  digitalWrite(redLed, HIGH);
  digitalWrite(yellowLed, HIGH);
  digitalWrite(greenLed, HIGH);
  digitalWrite(blueLed, HIGH);
  delay(500);
  digitalWrite(redLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);
  volumeFlag = 1;
}

//compare user input to the current array
bool checkUserSequence() {
  currentModeState = digitalRead(modeSwitch);
  if (currentModeState != previousModeState) {
    GameMode_Switch();
  }
  if (digitalRead(volumeControl) == HIGH && volumeFlag == 1) {
    Volume();
    volumeFlag = 0;
  } else if (digitalRead(volumeControl) == LOW && volumeFlag == 0) {
    volumeFlag = 1;
  }
  for (int i = 0; i < gameIndex; i++) {
    int expectedButton = gameSequence[i];
    byte actualButton = readButtons();
    lightLed(actualButton);
    if (expectedButton != actualButton) {
      return false;
    }
    volumeFlag = 1;
  }

  return true;
}

//Clear the current game tracking
void Clear_Array(){
  gameIndex = 0;
  gameSequence[MAX_LENGTH] = {0};
}

void GameMode_Switch() {
  // check if the state is switched
  if (digitalRead(modeSwitch) == LOW) {
    Serial.print("Single Player\n");
    gameMode = 0;
    currentModeState = 0;
    Clear_Array();
  }
  else {
    Serial.print("Multiplayer\n");
    gameMode = 1;
    currentModeState = 1;
    Clear_Array();
  }
    previousModeState = currentModeState; // update the previous state
    loop();
}

void Volume() {
  if (volumeFlag == 1) {
    if (volume < 30) {
      volume += 10;
      player.volume(volume);
    }
    else {
      volume = 0;
      player.volume(volume);
    }
    volumeFlag = 0;
    player.play(8);
  }
}

//The main game loop
void loop() {
    if (digitalRead(volumeControl) == HIGH && volumeFlag == 1) {
      Volume();
      volumeFlag = 0;
    } else if (digitalRead(volumeControl) == LOW && volumeFlag == 0) {
      volumeFlag = 1;
    }
  
  currentModeState = digitalRead(modeSwitch);
  
  if (currentModeState != previousModeState) {
    GameMode_Switch();
  }
  
  if (gameMode == 1) {
    // Add a random color to the end of the sequence
    gameSequence[gameIndex] = random(2, 6);
    gameIndex++;
    if (gameIndex >= MAX_LENGTH) {
      gameIndex = MAX_LENGTH - 1;
    }

    playSequence();
    if (!checkUserSequence()) {
      gameOver();
      player.play(1);
      delay(2000);
    }

    delay(300);

    if (gameIndex > 0) {
      delay(300);
    }
  }
  
  if (gameMode == 0) {
    byte actualButton = readButtons();
    lightLed(actualButton);
    delay(1000);
    gameSequence[gameIndex] = actualButton;
    gameIndex++;
    
    if (user == 1) {
      user = 2;
    }

    else if (user == 2) {
      user = 1;
    }

    playSequence();

    if (!checkUserSequence()) {
      gameOver();
      if (user == 1) {
        player.play(7);
      }
      if (user == 2) {
        player.play(6);
      }
      user = 1;
    }
  }
  volumeFlag = 1;
  loop();
}
