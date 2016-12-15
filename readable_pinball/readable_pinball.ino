#include <Bounce2.h>
#include "state_machine_headers2.h"
game_state curr_state = RESET;
#define MAX_INPUT_SIZE 40

unsigned int num_balls_remaining = MAX_NUM_BALLS;
unsigned long score = 0;
unsigned int num_interrupts = 0;
unsigned int num_states = 0;
unsigned int num_actions = 0;
int num_lines = 0;
unsigned long start_millis = 0;
bool doneReading = false;

volatile int num_tilts = 0;
volatile bool tilt_penalty = false;

Bounce asdf = Bounce();

game_struct g[MAX_NUM_INTERRUPTS];
state s[MAX_NUM_STATES];
action a[MAX_NUM_ACTIONS];



void setup() {
  Serial.begin(9600); //this is to read the custom rules document

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

  asdf.interval(1);
  asdf.attach(INTERRUPT_PIN);

  num_interrupts = 23;

  while (!doneReading) {
    readFile();
  }

  /*
  g[0].p = START_BUTTON;
  g[0].outcode = -1;
  g[0].curr_state = 0;

  g[1].p = RIGHT_LANE;
  g[1].outcode = -1;
  g[1].curr_state = 1;

  g[2].p = RAMP;
  g[2].curr_state = 1;
  g[2].outcode = -1;

  g[3].p = BUMPER1;
  g[3].curr_state = 1;
  g[3].outcode = BUMPER_OUT1;

  g[4].p = RIGHT_LANE;
  g[4].curr_state = 1;
  g[4].outcode = -1;

  g[5].p = BUMPER0;
  g[5].curr_state = 1;
  g[5].outcode = BUMPER_OUT0;

  g[6].p = BUMPER2;
  g[6].curr_state = 1;
  g[6].outcode = BUMPER_OUT2;

  g[7].p = RAMP;
  g[7].curr_state = 1;
  g[7].outcode = -1;

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
  //a[0].sound_seq = 0;
  a[0].score = 0;

  a[1].light_seq = -1;
  // a[1].sound_seq = 2;
  a[1].score = 100; */
  /*
    a[2].light_seq = 2;
    //a[2].sound_seq = 2;
    a[2].score = 100;

    a[3].light_seq = 3;
    //a[3].sound_seq = 3;
    a[3].score = 100;
    */


  curr_state = RESET;

}

