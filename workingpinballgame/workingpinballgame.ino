/* Basic game code by Brian Holman
 * For pinball game if Helen's code doesn't work
 * 12/13/2016
 */

// #include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
// #include <Adafruit_GFX.h>
// #include <Adafruit_LEDBackpack.h>
#include <Bounce2.h>
#include "pitch.h"
//#define BOUNCE_LOCK_OUT
//#include "state_machine_headers2.h"

/* ******************
 *  OUTPUT PIN DEFINES
 * ****************** */
#define FLIPPER_MASTER 13
#define BUMPERLEFT_OUT 10
#define BUMPERMIDDLE_OUT 11
#define BUMPERRIGHT_OUT 9
#define DROPTARGET_OUT 12
#define LIGHT_MASTER 14
#define LIGHTPIN1 15
#define LIGHTPIN2 16
#define LIGHTPIN3 17
#define SOUND_OUT 18

/* *****************
 *  INPUT PIN DEFINES
 *  **************** */

#define INTERRUPT_PIN 3
//#define TILT_SENSOR 2
#define INPUT_PIN0 4
#define INPUT_PIN1 5
#define INPUT_PIN2 6
#define INPUT_PIN3 7
#define INPUT_PIN4 8

/******************
 * INPUT ENCODER DEFINES
 ****************** */
#define START 0
#define END 23
#define TOPRIGHTLANE 1
#define TOPGATESWITCH 2
#define MIDDLEBUMPER 3
#define TOPRIGHTROLLOVER 4
#define LEFTBUMPER 5
#define RIGHTBUMPER 6
#define RAMPGATESWITCH 7
#define RIGHTOUTLANE 8
#define RIGHTINLANE 9
#define LEFTOUTLANE 10
#define LEFTINLANE 11
#define STANDUP 12
#define DROPTARGET 22
#define TOPLEFTROLLOVER 13
#define TOPROLLOVER 14
#define SLINGSHOT 15
#define TOPMIDDLELANE 21
#define TOPLEFTLANE 16
#define NOTHING 100

/* ****************
 *  SCORES
 ****************** */
#define RIGHTLOOPSCORE 150
#define LEFTLOOPSCORE 150
#define RAMPSCORE 250
#define STANDUPSCORE 100
#define BUMPERSCORE 50
#define SLINGSHOTSCORE 50
#define TOPLANESCORE 50
#define INLANESCORE 50
#define OUTLANESCORE 100

/***********
 * LIGHT DEFINES
 ************/
#define SHIELDMIDDLE 1 //checked
#define ARROWSHOOTER 5 // checked
#define SHIELDRIGHT 7 // checked
#define SHIELDLEFT 3 // checked
#define ARROWRAMP 2 // checked
#define ARROWSTANDUP 6 // checked
#define ARROWRIGHTLOOP 4 //checked
#define ARROWLEFTLOOP 8
#define ALLLIGHTS 0 // checked

/* *********************
 *  GAME LOGIC DEFINES
 *********************** */
 
#define INITIAL_NUM_BALLS 3
#define MAX_NUM_INPUTS 24
#define RAMP_SHOT_NUM 3

/* ********************
 *  STATE MACHINE DEFINES AND GAME LOGIC VARIABLES
 *  ******************* */

#define RESET 0
#define GAME_READY 1
#define BALL_IN_PLAY 2
#define BALL_DRAINED 3
#define BALL_LAUNCH_CASE 4

unsigned int numRampShots = 0;
unsigned int curr_state = RESET;
unsigned int num_balls_remaining = INITIAL_NUM_BALLS;
unsigned long score = 0;
unsigned int multiplier = 0;
unsigned int lastInput = NOTHING;
unsigned int bonusTarget = NOTHING;
unsigned int numRampNeeded = RAMP_SHOT_NUM;

/* ************************
 *  Miscellaeous setup stuff
 * ************************ */

// Variables used in reset state light display
int lightcounter = 0;
int lightnum = 0;

// Helen legacy strucutre
struct game_struct {
  volatile bool changed = false;
};

Bounce bounce = Bounce();
game_struct g[MAX_NUM_INPUTS];


/*******************
 * GAME FUNCTIONS
 ******************* */

void lightStartSequence() {
  for (int j = 0; j < 3; j++) {
    for (int k = 0; k < 9; k++) {
      light(k);
      delay(50);
    }
  }
  light(ARROWSHOOTER);
}

