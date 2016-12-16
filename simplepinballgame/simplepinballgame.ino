#include <Bounce2.h>
#include "state_machine_headers2.h"
#define BOUNCE_LOCK_OUT
game_state curr_state = RESET;

unsigned int num_balls_remaining = MAX_NUM_BALLS;
unsigned long score = 0;
unsigned int num_interrupts = 0;
unsigned int num_states = 0;
unsigned int num_actions = 0;
unsigned long start_millis = 0;

Bounce interruptPin = Bounce();

game_struct g[MAX_NUM_INTERRUPTS];
state s[MAX_NUM_STATES];
action a[MAX_NUM_ACTIONS];

void setup() {
  Serial.begin(9600); 

  pinMode(INTERRUPT_PIN, INPUT);
  pinMode(INPUT_PIN0, INPUT);
  pinMode(INPUT_PIN1, INPUT);
  pinMode(INPUT_PIN2, INPUT);
  pinMode(INPUT_PIN3, INPUT);
  pinMode(INPUT_PIN4, INPUT);
  pinMode(DROPTARGET_OUT, OUTPUT);
  pinMode(BUMPER_OUT0, OUTPUT);
  pinMode(BUMPER_OUT1, OUTPUT);
  pinMode(BUMPER_OUT2, OUTPUT);
  pinMode(LIGHT_PIN0, OUTPUT);
  pinMode(LIGHT_PIN1, OUTPUT);
  pinMode(LIGHT_PIN2, OUTPUT);
  pinMode(LIGHT_PIN3, OUTPUT);
  pinMode(FLIPPER_OUT, OUTPUT);
  pinMode(SOUND_OUT, OUTPUT);
  digitalWrite(DROPTARGET_OUT, LOW);
  digitalWrite(BUMPER_OUT0, LOW);
  digitalWrite(BUMPER_OUT1, LOW);
  digitalWrite(BUMPER_OUT2, LOW);
  digitalWrite(LIGHT_PIN0, LOW);
  digitalWrite(LIGHT_PIN1, LOW);
  digitalWrite(LIGHT_PIN2, LOW);
  digitalWrite(LIGHT_PIN3, LOW);
  digitalWrite(FLIPPER_OUT, LOW);
  digitalWrite(SOUND_OUT, LOW);

  interruptPin.interval(10);
  interruptPin.attach(INTERRUPT_PIN);

  num_interrupts = 23;
 
  g[0].p = START_BUTTON;
  g[0].curr_state = 0;

  g[1].p = RIGHT_LANE;
  g[1].curr_state = 1;

  g[2].p = RAMP;
  g[2].curr_state = 1;

  g[3].p = BUMPER1;
  g[3].curr_state = 1;
  g[3].outcode = BUMPER_OUT1;

  g[4].p = RIGHT_LANE;
  g[4].curr_state = 1;
  g[4].outcode = -1;

  g[5].p = BUMPER0;
  g[5].curr_state = 1;

  g[6].p = BUMPER2;
  g[6].curr_state = 1;

  g[7].p = RAMP;
  g[7].curr_state = 1;

  g[8].p = OUTLANE;
  g[8].curr_state = 1;

  g[9].p = INLANE;
  g[9].curr_state = 1;

  g[10].p = OUTLANE;
  g[10].curr_state = 1;

  g[11].p = INLANE;
  g[11].curr_state = 1;

  g[12].p = MISCELLANEOUS;
  g[12].curr_state = 1;

  g[22].p = MISCELLANEOUS;
  g[22].curr_state = 1;

  g[13].p = LEFT_LANE;
  g[13].curr_state = 1;

  g[15].p = SLINGSHOT;
  g[15].curr_state = 1;

  g[14].p = TOP_LANE;
  g[14].curr_state = 1;

  g[16].p = RAMP;
  g[16].curr_state = 1;

  g[21].p = RAMP;
  g[21].curr_state = 1;

  g[23].p = GAME_SWITCH;
  g[23].curr_state = 0;

  s[0].next = 0;
  s[0].action = 0;

  s[1].next = 1;
  s[1].action = 1;

  a[0].light_seq = 8;
  a[0].score = 0;

  a[1].light_seq = 1;
  a[1].score = 100;

  curr_state = RESET;

}

