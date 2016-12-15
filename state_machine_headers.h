#include "pitch.h"

#define INTERRUPT_PIN 3
#define INPUT_PIN0 4
#define INPUT_PIN1 5
#define INPUT_PIN2 6
#define INPUT_PIN3 7
#define INPUT_PIN4 8

#define BUMPER_OUT0 10
#define BUMPER_OUT1 11
#define BUMPER_OUT2 9
#define DROPTARGET_OUT 12
#define FLIPPER_OUT 13
#define LIGHT_PIN0 14
#define LIGHT_PIN1 15
#define LIGHT_PIN2 16
#define LIGHT_PIN3 17
#define SOUND_OUT 18

#define MAX_NUM_BALLS 3
#define MAX_NUM_STATES 10
#define MAX_NUM_ACTIONS 32
#define MAX_NUM_INTERRUPTS 32
#define MAX_NUM_CONDITIONS 5



typedef enum game_state {
  RESET,
  GAME_READY,
  BALL_IN_PLAY,
  GAME_OVER
};

typedef enum part_type {
  MISCELLANEOUS,
  START_BUTTON,
  GAME_SWITCH,
  BUMPER0,
  BUMPER1,
  BUMPER2,
  RAMP,
  RIGHT_LANE,
  LEFT_LANE,
  TOP_LANE,
  DROPTARGET,
  SLINGSHOT,
  INLANE,
  OUTLANE
} part;

typedef enum light_component { 
  SHIELD1,
  ARROW0,
  SHIELD0,
  SHIELD2,
  ARROW2,
  ARROW3,
  ARROW1,
  ARROW4,
  MASTER
} light_c;

struct game_struct {
  part p;
  int curr_state = 0;
  volatile bool changed = false;
};

struct state {
  int next;
  int action;
};

struct action {
  int light_seq = 0;
  int score;
};