// Flashy lights, etc, when new game starts
void newGameActions() {
   Serial.println("Ready for new game. Launch the ball!");
   sound_sequence(0);
   lightStartSequence();
   numRampShots = 0;
   num_balls_remaining = INITIAL_NUM_BALLS - 1;
   score = 0;
   multiplier = 1;
   lastInput = NOTHING;
   bonusTarget = NOTHING;
   numRampNeeded = RAMP_SHOT_NUM;
}

void printScore() {
  Serial.println("Score = " + String(score));
}

// Gives more points the further the user has shot the ball
void scoreSkillShot(int k) {
    if (k == TOPRIGHTROLLOVER) {
      Serial.println("Launch harder next time?");
      score += 50; }
    else if (k == TOPRIGHTLANE) { 
      Serial.println("Nice launch!");
      light(SHIELDRIGHT);
      score += 100; }
    else if (k == TOPMIDDLELANE) { 
      Serial.println("Great launch!!");
      light(SHIELDMIDDLE);
      score += 200; }
    else if (k == TOPLEFTLANE) { 
      Serial.println("Awesome launch!!!");
      light(SHIELDLEFT);
      score += 300; }
    else {
      Serial.println("Something weird occurred...");
    }
    printScore();
}

void sound_sequence (int seq) {
  switch (seq) {
    case 0: //startup tone
      tone(SOUND_OUT, NOTE_C5, 200); delay(260); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_E5, 200); delay(260); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 200); delay(260); noTone(SOUND_OUT);
      break;
    case 1: //ready //very happy
      tone(SOUND_OUT, NOTE_C5, 200); delay(300); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_C5, 200); delay(300); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 200); delay(300); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 200); delay(300); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_A5, 200); delay(300); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_A5, 200); delay(300); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 200); delay(300); noTone(SOUND_OUT);
      break;
    case 2: //score //kind of happy
      tone(SOUND_OUT, NOTE_C5, 200); delay(260); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 200); delay(260); noTone(SOUND_OUT);
      break;
    case 4: // ball drain, no end game //kind of sad
      tone(SOUND_OUT, NOTE_AS5, 200); delay(300); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 250); delay(400); noTone(SOUND_OUT);
      break;
    case 3: //end game //very sad
      tone(SOUND_OUT, NOTE_AS5, 200); delay(300); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_A5, 250); delay(400); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_GS5, 300); delay(500); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 600); delay(900); noTone(SOUND_OUT);
      break;
    case 5: // standup score. just a beep
      tone(SOUND_OUT, NOTE_G5, 200); delay(100); noTone(SOUND_OUT);
    default: break;
  }
}


void rotateBonus() {
  int nextTargetSelect = random(7);
  int nextBonusTarget;
  switch (nextTargetSelect) {
    case 0: nextBonusTarget = RAMPGATESWITCH; light(ARROWRAMP); break;
    case 1: nextBonusTarget = TOPGATESWITCH; light(ARROWLEFTLOOP); break;
    case 2: nextBonusTarget = TOPROLLOVER; light(ARROWRIGHTLOOP); break;
    case 3: nextBonusTarget = STANDUP; light(ARROWSTANDUP); break;
    case 4: nextBonusTarget = TOPLEFTLANE; light(SHIELDLEFT); break;
    case 5: nextBonusTarget = TOPMIDDLELANE; light(SHIELDMIDDLE); break;
    case 6: nextBonusTarget = TOPRIGHTLANE; light(SHIELDRIGHT); break;
    default: break;
  }
  //Quick code to make sure the bonus target always changes upon hit, favoring ramp and standup
  if (nextBonusTarget == bonusTarget) {
    if (nextBonusTarget == RAMPGATESWITCH) {
      nextBonusTarget = STANDUP; light(ARROWSTANDUP);
    } else {
      nextBonusTarget = RAMPGATESWITCH; light(ARROWRAMP);
    }
  }
  bonusTarget = nextBonusTarget;
}

void gameOverActions() {
  sound_sequence(3);
  light(ALLLIGHTS);
  Serial.println("Final score = " + String(score));
  Serial.println("Press start button to play again!");
  // Flipper master swtich off TODO 
}

