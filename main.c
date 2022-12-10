#include "SDL_Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define FRAMES_PER_SECOND 24
#define PARTICLES_NUMBER 200
#define NB_COLORS 6
#define ADN_TRANSMISSION 0.5
#define AUTO_TRANSMISSION 0.1
#define PARTICLE_SPEED 2
#define DAY_LENGTH 4
#define TRANSITION_TIME 5
#define HOUSE_SIZE 200
#define HOUSE_X (WIDTH / 2 - HOUSE_SIZE/2)
#define HOUSE_Y (HEIGHT/ 2 - HOUSE_SIZE/2)

typedef struct
{
    int alive;
    int round;
    double angle;                // destination angle in rad
    double destination_distance; // destination distance
    int time_to_go;              // max amout of time to reach destination
    double speed;                // speed
    double x;
    double y;
    double size;
    SDL_Color color;
} particle;

void display_box(SDL_Renderer *r);
void draw_particle(SDL_Renderer *r, particle p);
int move_particle(particle *p, double animation, int home); // returns 0 if particle in the box and 1 if outside
void display_particle_informations(SDL_Renderer *r, TTF_Font *f, particle *p, char *tmp, int round, int bestmove);
int initialise_particle(particle *p, SDL_Color *c);
void inherit_particle(particle *source, particle *target);
void copy_particle(particle *source, particle *target);
void close_Particle(particle **p);
void draw_path(SDL_Renderer *r, SDL_Point *p, int bestmove);
void setDestination(particle *p, int x, int y);