void loop() {
  if (interruptPin.update() && interruptPin.rose()) {
    int changed_pin = ( (!digitalRead(INPUT_PIN4) << 0) | (!digitalRead(INPUT_PIN3) << 1) | (!digitalRead(INPUT_PIN2) << 2) | (!digitalRead(INPUT_PIN1) << 3) | (!digitalRead(INPUT_PIN0) << 4));
    g[changed_pin].changed = true;
  }
  //power on
  if (curr_state == RESET) {
    curr_state = GAME_READY;
    num_balls_remaining = MAX_NUM_BALLS;
    light_sequence(8);
    sound_sequence(1);
    //fire drop target
    score = 0;
    Serial.println("Ready for new game");
  }
  // while ball in play
  for (int i = 0; i < num_interrupts; i++) {
    if (g[i].changed == true) {
      // Serial.println(String(i) + " has been changed.");
      g[i].changed = false;
      switch (g[i].p) {
        case BUMPER0:
          digitalWrite(BUMPER_OUT0, HIGH);
          delay(70);
          digitalWrite(BUMPER_OUT0, LOW);
          if (curr_state == BALL_IN_PLAY) {
            executeState(i);
          }
          break;
        case BUMPER1:
          if (curr_state == BALL_IN_PLAY) {
            executeState(i);
          }
          digitalWrite(BUMPER_OUT0, HIGH);
          delay(70);
          digitalWrite(BUMPER_OUT0, LOW);
          if (curr_state == BALL_IN_PLAY) {
            executeState(i);
          }
          break;
        case BUMPER2:
          if (curr_state == BALL_IN_PLAY) {
            executeState(i);
          }
          digitalWrite(BUMPER_OUT0, HIGH);
          delay(70);
          digitalWrite(BUMPER_OUT0, LOW);
          if (curr_state == BALL_IN_PLAY) {
            executeState(i);
          }
          break;

        case RAMP:
        case SLINGSHOT:
        case INLANE:
        case OUTLANE:
          if (curr_state == BALL_IN_PLAY) {
            executeState(g[i].states);
          }
          break;
        //for loops and top lane, activate normal gameplay if game is ready, otherwise add score
        case LEFT_LANE:
        case RIGHT_LANE:
        case TOP_LANE:
          if (curr_state == GAME_READY) {
            Serial.println("ball in play");
            curr_state = BALL_IN_PLAY;
            //play startup sound & startup lights
          } else if (curr_state == BALL_IN_PLAY) {
            executeState(i);
          }
          break;
        case GAME_SWITCH:
          if (curr_state == BALL_IN_PLAY) {
            num_balls_remaining--;
            curr_state = GAME_READY;
          }
          break;
        case MISCELLANEOUS:
          digitalWrite(DROPTARGET_OUT, HIGH);
          delay(200);
          digitalWrite(DROPTARGET_OUT, LOW);
          break;
        default: break;
      }
    }
  }

  //update display
  if (curr_state == BALL_IN_PLAY) {
    Serial.println("Current score is " + String(score) + ". Number of balls remaining: " + String(num_balls_remaining));
  } else  if (curr_state == GAME_READY) {
    light_sequence(8);
    sound_sequence(1);
  }
  //flipper output
  if (curr_state == BALL_IN_PLAY) {
    digitalWrite(FLIPPER_OUT, HIGH);
  } else {
    digitalWrite(FLIPPER_OUT, LOW);
  }

  //num balls remaining check
  if (!num_balls_remaining && curr_state == GAME_READY) {
    curr_state = RESET;
    if (curr_state == RESET) {
      Serial.println("Game over. Your score was " + String(score));
      Serial.println("Press start button to play a new game.");
    }
  }
}

void executeState(int i) {
  state curr_s = s[g[i].curr_state];
  executeAction(curr_s.action);
  g[i].curr_state = curr_s.next;
}

