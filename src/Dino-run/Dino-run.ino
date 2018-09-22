#include <Gamebuino.h>

#include <SPI.h>

#include "Bitmaps.h"

Gamebuino gb;

struct player {  
  bool on_ground;
  byte x;
  byte width;
  byte height;
  byte offset;
  int score;
  float y;
  byte velocity_x;
  float velocity_y;
};

struct obstacle
{
  long x;
  byte y;
  byte width;
  byte height;
  float velocity_x;
  int sprite;
};


byte gameState = 0;
//animation
byte current = 0;
byte limit = 0;
byte fall_delay = 0;

int max_score = 0;
float gravity;

struct player dino;
struct obstacle enemy;


uint16_t dino_bmps[] = {bmp_dino_run_0, bmp_dino_run_1, bmp_dino_idle};
uint16_t cactus_bmps[] = {bmp_cactus, bmp_cactus2, bmp_cactus3};
float cloud_x;
float cloud_y;

byte tempo = 250;
byte note = 0;

void setup() {
  gb.begin();
  gb.setFrameRate(10);
  randomSeed(analogRead(A4));    
  gb.titleScreen(F("Di-no-connection"));  
  gb.battery.show = false;
  //gb.display.persistence = false;
  gameTitle();

 //sound test
 

}


void gameTitle() {
  
  dino.offset = LCDHEIGHT;
  dino.x = 8;
  dino.y = dino.offset;
  dino.width = 24;
  dino.height = 22;
  dino.velocity_y = 0;
  dino.velocity_x = 5;
  dino.on_ground = false;
  dino.score = 0;  

  enemy.x = LCDWIDTH;
  enemy.y = 30;
  enemy.width = 8;
  enemy.height = 17;
  enemy.velocity_x = 4;
  enemy.sprite = 0;

  gravity = 1.3;
  cloud_x = 120;
  cloud_y = 2;

  gameState = 1;  
}


void gamePlay() {
  dino.score++;
  enemy.x -= enemy.velocity_x; 
  
  /* dino gravity */

  if (dino.y < 13) {
    if (gb.frameCount > fall_delay ) {
      dino.y = 13;
    }
  } else {
    dino.velocity_y += gravity;
    dino.y += dino.velocity_y;
  }

  if (dino.y > dino.offset) {
    dino.y = dino.offset;
    dino.velocity_y = 0;
    dino.on_ground = true;
  }
  
  animation();
  collision();
  


  if (gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_A)) {
    jump();
  }

  if (gb.buttons.pressed(BTN_C)){
    gameState = 2;
    gb.sound.playTick();
  }

  
}




void gamePause() {
  gb.display.cursorX = 20;
  gb.display.cursorY = 20;
  gb.display.print(F("Game Paused"));
  
  if (gb.buttons.pressed(BTN_C)) {
      gameState = 1;
      gb.sound.playTick();     
      
  }
}

void gameOver() {
  gb.display.drawBitmap(dino.x, dino.y - dino.height, bmp_dino_dead);

  if (dino.score > max_score) {
    max_score = dino.score;    
    gb.popup(F("new hi-score!!"), 20);
  }

  if (gb.buttons.pressed(BTN_B) || gb.buttons.held(BTN_B, 2) || gb.buttons.pressed(BTN_A) || gb.buttons.held(BTN_A, 2)) {
    gameState = 0;
  }


}


void music(){
  if (note < 3 && gameState == 1) {
    gb.sound.playNote(notes[note],notes[note+1]*tempo,0);
    note += 2;
  } else{
    note = 0;
  }       
}

void loop() {
  if (gb.update()) {       
    draw();
    switch (gameState) {      
      case 0: 
        gameTitle();
        break;
      case 1:
        gamePlay();
        break;
      case 2:
        gamePause();
        break;
      case 3:
        gameOver();
        break;      
    }
  }

}


void collision() {
  
  if (gb.collideBitmapBitmap(dino.x, dino.y - dino.height, dino_bmps[current], enemy.x, enemy.y, cactus_bmps[enemy.sprite])) {
    gb.sound.playTick();
    gameState = 3;
  }

  if(enemy.x < 0){
    int outScreen;
    outScreen = pgm_read_byte(cactus_bmps[enemy.sprite]);
    outScreen *= -1;
    if (enemy.x < outScreen ) {    
      enemy.x = random(84, 160);
      enemy.sprite = random(0, 3);
    }
  }
}



void animation() {
  if (dino.on_ground) {
    if (dino.velocity_x >= limit) {
      current += 1;
      if ( current > 1 ) {
        current = 0;
      }
      limit = dino.velocity_x;
    }
    limit -= 1;
  } else {
    current = 2;
  }
  gb.display.drawBitmap(dino.x, dino.y - dino.height, dino_bmps[current]);
}

void draw() {
  
  /*gb.display.fillRect(0,0,84,84);
    gb.display.setColor(WHITE, BLACK);*/  
  
  gb.display.drawBitmap(enemy.x, enemy.y, cactus_bmps[enemy.sprite]);
  gb.display.drawLine(0, 42, LCDWIDTH, 42);
  
  gb.display.print(dino.score);
  gb.display.cursorX = 60;
  gb.display.print(F("Hi"));
  gb.display.print(max_score);
  cloud();
  music();
  
}

void cloud(){
  cloud_x -= 0.3;  
  
  if (cloud_x < -20 ) {
    cloud_x = random(90, 160);
    cloud_y = random(0, 30);
  }
  gb.display.drawBitmap(cloud_x, cloud_y, bmp_cloud);  
}


void jump() {
  if (dino.on_ground) {
    dino.on_ground = false;
    dino.velocity_y -= 10;
    fall_delay =  gb.frameCount + 10;
  }
}


