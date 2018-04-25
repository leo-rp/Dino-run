  #include <Gamebuino.h>

  #include <SPI.h>

  #include "Bitmaps.h"

  Gamebuino gb;

  struct player{
    bool dead;
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
  };
   

  //animation
  byte current = 0;
  byte limit = 0;   
  byte fall_delay= 0;

  int old_frame;

  int max_score = 0;
  float gravity;

  struct player dino;
  struct obstacle enemy;
  long randomNumber;

  uint16_t dino_bmps[] = {bmp_dino_run_0, bmp_dino_run_1, bmp_dino_idle};
   
  void setup(){    
    //Serial.begin(9600);
    initVariables();  
    gb.begin();  
    gb.setFrameRate(10);
    //gb.pickRandomSeed();
    randomSeed(analogRead(A4));  
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
    fall_delay =  gb.frameCount + 10;
   }                
  }

  void dinoDead(){
    gb.display.drawBitmap(dino.x, dino.y - dino.height, bmp_dino_dead);
    gb.sound.playTick();      
    dino.dead= true;              

    if(dino.score > max_score){
      max_score = dino.score;
      gb.popup(F("new hi-score!!"), 20);
    }
  }

  void draw(){
    if(!dino.dead){
      animation();
    } 
    
    gb.display.drawBitmap(enemy.x, enemy.y, bmp_cactus);
    gb.display.drawLine(0, 42, LCDWIDTH, 42);

    gb.display.drawBitmap(enemy.x+20, 12, bmp_cloud);
     
    gb.display.print(F("Hi"));
    gb.display.print(max_score);
    
    gb.display.cursorX = 60;
    gb.display.print(randomNumber);    
    
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

    //just for testing
    if (gb.buttons.pressed(BTN_UP)) { 
      if(dino.velocity_x > 0){
        dino.velocity_x-=1;
      }
      enemy.velocity_x+=1;
    }
  }

  void collisions(){     
    if(gb.collideBitmapBitmap(dino.x, dino.y - dino.height, dino_bmps[current], enemy.x, enemy.y, bmp_cactus)){      
      dinoDead();     
    }

    if(enemy.x < -9 ){                   
      randomNumber = random(70,150);
      enemy.x = randomNumber;
    }
  }  


  void animation(){  
    if(dino.on_ground){
      if(dino.velocity_x >= limit){
          current += 1;
          if ( current > 1 ){ 
            current = 0; 
          }
          limit = dino.velocity_x;
      }  
      limit-=1;    
    }else{
      current = 2;
    }
    gb.display.drawBitmap(dino.x, dino.y - dino.height, dino_bmps[current]);
  }

  void dinoGravity(){
    if(dino.y < 13){
      if(gb.frameCount > fall_delay ){
        dino.y = 13;
      }
    }else{
      dino.velocity_y += gravity;         
      dino.y += dino.velocity_y;  
    }

    if(dino.y > dino.offset){
      dino.y = dino.offset;
      dino.velocity_y = 0;
      dino.on_ground = true;
    }
  }


  void loop() {
    if (gb.update()) {       
      if(!dino.dead){ 
        dino.score++;
        enemy.x -= enemy.velocity_x;
        dinoGravity();
        collisions();
      } 
      events();
      draw();          
    }
  }


     

