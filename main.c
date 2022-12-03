#include "SDL_Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define FRAMES_PER_SECOND 60
#define TABLE_SIZE 1000


typedef struct {
    double x;
    double y;
    double size;
    double vx;
    double vy;
    double trend_x;
    double trend_y;
    SDL_Color color;
}particle;



void display_box(SDL_Renderer *r);
void draw_particle(SDL_Renderer*r, particle p);
int move_particle(particle* p);//returns 0 if particle in the box and 1 if outside










int main()
{ /*gcc -c -Wall -Wextra main.c && gcc main.o -lm -o main && ./main*/

    SDL_Color red;
    red.r = 255;
    red.g = 0;
    red.b = 0;
    red.a = 1;


    srand(time(0));
    SDL_Window *w;
    SDL_Renderer *r;

    SDL_Event evt;
    SDL_bool program_launched = SDL_TRUE;
    openSDL(WIDTH, HEIGHT, 0, &w, &r);

    particle p;

    particle *pa = malloc(TABLE_SIZE*sizeof(particle));

    for(int i = 0 ; i < TABLE_SIZE ; i++){
        pa[i].x = WIDTH/2;
        pa[i].y = HEIGHT/2;
        pa[i].vx = 0;
        pa[i].vy = 0;
        pa[i].size = 20;
        pa[i].trend_x = (rand()/(float)RAND_MAX - 0.5);
        pa[i].trend_y = (rand()/(float)RAND_MAX - 0.5);
        pa[i].color = red;
    }

    p.x = WIDTH/2;
    p.y = HEIGHT/2;
    p.vx = 0;
    p.vy = 0;
    p.size = 10;
    p.trend_x = 0.05;
    p.trend_y = -0.1;
    p.color = red;



    while (program_launched)
    {
        /*if(move_particle(&p) == 1)
            printf("ball exiting\n");

        draw_particle(r, p);*/

        for(int i = 0 ; i < TABLE_SIZE ; i++)
            move_particle(pa + i);

        background(r, 255, 255, 255, WIDTH, HEIGHT);
        display_box(r);

        for(int i = 0 ; i < TABLE_SIZE ; i++)
            draw_particle(r, pa[i]);



        while (SDL_PollEvent(&evt))
        { // reads all the events (mouse moving, key pressed...)        //possible to wait for an event with SDL_WaitEvent
            switch (evt.type)
            {

            case SDL_QUIT:
                program_launched = SDL_FALSE; // quit the program if the user closes the window
                break;

            case SDL_KEYDOWN: // SDL_KEYDOWN : hold a key            SDL_KEYUP : release a key
                switch (evt.key.keysym.sym)
                { // returns the key ('0' ; 'e' ; 'SPACE'...)

                case SDLK_ESCAPE:
                    program_launched = SDL_FALSE; // escape the program by pressing esc
                    break;

                case SDLK_l:
                    break;

                default:
                    break;
                }
            case SDL_MOUSEBUTTONUP:
                break;

            default:
                break;
            }
        }

        SDL_RenderPresent(r); // refresh the render
        SDL_Delay(1000 / FRAMES_PER_SECOND);
    }


    free(pa);
    closeSDL(&w, &r);
    printf("Closed sucessfully !\n");
    return 0;
}



void display_box(SDL_Renderer* r){
    color(r, 0, 0, 0, 1);
    line(r, WIDTH/10, HEIGHT/10, WIDTH/10, HEIGHT*0.9);
    line(r, WIDTH/10, HEIGHT/10, WIDTH*0.9, HEIGHT/10);
    line(r, WIDTH/10, HEIGHT*0.9, WIDTH*0.9, HEIGHT*0.9);
    line(r, WIDTH*0.9, HEIGHT*0.9, WIDTH*0.9, HEIGHT*0.55);
    line(r, WIDTH*0.9, HEIGHT*0.1, WIDTH*0.9, HEIGHT*0.45);
}

void draw_particle(SDL_Renderer*r, particle p){
    color(r, p.color.r, p.color.g, p.color.b, p.color.a);
    circle(r, p.x, p.y, p.size, 1);
}

int move_particle(particle* p){
    //============================== bounce =============================
    if(p->y - p->size < HEIGHT*0.1)
        p->vy *= -1;
    else if(p->y + p->size > HEIGHT*0.9)
        p->vy *= -1;
    if(p->x - p->size < WIDTH*0.1)
        p->vx *= -1;
    else if((p->y - p->size < HEIGHT*0.45 || p->y  + p->size> HEIGHT*0.55) && p->x + p->size> WIDTH*0.9)
        p->vx *= -1;
    //==================================================================
    
    p->x += p->vx;
    p->y += p->vy;

    p->vx += (rand()/(float)RAND_MAX - 0.5)*p->trend_x;
    p->vy += (rand()/(float)RAND_MAX - 0.5)*p->trend_y;

    if(p->x > WIDTH*0.1 - 5 && p->y > HEIGHT*0.1 - 5&& p->x < WIDTH*0.9 + 5&& p->y < HEIGHT*0.9 + 5)
        return 0;
    else
        return 1;
}