void loop() {
  if (asdf.update() && asdf.rose()) {
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
      Serial.println(String(i) + " has been changed.");
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
            executeState(g[i].curr_state);
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
    light_sequence(random(1, 7));
    sound_sequence(2);
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
      tone(SOUND_OUT, NOTE_E5, 50); delay(100); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 50); delay(100); noTone(SOUND_OUT);
      break;
    case 1: //ready
      tone(SOUND_OUT, NOTE_F5, 100); delay(200); noTone(SOUND_OUT);
      break;
    case 2: //score1
      tone(SOUND_OUT, NOTE_C5, 100); delay(200); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_G5, 50); delay(100); noTone(SOUND_OUT);
      break;
    case 3: //shutdown
      tone(SOUND_OUT, NOTE_G5, 100); delay(200); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_E5, 50); delay(100); noTone(SOUND_OUT);
      tone(SOUND_OUT, NOTE_C5, 50); delay(100); noTone(SOUND_OUT);
      break;
    default: break;
  }
}
void readFile() {
  bool error = false;
  if (Serial.available())
  {
    char input[MAX_INPUT_SIZE + 1];
    memset(input, 0, MAX_INPUT_SIZE + 1);
    byte b = 0;
    while (true) {
      b = Serial.readBytes(input, MAX_INPUT_SIZE);
      if (b == 0) {
        Serial.flush();
      } else {
        break;
      }
    }
    int num_tokens = 0;

    if (strncmp(input, "//", 2) == 0) { //comment
      Serial.println("S");
      return;
    } else if (strncmp(input, "EOF", 3) == 0) { //end of file
      Serial.println("S - finish");
      doneReading = true;
      return;
    }

    char tokens[5][100];
    memset(tokens, 0, 5 * 100);
    char* token;

    token = strtok(input, ",");
    while (token != NULL) {
      if (num_tokens > 5) {
        MISCELLANEOUSRedo(input);
        return;
      }
      strcpy((char*)tokens[num_tokens], token);
      token = strtok(NULL, ",");
      num_tokens++;
    }
    Serial.flush();

    int pos = 0;
    token = strtok(tokens[0], ":");
    if (token != NULL) {
      if (strcmp(token, "pos") == 0) {
        token = strtok(NULL, ",");
        pos = atoi(token);
      }
    }

    int type = 0;
    //parse first
    token = strtok(tokens[1], ":");

    if (token != NULL) {
      if (strcmp(token, "part") == 0) {
        type = 0;
        token = strtok(NULL, ",");
        part p = getPart(token);
        if (p == MISCELLANEOUS) {
          MISCELLANEOUSRedo(input); return;
        } else {
          g[pos].p = p;
        }
      } else if (strcmp(token, "state") == 0) {
        type = 1;
      } else if (strcmp(token, "action") == 0) {
        type = 2;
      } else {
        type = -1;
        MISCELLANEOUSRedo(input); return;
      }
    } else {
      MISCELLANEOUSRedo(input); return;
    }

    //parse second line
    token = strtok(tokens[2], ":");
    if (token != NULL) {
      char* two_ptr = strtok(NULL, ";");
      if (!two_ptr) {
        MISCELLANEOUSRedo(input); return;
      }
      int two = atoi(two_ptr);
      switch (type) {
        case 0 :
          if (strcmp(token, "states") == 0) {
            g[pos].curr_state = two;
          } else {
            MISCELLANEOUSRedo(input); return;
          }
          break;
        case 1 :
          if (strcmp(token, "next") == 0) {
            s[pos].action = two;
          } else {
            MISCELLANEOUSRedo(input); return;
          }
          break;
        case 2 :
          if (strcmp(token, "lights") == 0) {
            a[pos].light_seq = two;
          } else {
            Serial.println("8");
            MISCELLANEOUSRedo(input); return;
          }
          break;
        default:
          Serial.println("9");
          MISCELLANEOUSRedo(input); return;
          break;
      }
    }

    //parse third line
    if (type != 0 && type != 1) {
      token = strtok(tokens[3], ":");
      if (token != NULL) {
        char* three_ptr = strtok(token, ";");
        if (!three_ptr) {
          Serial.println("10");
          MISCELLANEOUSRedo(input); return;
        }
        int three = atoi(three_ptr);
        switch (type) {
          case 1 :
            if (strcmp(token, "action") == 0) {
              s[pos].action = three;
            } else {
              Serial.println("12");
              MISCELLANEOUSRedo(input); return;
            }
            break;
          case 2 :
            if (strncmp(token, "sc", 2) == 0) {
              a[pos].score = three;
            } else {
              Serial.println("13");
              MISCELLANEOUSRedo(input); return;
            }
            break;
          default:
            Serial.println("14");
            MISCELLANEOUSRedo(input); return;
            break;
        }
      }
    }
    //increment line read, otherwise ignore so we can re-read that particular line later
    num_lines++;
    switch (type) {
      case 0 :
        num_interrupts++; break;
      case 1 :
        num_states++; break;
      case 2 :
        num_actions++; break;
      default :
        break;
    }
    Serial.println("S");
    Serial.flush();
  }
}

void eraseGame() {
  memset(g, 0, num_interrupts * sizeof(g[0]));
  memset(s, 0, num_states * sizeof(s[0]));
  memset(a, 0, num_actions * sizeof(a[0]));

  num_interrupts = 0;
  num_states = 0;
  num_actions = 0;
  num_lines = 0;
}

void MISCELLANEOUSRedo(String latestRead) {
  Serial.println("Misformatting detected in line " + String(++num_lines));//"); + ": \"" + latestRead + "\"");
  Serial.flush();
}

part getPart(char* part) {
  if (strcmp(part, "start_button") == 0) {
    return START_BUTTON;
  } else if (strcmp(part, "game_switch") == 0) {
    return GAME_SWITCH;
  } else if (strcmp(part, "bumper0") == 0) {
    return BUMPER0;
  } else if (strcmp(part, "bumper1") == 0) {
    return BUMPER1;
  } else if (strcmp(part, "bumper2") == 0) {
    return BUMPER2;
  } else if (strcmp(part, "ramp") == 0) {
    return RAMP;
  } else if (strcmp(part, "right_lane") == 0) {
    return RIGHT_LANE;
  } else if (strcmp(part, "left_lane") == 0) {
    return LEFT_LANE;
  } else if (strcmp(part, "top_lane") == 0) {
    return TOP_LANE;
  } else if (strcmp(part, "droptarget") == 0) {
    return DROPTARGET;
  } else if (strcmp(part, "slingshot") == 0) {
    return SLINGSHOT;
  } else if (strcmp(part, "inlane") == 0) {
    return INLANE;
  } else if (strcmp(part, "outlane") == 0) {
    return OUTLANE;
  } else {
    return MISCELLANEOUS;
  }
}

String toString(game_struct n) {
  return String(n.p) + " " + String(n.curr_state);
}

String toString(state n) {
  return String(n.next) + " " + String(n.action);
}

String toString(action n) {
  return String(n.light_seq) + " " + " " + String(n.score);
}