void executeAction(int a_int) {
  action acn = a[a_int];
  score += acn.score;
  if (acn.light_seq > -1) {
    light_sequence(acn.light_seq);
  } else {
    light_sequence(random(1, 8));
  }
  sound_sequence(acn.sound_seq);
}

void light_sequence (int seq) {
  switch (seq) {
    case 0:     //delay(200);
      digitalWrite(LIGHT_PIN0, LOW);
      digitalWrite(LIGHT_PIN1, LOW);
      digitalWrite(LIGHT_PIN2, LOW);
      digitalWrite(LIGHT_PIN3, LOW);
      break;
    case 1:
      // digitalWrite(LIGHT_PIN0,HIGH);
      digitalWrite(LIGHT_PIN1, HIGH);
      digitalWrite(LIGHT_PIN2, LOW);
      digitalWrite(LIGHT_PIN3, LOW);
      digitalWrite(LIGHT_PIN0, LOW);
      break;
    case 2:
      digitalWrite(LIGHT_PIN1, LOW);
      digitalWrite(LIGHT_PIN2, HIGH);
      digitalWrite(LIGHT_PIN3, LOW);
      digitalWrite(LIGHT_PIN0, LOW);
      break;
    case 3:
      digitalWrite(LIGHT_PIN1, HIGH);
      digitalWrite(LIGHT_PIN2, HIGH);
      digitalWrite(LIGHT_PIN3, LOW);
      digitalWrite(LIGHT_PIN0, LOW);
      break;
    case 4:
      digitalWrite(LIGHT_PIN1, LOW);
      digitalWrite(LIGHT_PIN2, LOW);
      digitalWrite(LIGHT_PIN3, HIGH);
      digitalWrite(LIGHT_PIN0, LOW);
      break;
    case 5:
      digitalWrite(LIGHT_PIN1, HIGH);
      digitalWrite(LIGHT_PIN2, LOW);
      digitalWrite(LIGHT_PIN3, HIGH);
      digitalWrite(LIGHT_PIN0, LOW);
      break;
    case 6:
      digitalWrite(LIGHT_PIN1, LOW);
      digitalWrite(LIGHT_PIN2, HIGH);
      digitalWrite(LIGHT_PIN3, HIGH);
      digitalWrite(LIGHT_PIN0, LOW);
      break;
    case 7:
      digitalWrite(LIGHT_PIN1, HIGH);
      digitalWrite(LIGHT_PIN2, HIGH);
      digitalWrite(LIGHT_PIN3, HIGH);
      digitalWrite(LIGHT_PIN0, LOW);
      break;
    case 8:
      digitalWrite(LIGHT_PIN1, LOW);
      digitalWrite(LIGHT_PIN2, LOW);
      digitalWrite(LIGHT_PIN3, LOW);
      digitalWrite(LIGHT_PIN0, HIGH);
      break;
    default: break;
  }
}

void sound_sequence (int seq) {
  switch (seq) {
    case 0: //startup tone
      tone(SOUND_OUT, NOTE_C5, 100); delay(200); noTone(SOUND_OUT);
      // tone(SOUND_OUT, NOTE_E5, 50); delay(100); noTone(SOUND_OUT);
      // tone(SOUND_OUT, NOTE_G5, 50); delay(100); noTone(SOUND_OUT);
      break;
    case 1: //ready
      tone(SOUND_OUT, NOTE_F5, 100); delay(200); noTone(SOUND_OUT);
      break;
    case 2: //score1
      tone(SOUND_OUT, NOTE_C5, 100); delay(200); noTone(SOUND_OUT);
      // tone(SOUND_OUT, NOTE_G5, 50); delay(100); noTone(SOUND_OUT);
      break;
    case 3: //shutdown
      tone(SOUND_OUT, NOTE_G5, 100); delay(200); noTone(SOUND_OUT);
      // tone(SOUND_OUT, NOTE_E5, 50); delay(100); noTone(SOUND_OUT);
      // tone(SOUND_OUT, NOTE_C5, 50); delay(100); noTone(SOUND_OUT);
      break;
    default: break;
  }
}

