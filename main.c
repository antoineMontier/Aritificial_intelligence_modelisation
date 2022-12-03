#include "SDL_Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define FRAMES_PER_SECOND 60
#define TABLE_SIZE 1000
#define NB_COLORS 3

typedef struct
{
    double x;
    double y;
    double size;
    double vx;
    double vy;
    double seed_x;
    double seed_y;
    SDL_Color color;
} particle;

void display_box(SDL_Renderer *r);
void draw_particle(SDL_Renderer *r, particle p);
int move_particle(particle *p); // returns 0 if particle in the box and 1 if outside
void display_particle_informations(SDL_Renderer *r, TTF_Font *f, particle p, char *tmp, const char *position);
int initialise_particle(particle *p, SDL_Color*c);

int main()
{ /*gcc -c -Wall -Wextra main.c && gcc main.o -lm -o main && ./main*/

    SDL_Color red;
    red.r = 255;
    red.g = 0;
    red.b = 0;
    red.a = 1;
    SDL_Color green;
    green.r = 0;
    green.g = 255;
    green.b = 0;
    green.a = 1;
    SDL_Color blue;
    blue.r = 0;
    blue.g = 0;
    blue.b = 255;
    blue.a = 1;



    srand(time(0));
    SDL_Window *w;
    SDL_Renderer *r;

    SDL_Event evt;
    SDL_bool program_launched = SDL_TRUE;
    openSDL(WIDTH, HEIGHT, 0, &w, &r);

    TTF_Font *param_font;

    setFont(&param_font, "Roboto_Black.ttf", 20);
    SDL_Color*colors = malloc(NB_COLORS*sizeof(SDL_Color));
    colors[0] = red; colors[1] = green; colors[2] = blue;
    char *tmp = malloc(10);
    particle red_particle, green_particle;
    initialise_particle(&red_particle, colors);
    initialise_particle(&green_particle, colors);

    while (program_launched)
    {

        if (move_particle(&red_particle) == 1)
            printf("red ball exiting\n");
        if (move_particle(&green_particle) == 1)
            printf("green ball exiting\n");

        background(r, 255, 255, 255, WIDTH, HEIGHT);
        draw_particle(r, red_particle);
        draw_particle(r, green_particle);

        display_box(r);
        display_particle_informations(r, param_font, red_particle, tmp, "up");
        display_particle_informations(r, param_font, green_particle, tmp, "down");


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
    free(colors);
    free(tmp);
    TTF_CloseFont(param_font);
    closeSDL(&w, &r);
    printf("Closed sucessfully !\n");
    return 0;
}

void display_box(SDL_Renderer *r)
{
    color(r, 0, 0, 0, 1);
    line(r, WIDTH / 10, HEIGHT / 10, WIDTH / 10, HEIGHT * 0.9);
    line(r, WIDTH / 10, HEIGHT / 10, WIDTH * 0.9, HEIGHT / 10);
    line(r, WIDTH / 10, HEIGHT * 0.9, WIDTH * 0.9, HEIGHT * 0.9);
    line(r, WIDTH * 0.9, HEIGHT * 0.9, WIDTH * 0.9, HEIGHT * 0.55);
    line(r, WIDTH * 0.9, HEIGHT * 0.1, WIDTH * 0.9, HEIGHT * 0.45);
}

void draw_particle(SDL_Renderer *r, particle p)
{

    color(r, p.color.r, p.color.g, p.color.b, p.color.a);
    circle(r, p.x, p.y, p.size, 1);
}

int move_particle(particle *p)
{
    //============================== bounce =============================
    if (p->y - p->size < HEIGHT * 0.1)
        p->vy *= -1;
    else if (p->y + p->size > HEIGHT * 0.9)
        p->vy *= -1;
    if (p->x - p->size < WIDTH * 0.1)
        p->vx *= -1;
    else if ((p->y - p->size < HEIGHT * 0.45 || p->y + p->size > HEIGHT * 0.55) && p->x + p->size > WIDTH * 0.9)
        p->vx *= -1;
    //==================================================================

    p->x += p->vx ;
    p->y += p->vy ;

    p->vx += (rand() / (float)RAND_MAX - 0.5) + p->seed_x;
    p->vy += (rand() / (float)RAND_MAX - 0.5) + p->seed_y;

    if (p->x + p->size > WIDTH * 0.1 - 5 && p->y + p->size > HEIGHT * 0.1 - 5 && p->x - p->size < WIDTH * 0.9 + 5 && p->y - p->size < HEIGHT * 0.9 + 5)
        return 0;
    else
        return 1;
}

void display_particle_informations(SDL_Renderer *r, TTF_Font *f, particle p, char *tmp, const char *position)
{
    if (strcmp(position, "up") == 0)
    {
        text(r, WIDTH * 0, HEIGHT * 0, "vx", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.1, HEIGHT * 0, "vy", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.2, HEIGHT * 0, "seed x", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.3, HEIGHT * 0, "seed y", f, p.color.r, p.color.g, p.color.b);

        gcvt(p.vx, 3, tmp);
        text(r, WIDTH * 0, HEIGHT * 0.02, tmp, f, p.color.r, p.color.g, p.color.b);
        gcvt(p.vy, 3, tmp);
        text(r, WIDTH * 0.1, HEIGHT * 0.02, tmp, f, p.color.r, p.color.g, p.color.b);
        gcvt(p.seed_x, 4, tmp);
        text(r, WIDTH * 0.2, HEIGHT * 0.02, tmp, f, p.color.r, p.color.g, p.color.b);
        gcvt(p.seed_y, 4, tmp);
        text(r, WIDTH * 0.3, HEIGHT * 0.02, tmp, f, p.color.r, p.color.g, p.color.b);
    }else if(strcmp(position, "down")==0){


        text(r, WIDTH * 0, HEIGHT * 0.95, "vx", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.1, HEIGHT * 0.95, "vy", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.2, HEIGHT * 0.95, "seed x", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.3, HEIGHT * 0.95, "seed y", f, p.color.r, p.color.g, p.color.b);

        gcvt(p.vx, 3, tmp);
        text(r, WIDTH * 0, HEIGHT * 0.97, tmp, f, p.color.r, p.color.g, p.color.b);
        gcvt(p.vy, 3, tmp);
        text(r, WIDTH * 0.1, HEIGHT * 0.97, tmp, f, p.color.r, p.color.g, p.color.b);
        gcvt(p.seed_x, 4, tmp);
        text(r, WIDTH * 0.2, HEIGHT * 0.97, tmp, f, p.color.r, p.color.g, p.color.b);
        gcvt(p.seed_y, 4, tmp);
        text(r, WIDTH * 0.3, HEIGHT * 0.97, tmp, f, p.color.r, p.color.g, p.color.b);
    }
}

int initialise_particle(particle *p, SDL_Color*c){
    p->vx = 0;
    p->vy = 0;
    p->seed_x = rand()/(float)RAND_MAX - 0.5;
    p->seed_y = rand()/(float)RAND_MAX - 0.5;
    p->x = WIDTH/2;
    p->y = HEIGHT/2;
    p->size = 20;
    int col = rand() % NB_COLORS;
    p->color.r = c[col].r;
    p->color.g = c[col].g;
    p->color.b = c[col].b;
    p->color.a = c[col].a;
    return 1;
}