void light(int designator) {
  switch (designator) {
    case 0:
      digitalWrite(LIGHT_MASTER, HIGH);
      digitalWrite(LIGHTPIN1, HIGH);
      digitalWrite(LIGHTPIN2, HIGH);
      digitalWrite(LIGHTPIN3, HIGH);
      break;
    case 1:
      digitalWrite(LIGHTPIN1, LOW);
      digitalWrite(LIGHTPIN2, LOW);
      digitalWrite(LIGHTPIN3, LOW);
      digitalWrite(LIGHT_MASTER, LOW);
      break;
    case 2:
      digitalWrite(LIGHTPIN1, LOW);
      digitalWrite(LIGHTPIN2, LOW);
      digitalWrite(LIGHTPIN3, HIGH);
      digitalWrite(LIGHT_MASTER, LOW);
      break;
    case 3:
      digitalWrite(LIGHTPIN1, LOW);
      digitalWrite(LIGHTPIN2, HIGH);
      digitalWrite(LIGHTPIN3, LOW);
      digitalWrite(LIGHT_MASTER, LOW);
      break;
    case 4:
      digitalWrite(LIGHTPIN1, LOW);
      digitalWrite(LIGHTPIN2, HIGH);
      digitalWrite(LIGHTPIN3, HIGH);
      digitalWrite(LIGHT_MASTER, LOW);
      break;
    case 5:
      digitalWrite(LIGHTPIN1, HIGH);
      digitalWrite(LIGHTPIN2, LOW);
      digitalWrite(LIGHTPIN3, LOW);
      digitalWrite(LIGHT_MASTER, LOW);
      break;
    case 6:
      digitalWrite(LIGHTPIN1, HIGH);
      digitalWrite(LIGHTPIN2, LOW);
      digitalWrite(LIGHTPIN3, HIGH);
      digitalWrite(LIGHT_MASTER, LOW);
      break;
    case 7:
      digitalWrite(LIGHTPIN1, HIGH);
      digitalWrite(LIGHTPIN2, HIGH);
      digitalWrite(LIGHTPIN3, LOW);
      digitalWrite(LIGHT_MASTER, LOW);
      break;
    case 8:
      digitalWrite(LIGHTPIN1, HIGH);
      digitalWrite(LIGHTPIN2, HIGH);
      digitalWrite(LIGHTPIN3, HIGH);
      digitalWrite(LIGHT_MASTER, LOW);
      break;
  }                                
}

// Flashy reset state lights
void lightLoop() {
    if (lightcounter == 0) {
      lightnum = random(9);
    }
    light(lightnum);
    delay(50);
    lightcounter++;
    if(lightcounter == 7) { lightcounter = 0; }
}


/* ********************
 * SETUP
 ********************** */

void setup() {
  Serial.begin(9600); //this is to read the custom rules document (which I ditched - Brian)

  pinMode(INTERRUPT_PIN, INPUT);
  pinMode(INPUT_PIN0, INPUT);
  pinMode(INPUT_PIN1, INPUT);
  pinMode(INPUT_PIN2, INPUT);
  pinMode(INPUT_PIN3, INPUT);
  pinMode(INPUT_PIN4, INPUT);
  pinMode(DROPTARGET_OUT, OUTPUT);
  pinMode(BUMPERLEFT_OUT, OUTPUT);
  pinMode(BUMPERMIDDLE_OUT, OUTPUT);
  pinMode(BUMPERRIGHT_OUT, OUTPUT);
  pinMode(LIGHT_MASTER, OUTPUT);
  pinMode(LIGHTPIN1, OUTPUT);
  pinMode(LIGHTPIN2, OUTPUT);
  pinMode(LIGHTPIN3, OUTPUT);
  pinMode(FLIPPER_MASTER, OUTPUT);
  //pinMode(DISPLAY_OUT0, OUTPUT);
  //pinMode(DISPLAY_OUT1, OUTPUT);
  pinMode(SOUND_OUT, OUTPUT);
  digitalWrite(DROPTARGET_OUT, LOW);
  digitalWrite(BUMPERLEFT_OUT, LOW);
  digitalWrite(BUMPERMIDDLE_OUT, LOW);
  digitalWrite(BUMPERRIGHT_OUT, LOW);
  digitalWrite(LIGHT_MASTER, LOW);
  digitalWrite(LIGHTPIN1, LOW);
  digitalWrite(LIGHTPIN2, LOW);
  digitalWrite(LIGHTPIN3, LOW);
  digitalWrite(FLIPPER_MASTER, HIGH); // TODO fix this functionality
 // digitalWrite(DISPLAY_OUT0, LOW);
 // digitalWrite(DISPLAY_OUT1, LOW);
  digitalWrite(SOUND_OUT, LOW);

  bounce.interval(20);
  bounce.attach(INTERRUPT_PIN);

  Serial.println("Ready for new game...press start");

}

