#include "SDL_Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define FRAMES_PER_SECOND 1000
#define PARTICLES_NUMBER 300
#define NB_COLORS 5
#define ADN_TRANSMISSION 0.1
#define COMMAND_NUMBER 1000 // 5 commands : 0 nothing 1 up 2 down 3 left 4 right
#define PARTICLE_SPEED 5

typedef struct
{
    int round;
    double p_x;
    double p_y;
    double x;
    double y;
    double size;
    int *seed;
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
void load_positions(particle *src, SDL_Point*target, int moves);

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
    SDL_Point *points = malloc(10000 * sizeof(SDL_Point));
    colors[0] = red;
    colors[1] = green;
    colors[2] = blue;
    colors[3] = yellow;
    colors[4] = purple;
    char *tmp = malloc(10);
    particle *part = malloc(PARTICLES_NUMBER * sizeof(particle));

    particle temp, best;
    initialise_particle(&temp, colors);
    initialise_particle(&best, colors);
    for (int i = 0; i < PARTICLES_NUMBER; i++)
        initialise_particle(&part[i], colors);

    int iterations = 0, round = 0, bestMove = 999999;

    while (program_launched)
    {
        printf("%d\n", iterations);
        for (int i = 0; i < PARTICLES_NUMBER; i++)
        {
            if (move_particle(&part[i]) == 0)
            {
                printf("regeneration\n");
                if (bestMove > part[i].round)
                {
                    bestMove = part[i].round;
                    copy_particle(&part[i], &best);
                    load_positions(&part[i], points, bestMove);
                }
                copy_particle(&part[i], &temp);
                for (int j = 0; j < PARTICLES_NUMBER; j++)
                    inherit_particle(&temp, &part[j]);
                iterations = 0;
                round++;
                i = PARTICLES_NUMBER;
                printf("enf of regeneration\n");
            }
        }

        background(r, 255, 255, 255, WIDTH, HEIGHT);
        for (int i = 0; i < PARTICLES_NUMBER; i++)
            draw_particle(r, part[i]);
        // printf("%d\n", iterations);
        display_box(r);
        display_particle_informations(r, param_font, part, tmp, round, bestMove);
        if (round > 0 && iterations > 1)
            draw_path(r, points, bestMove);

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
        iterations++;
        SDL_RenderPresent(r); // refresh the render
        SDL_Delay(1000 / FRAMES_PER_SECOND);
    }
    close_Particle(&part);
    free(points);
    free(best.seed);
    free(temp.seed);
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
    circle(r, WIDTH * 0.7, HEIGHT * 0.55, HEIGHT * 0.05, 0);
}

void draw_particle(SDL_Renderer *r, particle p)
{

    color(r, p.color.r, p.color.g, p.color.b, p.color.a);
    circle(r, p.x, p.y, p.size, 1);
}

int move_particle(particle *p)
{

    //============================== bounce =============================

    //==================================================================
    switch (p->seed[p->round % 1000])
    {
    case 1:
        p->y -= PARTICLE_SPEED;
        // printf("up\n");
        break;
    case 2:
        p->y += PARTICLE_SPEED;
        // printf("down\n");
        break;
    case 3:
        p->x -= PARTICLE_SPEED;
        // printf("left\n");
        break;
    case 4:
        p->x += PARTICLE_SPEED;
        // printf("right\n");
        break;
    default:
        break;
    }
    p->round += 1;
    if (p->y - p->size < HEIGHT * 0.1 || p->y + p->size > HEIGHT * 0.9)
        p->y = p->p_y;
    if (p->x - p->size < WIDTH * 0.1 || ((p->y - p->size < HEIGHT * 0.45 || p->y + p->size > HEIGHT * 0.55) && p->x + p->size > WIDTH * 0.9))
        p->x = p->p_x;
    if (dist(WIDTH * 0.7, HEIGHT * 0.55, p->x, p->y) < HEIGHT * 0.05 + p->size)
    {
        p->y = p->p_y;
        p->x = p->p_x;
    }

    p->p_x = p->x;
    p->p_y = p->y;

    if (p->x + p->size > WIDTH * 0.9 && p->y < HEIGHT * 0.55 && p->y > HEIGHT * 0.45)
    {
        return 0;
    }
    return 1;
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
    p->seed = malloc(COMMAND_NUMBER * sizeof(int));
    for (int i = 0; i < COMMAND_NUMBER; i++)
        p->seed[i] = rand() % 5;
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
    for (int i = 0; i < COMMAND_NUMBER; i++)
        if (rand() / (float)RAND_MAX < ADN_TRANSMISSION) // transmission
            target->seed[i] = source->seed[i];
        else                                   // keep the command
            target->seed[i] = target->seed[i]; //=nothing
    target->round = 0;
    target->x = WIDTH / 2;
    target->y = HEIGHT / 2;
    target->size = 20;
}

void copy_particle(particle *source, particle *target)
{
    for (int i = 0; i < COMMAND_NUMBER; i++)
        target->seed[i] = source->seed[i];
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

void close_Particle(particle **p)
{
    for (int i = 0; i < PARTICLES_NUMBER; i++)
    {
        if ((*p)[i].seed == NULL)
            printf("line %d hasn't been destroyed yet\n", i);
        else
        {
            free((*p)[i].seed);
            (*p)[i].seed = NULL;
        }
    }
    if (*p == NULL)
        printf("particle array hasn't been destroyed yet\n");
    else
    {
        free(*p);
        *p = NULL;
    }
}

void draw_path(SDL_Renderer *r, SDL_Point *p, int bestmove)
{
    color(r, 0, 0, 0, 1);
    for(int i = 1; i <bestmove; i++){
        line(r, p[i].x, p[i].y, p[i-1].x, p[i-1].y);
    }


}

void load_positions(particle *src, SDL_Point*target, int moves){
    printf("%d\n", moves);
    src->x = src->p_x = WIDTH/2;
    src->y = src->p_y = HEIGHT/2;
    for (int i = 0; i < moves; i++){
        move_particle(src);
        target[i].x = src->x;
        target[i].y = src->y;
    }
}