int main()
{ /*gcc -c -Wall -Wextra main.c && gcc main.o -lm -o main && ./main*/
    srand(time(0));

    SDL_Color red = {255, 0, 0, 1};
    SDL_Color green = {0, 255, 0, 1};
    SDL_Color blue = {0, 0, 255, 1};
    SDL_Color yellow = {255, 255, 0, 1};
    SDL_Color purple = {255, 0, 255, 1};
    SDL_Color cyan = {0, 150, 200, 1};

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
    colors[5] = cyan;
    char *tmp = malloc(10); // transition : 1 is day, 0 is night
    double oldtime = SDL_GetTicks(), newtime = SDL_GetTicks(), calcul_time = 0.0, real_fps = 0.0, a = 0, dt = 0, transition = 1.0;

    particle *part = malloc(PARTICLES_NUMBER * sizeof(particle));
    for (int i = 0; i < PARTICLES_NUMBER; i++)
        initialise_particle(&part[i], colors);

    int iterations = 0, start_time = 0, day_plus_night = DAY_LENGTH * 2 + TRANSITION_TIME * 2, go_home = 0;
    start_time = time(0);
    while (program_launched)
    {
        // day & nigth management
        if (DAY_LENGTH >= (time(0) - start_time) % day_plus_night)
        { // day
            transition = 1.0;
            go_home = 0;
        }
        else if ((time(0) - start_time) % day_plus_night <= DAY_LENGTH + TRANSITION_TIME)
        { // switch to night
            transition -= 1 / (float)(TRANSITION_TIME * FRAMES_PER_SECOND);
            go_home = 1;
        }
        else if ((time(0) - start_time) % day_plus_night <= DAY_LENGTH * 2 + TRANSITION_TIME)
        { // night
            transition = 0.0;
            go_home = 1;
        }
        else if ((time(0) - start_time) % day_plus_night <= DAY_LENGTH * 2 + TRANSITION_TIME * 2)
        { // switch to day
            transition += 1 / (float)(TRANSITION_TIME * FRAMES_PER_SECOND);
            go_home = 0;
        }

        oldtime = SDL_GetTicks();
        for (int i = 0; i < PARTICLES_NUMBER; i++)
            move_particle(&part[i], transition, go_home);

        background(r, transition * 200 + 25, transition * 200 + 25, transition * 200 + 25, WIDTH, HEIGHT);
        display_box(r);
        for (int i = 0; i < PARTICLES_NUMBER; i++)
            draw_particle(r, part[i]);

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
        a = newtime;
        newtime = SDL_GetTicks();
        dt = newtime - a;
        calcul_time = (newtime - oldtime);
        real_fps = 1000 / dt;
        iterations++;
        if (1000 / FRAMES_PER_SECOND > calcul_time)
        {
            printf("fps = %.1lf\titerations = %d\tcalcul-time = %.1lf\t program has been running for %d sec\n", real_fps, iterations, calcul_time, time(0) - start_time);
            SDL_Delay(1000 / FRAMES_PER_SECOND - calcul_time); //);
        }
        else
            printf("max calculation power reached ! fps = %.1lf\titerations = %d\tcalcul-time = %.1lf\t program has been running for %d sec\n", real_fps, iterations, calcul_time, time(0) - start_time);
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
    color(r, 255, 128, 128, 1);
    line(r, WIDTH / 10, HEIGHT / 10, WIDTH / 10, HEIGHT * 0.9);
    line(r, WIDTH / 10 - 1, HEIGHT / 10 - 1, WIDTH / 10 - 1, HEIGHT * 0.9 + 1);
    line(r, WIDTH / 10, HEIGHT / 10, WIDTH * 0.9, HEIGHT / 10);
    line(r, WIDTH / 10 - 1, HEIGHT / 10 - 1, WIDTH * 0.9 + 1, HEIGHT / 10 - 1);
    line(r, WIDTH / 10, HEIGHT * 0.9, WIDTH * 0.9, HEIGHT * 0.9);
    line(r, WIDTH / 10 - 1, HEIGHT * 0.9 + 1, WIDTH * 0.9 + 1, HEIGHT * 0.9 + 1);
    line(r, WIDTH * 0.9, HEIGHT * 0.9, WIDTH * 0.9, HEIGHT * 0.1);
    line(r, WIDTH * 0.9 + 1, HEIGHT * 0.9 + 1, WIDTH * 0.9 + 1, HEIGHT * 0.1 - 1);
    color(r, 0, 0, 128, 0.5);
    roundRect(r, HOUSE_X, HOUSE_Y, HOUSE_SIZE, HOUSE_SIZE, 1, 10, 10, 10, 10);
}

void draw_particle(SDL_Renderer *r, particle p)
{
    color(r, p.color.r, p.color.g, p.color.b, p.color.a);
    if (!p.alive)
        color(r, 0, 0, 0, 1);
    circle(r, p.x, p.y, p.size, 1);
}

int move_particle(particle *p, double animation, int home)
{
    if (!p->alive)
        return 0;
    SDL_Color black = {0, 0, 0, 0};
    if (p->time_to_go <= 0 || p->destination_distance <= 2)
    {
        p->angle = rand() * 2 * 3.1415 / RAND_MAX;
        p->destination_distance = rand() % (int)fmin(HEIGHT, WIDTH);
        p->time_to_go = rand() % 60;
    }
    if (home == 1)
    {
        setDestination(p, HOUSE_X + HOUSE_SIZE / 2.0, HOUSE_Y + HOUSE_SIZE / 2.0);
        p->time_to_go = 5;
        if (animation == 0.0 && (p->x + p->size < HOUSE_X || p->x - p->size > HOUSE_X + HOUSE_SIZE || p->y + p->size < HOUSE_Y || p->y - p->size > HOUSE_Y + HOUSE_SIZE))
        {
            p->alive = 0;
            return 0;
        }
    }

    p->x += p->speed * cos(p->angle);
    p->y += p->speed * sin(p->angle);
    p->destination_distance -= p->speed;
    p->time_to_go -= 1;

    //=================bounce================
    if (home == 0)
    {
        if (p->x + p->size >= WIDTH * 0.9 || p->x - p->size <= WIDTH * 0.1)
            p->angle -= 3.1415;
        if (p->y + p->size >= HEIGHT * 0.9 || p->y - p->size <= HEIGHT * 0.1)
            p->angle -= 3.1415;
    }

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
    p->speed = PARTICLE_SPEED;
    p->angle = rand() * 2 * 3.1415 / RAND_MAX;
    p->destination_distance = rand() * fmin(fabs(p->y - HEIGHT), fabs(p->x - WIDTH)) / RAND_MAX;
    p->time_to_go = rand() % 60;
    p->x = WIDTH * 0.2 + rand() % WIDTH * 0.6;
    p->y = HEIGHT * 0.2 + rand() % HEIGHT * 0.6;
    p->size = 20;
    int col = rand() % NB_COLORS;
    p->color.r = c[col].r;
    p->color.g = c[col].g;
    p->color.b = c[col].b;
    p->color.a = c[col].a;
    p->round = 0;
    p->alive = 1;
    return 1;
}

void inherit_particle(particle *source, particle *target)
{
}

void copy_particle(particle *source, particle *target)
{
    target->x = source->x;
    target->y = source->y;
    target->size = source->size;
    target->color.r = source->color.r;
    target->color.g = source->color.g;
    target->color.b = source->color.b;
    target->color.a = source->color.a;
    target->round = source->round;
    target->speed = source->speed;
    target->destination_distance = source->destination_distance;
    target->time_to_go = source->time_to_go;
    target->angle = source->angle;
}

void setDestination(particle *p, int x, int y)
{
    p->destination_distance = dist(p->x, p->y, x, y);
    p->angle = atan2(y - p->y, x - p->x);
}