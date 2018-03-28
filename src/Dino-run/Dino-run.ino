#include <SPI.h>
#include <Gamebuino.h>
#include "Bitmaps.h"

Gamebuino gb;

struct player{
  bool dead;
  bool on_ground; 
  int x;
  int width;
  int height;
  int offset;
  int score;    
  float y;   
  float velocity_x;
  float velocity_y;
};

struct obstacle
{
  int x; 
  int y;
  int width;
  int height;
  int velocity_x; 
};
 

//animation
int current = 0;
int limit = 0;   

int max_score = 0;
int jump_delay= 0;

float gravity;

struct player dino;
struct obstacle enemy;


//uint16_t dino_sprites[] = {bmp_dino_run_0, bmp_dino_run_1, bmp_dino_idle, bmp_dino_dead};
 
void setup(){  
  initVariables();  
  gb.begin();  
  gb.setFrameRate(10);
  gb.titleScreen(F("Di-no-connection"));
  gb.battery.show = false;
}

void resetGame(){
    initVariables();
}

void initVariables(){
  dino.offset = LCDHEIGHT-2;
  dino.x = 8;
  dino.y = dino.offset;  
  dino.width = 24;
  dino.height = 22;  
  dino.velocity_y = 0;
  dino.velocity_x = 5;
  dino.on_ground = false;
  dino.score = 0;  
  dino.dead = false;  
  
  enemy.x = LCDWIDTH;
  enemy.y = 30;
  enemy.width = 8;
  enemy.height = 17;
  enemy.velocity_x = 2;    
  
  gravity = 1.3;

}

void jump(){
 if(dino.on_ground){          
  dino.on_ground = false;            
  dino.velocity_y-=10;                                          
 }                
}

void draw(){
  if( dino.dead){
    //gb.display.drawBitmap(dino.x, dino.y - dino.height, dino.bitmaps[3]);
    gb.display.drawBitmap(dino.x, dino.y - dino.height, bmp_dino_dead);
    if(dino.score > max_score){
        max_score = dino.score;
        gb.popup(F("new hi-score!!"), 20);
      }
  }else{
    animation();
  } 
  
  gb.display.drawBitmap(enemy.x, enemy.y, bmp_cactus);
  gb.display.drawLine(0, 42, LCDWIDTH, 42);
  
  if(max_score > 0){
    gb.display.print(F("Hi"));
    gb.display.print(max_score);
  }
  gb.display.cursorX = 60;
  gb.display.print(dino.score);    
  
}

void events(){   
    if(gb.buttons.pressed(BTN_B) || gb.buttons.held(BTN_B,2)){          
     if(dino.dead){
       resetGame();
      }else{
        jump();
      }
    }       
    if (gb.buttons.pressed(BTN_C)) { 
      gb.titleScreen(F("Di-no-connection"));
    }   

    if (gb.buttons.pressed(BTN_UP)) { 
      if(dino.velocity_x > 0){dino.velocity_x-=1;}
        enemy.velocity_x+=1;
      }
 }

void collisions(){     
  if(gb.collideBitmapBitmap(dino.x, dino.y - dino.height, bmp_dino_run_1, enemy.x, enemy.y, bmp_cactus)){
    gb.sound.playTick();      
    dino.dead= true;              
    //gb.popup(F("YOU LOST!"), 5);       
  }
}  



void animation(){  

  if(dino.on_ground){
    if(dino.velocity_x >= limit){
        current += 1;
        if ( current > 1 ) { current = 0; }
        limit = dino.velocity_x;
    }
    if(current == 0){gb.display.drawBitmap(dino.x, dino.y - dino.height, bmp_dino_run_0); }
    if(current == 1){gb.display.drawBitmap(dino.x, dino.y - dino.height, bmp_dino_run_1); }    
    limit-=1;    
  }else{
    gb.display.drawBitmap(dino.x, dino.y - dino.height, bmp_dino_idle);
  }
  
}


void loop() {
  if (gb.update()) {       
    if(!dino.dead){ 
      dino.score++;    
      dino.velocity_y += gravity;         
      dino.y += dino.velocity_y;
      
      enemy.x -= enemy.velocity_x;
  
      if(enemy.x < -9 || enemy.x > LCDWIDTH ){        
        enemy.x = LCDWIDTH;        
      }
             
      if(dino.y > dino.offset){
        dino.y = dino.offset;
        dino.velocity_y = 0;
        dino.on_ground = true;
      }
      
      collisions();
      
    }

      events();
      draw();

        
}}


   

