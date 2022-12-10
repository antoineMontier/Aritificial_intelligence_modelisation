#include "SDL_Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define FRAMES_PER_SECOND 60
#define PARTICLES_NUMBER 500
#define NB_COLORS 5
#define ADN_TRANSMISSION 0.5
#define AUTO_TRANSMISSION 0.1
#define PARTICLE_SPEED 15

typedef struct
{
    int round;
    double p_x;
    double p_y;
    double x;
    double y;
    double size;
    SDL_Color color;
} particle;

void display_box(SDL_Renderer *r);
void draw_particle(SDL_Renderer *r, particle p);
int move_particle(particle *p); // returns 0 if particle in the box and 1 if outside
void display_particle_informations(SDL_Renderer *r, TTF_Font *f, particle *p, char *tmp, int round, int bestmove);
int initialise_particle(particle *p, SDL_Color *c);
void inherit_particle(particle *source, particle *target);
void copy_particle(particle *source, particle *target);
void close_Particle(particle **p);
void draw_path(SDL_Renderer *r, SDL_Point *p, int bestmove);

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
    SDL_Color yellow;
    yellow.r = 255;
    yellow.g = 255;
    yellow.b = 0;
    yellow.a = 1;
    SDL_Color purple;
    purple.r = 255;
    purple.g = 0;
    purple.b = 255;
    purple.a = 1;

    srand(time(0));
    SDL_Window *w;
    SDL_Renderer *r;

    SDL_Event evt;
    SDL_bool program_launched = SDL_TRUE;
    openSDL(WIDTH, HEIGHT, 0, &w, &r);

    TTF_Font *param_font;

    setFont(&param_font, "Roboto_Black.ttf", 20);
    SDL_Color *colors = malloc(NB_COLORS * sizeof(SDL_Color));

    colors[0] = red;
    colors[1] = green;
    colors[2] = blue;
    colors[3] = yellow;
    colors[4] = purple;
    char *tmp = malloc(10);
    double oldtime = SDL_GetTicks(), newtime = SDL_GetTicks(), dt = 0.0, real_fps = 0.0;

    particle *part = malloc(PARTICLES_NUMBER * sizeof(particle));

    int iterations = 0;

    while (program_launched)
    {
        background(r, 255, 255, 255, WIDTH, HEIGHT);
        // for (int i = 0; i < PARTICLES_NUMBER; i++)
        //     draw_particle(r, part[i]);

        display_box(r);

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
        oldtime = newtime;
        newtime = SDL_GetTicks();
        dt = (newtime - oldtime);
        real_fps = 1000/dt;
        printf("fps = %3.1lf\titerations = %d\n", real_fps, iterations);
        iterations++;
        SDL_Delay(1000 / FRAMES_PER_SECOND);
        SDL_RenderPresent(r); // refresh the render
    }
    free(part);
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
    line(r, WIDTH * 0.9, HEIGHT * 0.9, WIDTH * 0.9, HEIGHT * 0.1);
}

void draw_particle(SDL_Renderer *r, particle p)
{
    color(r, p.color.r, p.color.g, p.color.b, p.color.a);
    circle(r, p.x, p.y, p.size, 1);
}

int move_particle(particle *p)
{

    return 0;
}

void display_particle_informations(SDL_Renderer *r, TTF_Font *f, particle *p, char *tmp, int round, int bestmove)
{

    text(r, WIDTH * 0, HEIGHT * 0, "moves", f, 0, 0, 0);
    text(r, WIDTH * 0.1, HEIGHT * 0, "round", f, 0, 0, 0);
    text(r, WIDTH * 0.2, HEIGHT * 0, "best move", f, 0, 0, 0);

    gcvt(p[0].round, 4, tmp);
    text(r, WIDTH * 0, HEIGHT * 0.02, tmp, f, 0, 0, 0);
    gcvt(round, 4, tmp);
    text(r, WIDTH * 0.1, HEIGHT * 0.02, tmp, f, 0, 0, 0);
    gcvt(bestmove, 4, tmp);
    text(r, WIDTH * 0.2, HEIGHT * 0.02, tmp, f, 0, 0, 0);
}

int initialise_particle(particle *p, SDL_Color *c)
{
    p->x = WIDTH * 0.5;
    p->y = HEIGHT * 0.5;
    p->size = 20;
    int col = rand() % NB_COLORS;
    p->color.r = c[col].r;
    p->color.g = c[col].g;
    p->color.b = c[col].b;
    p->color.a = c[col].a;
    p->round = 0;
    return 1;
}

void inherit_particle(particle *source, particle *target)
{
}

void copy_particle(particle *source, particle *target)
{

    target->x = source->x;
    target->p_x = source->p_x;
    target->y = source->y;
    target->p_y = source->p_y;
    target->size = source->size;
    target->color.r = source->color.r;
    target->color.g = source->color.g;
    target->color.b = source->color.b;
    target->color.a = source->color.a;
    target->round = source->round;
}
