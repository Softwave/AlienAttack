#include <allegro.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <random.h>
#include "util.h"

// Resources
BITMAP *bmp_bg;
BITMAP *screen2;
BITMAP *bmp_plr;
BITMAP *bmp_bullet;
BITMAP *bmp_enemy;
PALETTE the_palette;
SAMPLE *SND_BOOM;
//

// Defs
void g_update(void);
void g_draw(void);
void g_title(void);
void g_over(void);
#define NUM_ENEMY 10
#define NUM_AMMO 50

// Structures
typedef struct Player {
        Box box;
        bool isalive;
} PLAYER;

typedef struct Enemy {
        Box box;
        int speed;
        bool isalive;
} ENEMY;

typedef struct Bullet {
        Box box;
        int speed;
        bool isalive;
} BULLET;

// Variables
PLAYER plr;
ENEMY enemy[10];
BULLET bullet[NUM_AMMO];
int score = 0;
int tokill = 10;
bool gameover;

int main(void)
{
        // Setup allegro
        if (allegro_init() != 0)
        {
                return 1;
        }
        install_keyboard();

        // TODO: Make this safer
        // Setup GFX
        set_gfx_mode(GFX_AUTODETECT,320,200,0,0);
        clear_to_color(screen,makecol(0,0,0));
        // Setup sound
        install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL);
        set_volume(255,255);

        // Load resources
        screen2 = create_bitmap(screen->w,screen->h);
        bmp_bg = load_bitmap("bg.pcx",the_palette);
        bmp_plr = load_bitmap("plr.pcx",the_palette);
        bmp_enemy = load_bitmap("alien.pcx",the_palette);
        bmp_bullet = load_bitmap("bullet.pcx",the_palette);
        SND_BOOM = load_wav("boom.wav");

        // Init vars
        gameover = false;

        // Setup entities
        plr.box.x = 160;
        plr.box.y = 100;
        plr.box.width = 10;
        plr.box.height = 10;
        plr.isalive = true;
        // Enemies
        for (int i = 0; i < 10; i++)
        {
                enemy[i].box.width = 10;
                enemy[i].box.height = 10;
                enemy[i].box.x = rand() % 320 + 320;
                enemy[i].box.y = rand() % 200;
                enemy[i].speed = 1;
                enemy[i].isalive = true;
        }
        // Bullets
        for (int i = 0; i < NUM_AMMO; i++)
        {
                bullet[i].box.x = plr.box.x;
                bullet[i].box.y = plr.box.y + 5;
                bullet[i].box.width = 3;
                bullet[i].box.height = 3;
                bullet[i].speed = 3;
                bullet[i].isalive = false;
        }

        // Main loop
        g_title();
        readkey();
        while (!gameover)
        {
                g_update();
                g_draw();
        } // end of main loop
        g_over();
        while (!key[KEY_ESC]) {}
        return 0;
}

void g_update()
{
        // Move player
        if (key[KEY_RIGHT]) ++ plr.box.x;
        if (key[KEY_LEFT]) -- plr.box.x;
        if (key[KEY_UP]) -- plr.box.y;
        if (key[KEY_DOWN]) ++ plr.box.y;

        // Col enemy
        if (plr.isalive)
        {
            for (int i = 0; i < NUM_ENEMY; i++)
            {
                if (CheckCol(plr.box,enemy[i].box))
                {
                    // Player is killed
                    if (enemy[i].isalive)
                    {
                      plr.isalive = false;
                      enemy[i].isalive = false;
                      play_sample(SND_BOOM,255,0,1000,0);
                      gameover = true;
                    }
                }
            }
        }

        // Shoot
        if (key[KEY_SPACE])
        {
                for (int i = 0; i < NUM_AMMO; i++)
                {
                        if (!bullet[i].isalive)
                        {
                                bullet[i].box.x = plr.box.x;
                                bullet[i].box.y = plr.box.y + 5;
                                bullet[i].isalive = true;
                        }
                }
        } // End shoot

        // If killed all enemies
        if (score == tokill)
        {
                for (int i = 0; i < NUM_ENEMY; i++)
                {
                        if (!enemy[i].isalive)
                        {
                            enemy[i].box.x = rand() % 320 + 320;
                            enemy[i].box.y = rand() % 200;
                            enemy[i].speed += 1;
                            enemy[i].isalive = true;  
                        }
                }
                tokill = tokill + 10;
        }

        // Move enemies
        for (int i = 0; i < NUM_ENEMY; i++)
        {
                if (enemy[i].isalive)
                {
                        enemy[i].box.x -= enemy[i].speed;
                        // If enemy falls off world
                        if (enemy[i].box.x < 0)
                        {
                                enemy[i].box.x = rand() % 320 + 320;
                                enemy[i].box.y = rand() % 200;
                        }
                }
        }

        // Move bullest
        for (int i = 0; i < NUM_AMMO; i++)
        {
                if (bullet[i].isalive)
                {
                        bullet[i].box.x += bullet[i].speed;
                        // If bullet leaves game
                        if (bullet[i].box.x > 320)
                        {
                                bullet[i].isalive = false;
                        }
                        // If bullet hits enemy
                        for (int j = 0; j < NUM_ENEMY; j++)
                        {
                            if (enemy[j].isalive) {
                                if (CheckCol(bullet[i].box,enemy[j].box)) {
                                    enemy[j].isalive = false;
                                    bullet[i].isalive = false;
                                    play_sample(SND_BOOM,255,0,1000,0);
                                    score++;
                                    break;
                                }
                            }
                        }         
                }         
        }
}
void g_draw()
{
        clear_to_color(bmp_bg,makecol(0,0,0));
        clear_bitmap(screen2);

        // Draw entities
        // Player
        if (plr.isalive)
        {
            draw_sprite(screen2,bmp_plr,plr.box.x,plr.box.y);
        }
        // Enemies
        for (int i = 0; i < NUM_ENEMY; i++)
        {
            if (enemy[i].isalive)
            {
                draw_sprite(screen2,bmp_enemy,enemy[i].box.x,enemy[i].box.y);
            }
        }
        // Bullets
        for (int c = 0; c < NUM_AMMO; c++)
        {
                if (bullet[c].isalive)
                {
                        draw_sprite(screen2,bmp_bullet,
                        bullet[c].box.x,bullet[c].box.y);
                }
        }

        // Draw Text
        textprintf_ex(screen2,font,16,16,makecol(255,255,255),-1,
        "SCORE: %d",score);

        // Blit
        set_palette(the_palette);
        blit(bmp_bg,screen,0,0,0,0,screen->w,screen->h);
        draw_sprite(screen,screen2,0,0);
}
void g_title()
{
        clear_to_color(screen,makecol(0,0,0));
        textout_centre_ex(screen,font,"ALIEN ATTACK",SCREEN_W/2,SCREEN_H/2,
        makecol(255,255,255),-1);
        textout_centre_ex(screen,font,"Press any key to play",SCREEN_W/2,SCREEN_H-16,
        makecol(255,255,255),-1);
}
void g_over()
{
    clear_to_color(screen,makecol(0,0,0));
    textout_centre_ex(screen, font, "GAME OVER", SCREEN_W/2, SCREEN_H/2, makecol(255,255,255), -1);
    textprintf_ex(screen, font,16,16,makecol(255,255,255),-1,
    "SCORE: %d",score);
    textout_centre_ex(screen,font,"Press ESC to quit",SCREEN_W/2,SCREEN_H-16,makecol(255,255,255),-1);
}