void loop() {
  // Turn binary encoded input into a number
  if (bounce.update() && bounce.rose()) {
    int changed_pin = ( (!digitalRead(INPUT_PIN4) << 0) | (!digitalRead(INPUT_PIN3) << 1) | (!digitalRead(INPUT_PIN2) << 2) | (!digitalRead(INPUT_PIN1) << 3) | (!digitalRead(INPUT_PIN0) << 4));
    g[changed_pin].changed = true;
  }

  if (curr_state == RESET) {
    lightLoop();  
  }

  for (int i = 0; i < MAX_NUM_INPUTS; i++) {
    if (g[i].changed == true) {
      Serial.println(String(i) + " has been changed.");
      g[i].changed = false;

      // CASE SPECIFIC LOGIC:

      // weird specific case play for when ball goes into the top lanes on launch
      if (curr_state == BALL_LAUNCH_CASE) {
        curr_state = BALL_IN_PLAY;
        scoreSkillShot(i);
        light(ALLLIGHTS);
      }

      if ((curr_state == GAME_READY) || (curr_state == BALL_DRAINED)) { // occurs on ball launch
        if (i == TOPROLLOVER) {
          curr_state = BALL_LAUNCH_CASE;
        } else if (i != END) {
          scoreSkillShot(i);
          light(ALLLIGHTS);
          curr_state = BALL_IN_PLAY;
        }
      }

      // Extra ball cancel
      if ((curr_state == BALL_IN_PLAY) && (numRampShots > 0)) {
        if ((i != RAMPGATESWITCH) && (i != LEFTINLANE)) {
          numRampShots = 0;
          Serial.println("Ramp combo reset");
        } else if ((i == LEFTINLANE) && (lastInput != RAMPGATESWITCH)) {
          numRampShots = 0;
          Serial.println("Ramp combo reset");
        }
      }

      // Ball drain
          if ((i == END) || (i == LEFTOUTLANE) || (i == RIGHTOUTLANE)){
            if ((curr_state == BALL_IN_PLAY) && (lastInput != END) && (lastInput != LEFTOUTLANE) && (lastInput != RIGHTOUTLANE)) {
              if ((i == LEFTOUTLANE) || (i == RIGHTOUTLANE)) {
                score += OUTLANESCORE;
                printScore();
              }
              if (num_balls_remaining > 0) {
                sound_sequence(4);
                Serial.println("Ball drained! Launch again. " + String(num_balls_remaining) + " balls remaining");
                light(ARROWSHOOTER);
                num_balls_remaining--;
                curr_state = BALL_DRAINED;
              } else {
                Serial.println("Ball drained! Game over!");
                gameOverActions();
                curr_state = RESET;
              }
              numRampShots = 0;
              multiplier = 1;
              bonusTarget = NOTHING;
            }
          }

      // INPUT SPECIFIC LOGIC:
      switch (i) {
          case START:
            if (curr_state == RESET) {
              newGameActions();
              curr_state = GAME_READY;
            } break;

          case (LEFTBUMPER):
            digitalWrite(BUMPERLEFT_OUT, HIGH);
            delay(100);
            digitalWrite(BUMPERLEFT_OUT, LOW);
            if (curr_state == BALL_IN_PLAY) {
              score += BUMPERSCORE;
              printScore();
              rotateBonus();
            } break;
          case (MIDDLEBUMPER):
            digitalWrite(BUMPERMIDDLE_OUT, HIGH);
            delay(100);
            digitalWrite(BUMPERMIDDLE_OUT, LOW);
            if (curr_state == BALL_IN_PLAY) {
              score += BUMPERSCORE;
              printScore();
              rotateBonus();
            } break;
          case (RIGHTBUMPER):
            digitalWrite(BUMPERRIGHT_OUT, HIGH);
            delay(100);
            digitalWrite(BUMPERRIGHT_OUT, LOW);
            if (curr_state == BALL_IN_PLAY) {
              score += BUMPERSCORE;
              printScore();
              rotateBonus();
            } break;
          case (SLINGSHOT):
            if (curr_state == BALL_IN_PLAY) {
              score += SLINGSHOTSCORE;
              printScore();
            } break;
          case (STANDUP):
            if (curr_state == BALL_IN_PLAY) {
              if (bonusTarget == STANDUP) {
                multiplier++;
                score += (STANDUPSCORE * multiplier);
                Serial.println("Bonus target hit! Target score multiplied by " + String(multiplier));
                rotateBonus();
              } else {
                score += STANDUPSCORE;
              }
              sound_sequence(5);
              printScore();
            } break;
          case (RAMPGATESWITCH):
            if (curr_state == BALL_IN_PLAY) {
              if (bonusTarget == RAMPGATESWITCH) {
                multiplier++;
                score += (RAMPSCORE * multiplier);
                Serial.println("Bonus target hit! Ramp score multiplied by " + String(multiplier));
                rotateBonus();
              } else {
                score += RAMPSCORE;
              }
              printScore();
              //Note: ramp counter reset handled above
              numRampShots++;
              if (numRampShots == numRampNeeded) {
                num_balls_remaining++;
                sound_sequence(1); // very happy sound
                Serial.println("Congrats! Extra ball earned! You now have " + String(num_balls_remaining) + " balls remianing");
                numRampShots = 0;
                numRampNeeded ++;
              } else {
                sound_sequence(2); // happy sound
                Serial.println(String(numRampShots) +" ramp shots in a row. " + String(numRampNeeded) + " needed for extra ball.");
              }
            } break;
          case (TOPGATESWITCH):
            if(curr_state == BALL_IN_PLAY) {
              if (bonusTarget == TOPGATESWITCH) {
                multiplier++;
                score += (LEFTLOOPSCORE * multiplier);
                Serial.println("Bonus target hit! Left loop score multiplied by " + String(multiplier));
                rotateBonus();
              } else {
                score += LEFTLOOPSCORE;
              }
              printScore();
            } break;
           case (TOPROLLOVER):
              if(curr_state == BALL_IN_PLAY) {
                //if (lastInput == TOPRIGHTROLLOVER) { // switch is unreliable, ignore it
                  if (lastInput != TOPLEFTROLLOVER) {
                  if (bonusTarget == TOPROLLOVER) {
                    multiplier++;
                    score += (RIGHTLOOPSCORE * multiplier);
                    Serial.println("Bonus target hit! Right loop score multiplied by " + String(multiplier));
                    rotateBonus();
                  } else {
                    score += RIGHTLOOPSCORE;
                  }
                  printScore();
                }
              } break;
           case (TOPLEFTLANE):
              if(curr_state == BALL_IN_PLAY) {
                if (bonusTarget == TOPLEFTLANE) {
                  multiplier++;
                  score += (TOPLANESCORE * multiplier);
                  Serial.println("Bonus target hit! Top lane score multiplied by " + String(multiplier));
                  rotateBonus(); 
                } else {
                  score += TOPLANESCORE;
                }
                printScore();
              } break;
            case (TOPMIDDLELANE):
              if(curr_state == BALL_IN_PLAY) {
                if (bonusTarget == TOPMIDDLELANE) {
                  multiplier++;
                  score += (TOPLANESCORE * multiplier);
                  Serial.println("Bonus target hit! Top lane score multiplied by " + String(multiplier));
                  rotateBonus(); 
                } else {
                  score += TOPLANESCORE;
                }
                printScore();
              } break;
            case (TOPRIGHTLANE):
              if(curr_state == BALL_IN_PLAY) {
                if (bonusTarget == TOPRIGHTLANE) {
                  multiplier++;
                  score += (TOPLANESCORE * multiplier);
                  Serial.println("Bonus target hit! Top lane score multiplied by " + String(multiplier));
                  rotateBonus(); 
                } else {
                  score += TOPLANESCORE;
                }
                printScore();
              } break;
            case (LEFTINLANE):
              if (curr_state == BALL_IN_PLAY) {
                 score += INLANESCORE;
                  printScore();
              }
              break;
            case (RIGHTINLANE):
              if (curr_state == BALL_IN_PLAY) {
                score += INLANESCORE;
                printScore();
              }
              break;
            
        default: break;
      }
      lastInput = i;
    }
  }
}



/*
void light_sequence (int seq) {
  switch (seq) {
    case 0:
      digitalWrite(LIGHT_PIN0, HIGH);
      digitalWrite(LIGHT_PIN1, LOW);
      digitalWrite(LIGHT_PIN2, LOW);
      digitalWrite(LIGHT_PIN3, LOW);
      //delay(200);
      break;
    case 1:
      digitalWrite(LIGHT_PIN0, LOW);
      digitalWrite(LIGHT_PIN1, HIGH);
      digitalWrite(LIGHT_PIN2, LOW);
      digitalWrite(LIGHT_PIN3, LOW);
      //delay(200);
      break;
    case 2:
      digitalWrite(LIGHT_PIN0, LOW);
      digitalWrite(LIGHT_PIN1, LOW);
      digitalWrite(LIGHT_PIN2, HIGH);
      digitalWrite(LIGHT_PIN3, LOW);
      //delay(200);
      break;
    case 3:
      digitalWrite(LIGHT_PIN0, LOW);
      digitalWrite(LIGHT_PIN1, LOW);
      digitalWrite(LIGHT_PIN2, LOW);
      digitalWrite(LIGHT_PIN3, HIGH);
      //delay(200);
      break;
    default: break;
  }
}

*/
