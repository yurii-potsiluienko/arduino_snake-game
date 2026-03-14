#include "LedControl.h"
#include "pitches.h"
//=========== SETUP ============

// digital pins for the LED matrix display
#define DIN_PIN 12
#define CS_PIN 11
#define CLK_PIN 10
LedControl lc = LedControl(DIN_PIN,CLK_PIN,CS_PIN,1);

// analog pins for the joy stick
#define JOYSTICK_X_PIN 0
#define JOYSTICK_Y_PIN 1

// digital pin for the active buzzer
#define BUZZER_PIN 8

// analog pin for the potentiometer
#define POTENTIOMETER_PIN A3

// variables
int snake_size = 3;
int snake_x[64];
int snake_y[64];
char direction = 'U';
bool game_over = false;
int food_x, food_y;
int notes[] = {NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
int speed = 5; // 1 to 10
int delay_time = speed * 50; // (ms)--to control the speed of the game
//=========== HELPER FUNCTIONS ============
// update speed based on potentiometer value
void update_speed() {
  int pot_val = analogRead(POTENTIOMETER_PIN); // 0 - 1023
  speed = map(pot_val, 0, 1023, 1, 10);
  // Convert speed to delay time (ms)
  delay_time = speed * 50; // smaller delay = faster snake
}
// update direction based on joystick input
void update_direction() {
  int x_val = analogRead(JOYSTICK_X_PIN);
  int y_val = analogRead(JOYSTICK_Y_PIN);

  if (y_val < 400 && direction != 'D') direction = 'U'; // Up
  else if (x_val > 600 && direction != 'L') direction = 'R'; // Right
  else if (y_val > 600 && direction != 'U') direction = 'D'; // Down
  else if (x_val < 400 && direction != 'R') direction = 'L'; // Left
}

// update snake position
void update_snake_position() {
  // all parts of the snake EXCEPT the head move to where the previous part was before
  // (e.g. 5->4, 4->3, 3->2, 2->1)
  for (int i = snake_size - 1; i > 0; i--){
    snake_x[i] = snake_x[i - 1];
    snake_y[i] = snake_y[i - 1];
  }
  
  // the head of the snake will move depending on current direction
  switch(direction){
    case 'U': snake_y[0] = (snake_y[0] - 1 + 8) % 8; break;
    case 'D': snake_y[0] = (snake_y[0] + 1) % 8; break;
    case 'R': snake_x[0] = (snake_x[0] + 1) % 8; break;
    case 'L': snake_x[0] = (snake_x[0] - 1 + 8) % 8; break;
  }
}

// check for collision of snake with itself
bool check_collision() {
  for (int i = 1; i < snake_size; i++) {
    if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
      return true; // Hit itself
    }
  }
  return false;
}

// eat food
void eat_food() {
  snake_size++;
  tone(BUZZER_PIN, notes[0], 100);
  spawn_food();
}

// spawn new food
void spawn_food() {
  bool valid = false;

  while (!valid) {
    food_x = random(0, 8);
    food_y = random(0, 8);
    valid = true;

    for (int i = 0; i < snake_size; i++) {
      if (snake_x[i] == food_x && snake_y[i] == food_y) {
        valid = false;
        break;
      }
    }
  }
}

// draw the game on the LED matrix
void draw_game() {
  lc.clearDisplay(0);
  for (int i = 0; i < snake_size; i++) {
    lc.setLed(0, snake_y[i], snake_x[i], true);
  }
  lc.setLed(0, food_y, food_x, true); 
}

// display "game over" message
void display_game_over() {
  lc.clearDisplay(0);
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, 0xFF); // Fill rows
    tone(BUZZER_PIN, notes[0], 100);
    delay(100);
  }
  lc.clearDisplay(0);
}

// reset game
void reset_game() {
  snake_size = 3;
  snake_x[0] = 3; snake_y[0] = 3;
  snake_x[1] = 3; snake_y[1] = 4;
  snake_x[2] = 3; snake_y[2] = 5;
  direction = 'U';
  game_over = false;
}
//============================
void setup() {
  // set up display
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
  // Initialize snake position (3,3), (4,3), (5,3)
  snake_x[0] = 3; 
  snake_y[0] = 3;
  snake_x[1] = 3; 
  snake_y[1] = 4;
  snake_x[2] = 3; 
  snake_y[2] = 5;
  // set up buzzer
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  if (game_over){
    display_game_over();
    reset_game();
    delay(2000);
  }
  update_speed();
  update_direction();
  update_snake_position();
  if (check_collision()){
    game_over = true;
  } else{
    if (snake_x[0] == food_x && snake_y[0] == food_y){
      eat_food();
    }
  }
  draw_game();
  delay(delay_time);
}
