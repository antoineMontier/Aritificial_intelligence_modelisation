#include "SDL_Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define FRAMES_PER_SECOND 60
#define PARTICLES_NUMBER 200
#define NB_COLORS 3
#define ADN_TRANSMISSION 0.5
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
    int* seed;
    SDL_Color color;
} particle;

void display_box(SDL_Renderer *r);
void draw_particle(SDL_Renderer *r, particle p);
int move_particle(particle *p); // returns 0 if particle in the box and 1 if outside
void display_particle_informations(SDL_Renderer *r, TTF_Font *f, particle p, char *tmp, const char *position);
int initialise_particle(particle *p, SDL_Color *c);
void inherit_particle(particle *source, particle *target);
void copy_particle(particle *source, particle *target);
void close_Particle(particle **p);

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
    SDL_Color *colors = malloc(NB_COLORS * sizeof(SDL_Color));
    colors[0] = red;
    colors[1] = green;
    colors[2] = blue;
    char *tmp = malloc(10);
    particle* part = malloc(PARTICLES_NUMBER*sizeof(particle));
    particle temp;
    initialise_particle(&temp, colors);
    for(int i=0; i< PARTICLES_NUMBER; i++)
        initialise_particle(&part[i], colors);

    int iterations = 0;

    while (program_launched)
    {
        printf("%d\n", iterations);
        for(int i = 0; i < PARTICLES_NUMBER ; i++){
            if(move_particle(&part[i]) == 0){
                printf("regeneration\n");
                copy_particle(&part[i], &temp);
                for(int j = 0; j < PARTICLES_NUMBER ; j++)
                    inherit_particle(&temp, &part[j]);
                iterations = 0;
                i = PARTICLES_NUMBER;
            }
        }


        background(r, 255, 255, 255, WIDTH, HEIGHT);
        for(int i = 0; i <  PARTICLES_NUMBER ; i++)
            draw_particle(r, part[i]);
        // printf("%d\n", iterations);
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
        iterations++;
        SDL_RenderPresent(r); // refresh the render
        SDL_Delay(1000 / FRAMES_PER_SECOND);
    }
    close_Particle(&part);
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

    if(p->round >= COMMAND_NUMBER){
        p->round = 0;
        return -1;
    }

    //============================== bounce =============================
    if (p->y - p->size < HEIGHT * 0.1)
        p->y = p->size + HEIGHT * 0.1;
    else if (p->y + p->size > HEIGHT * 0.9)
        p->y = HEIGHT * 0.9 - p->size;
    if (p->x - p->size < WIDTH * 0.1)
        p->x = WIDTH * 0.1 + p->size;
    else if ((p->y - p->size < HEIGHT * 0.45 || p->y + p->size > HEIGHT * 0.55) && p->x + p->size > WIDTH * 0.9)
        p->x = WIDTH * 0.9 - p->size;
    p->p_x = p->x;
    p->p_y = p->y;
    //==================================================================
    switch (p->seed[p->round])
    {
    case 1:
        p->y -= PARTICLE_SPEED;
        //printf("up\n");
        break;
    case 2:
        p->y += PARTICLE_SPEED;
        //printf("down\n");
        break;
    case 3:
        p->x -= PARTICLE_SPEED;
        //printf("left\n");
        break;
    case 4:
        p->x += PARTICLE_SPEED;
        //printf("right\n");
        break;
    
    default:
        break;
    }
    p->round += 1;
    if (p->x + p->size > WIDTH * 0.1 - 5 && p->y + p->size > HEIGHT * 0.1 - 5 && p->x - p->size < WIDTH * 0.9 + 5 && p->y - p->size < HEIGHT * 0.9 + 5)
    {
        return 1;
    }
    return 0;
}

void display_particle_informations(SDL_Renderer *r, TTF_Font *f, particle p, char *tmp, const char *position)
{
    if (strcmp(position, "up") == 0)
    {
        text(r, WIDTH * 0, HEIGHT * 0, "x", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.1, HEIGHT * 0, "y", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.2, HEIGHT * 0, "seed x", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.3, HEIGHT * 0, "seed y", f, p.color.r, p.color.g, p.color.b);

        gcvt(p.x, 3, tmp);
        text(r, WIDTH * 0, HEIGHT * 0.02, tmp, f, p.color.r, p.color.g, p.color.b);
        gcvt(p.y, 3, tmp);
        text(r, WIDTH * 0.1, HEIGHT * 0.02, tmp, f, p.color.r, p.color.g, p.color.b);

    }
    else if (strcmp(position, "down") == 0)
    {

        text(r, WIDTH * 0, HEIGHT * 0.95, "x", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.1, HEIGHT * 0.95, "y", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.2, HEIGHT * 0.95, "seed x", f, p.color.r, p.color.g, p.color.b);
        text(r, WIDTH * 0.3, HEIGHT * 0.95, "seed y", f, p.color.r, p.color.g, p.color.b);

        gcvt(p.x, 3, tmp);
        text(r, WIDTH * 0, HEIGHT * 0.97, tmp, f, p.color.r, p.color.g, p.color.b);
        gcvt(p.y, 3, tmp);
        text(r, WIDTH * 0.1, HEIGHT * 0.97, tmp, f, p.color.r, p.color.g, p.color.b);

    }
}

int initialise_particle(particle *p, SDL_Color *c)
{
    p->seed = malloc(COMMAND_NUMBER*sizeof(int));
    for (int i = 0; i < COMMAND_NUMBER; i++)
        p->seed[i] = rand() % 5;
    p->x = WIDTH / 2;
    p->y = HEIGHT / 2;
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
    for(int i = 0; i < COMMAND_NUMBER ; i++)
        if(rand()/(float)RAND_MAX < ADN_TRANSMISSION)//transmission
            target->seed[i] = source->seed[i];
        else//keep the command
            target->seed[i] = target->seed[i];//=nothing
    target->round = 0;
    target->x = WIDTH / 2;
    target->y = HEIGHT / 2;
    target->size = 20;
}

void copy_particle(particle *source, particle *target)
{   
    for(int i = 0; i < COMMAND_NUMBER ; i++)
        target->seed[i] = source->seed[i];
    target->x = source->x;
    target->y = source->y;
    target->size = source->size;
    target->color.r = source->color.r;
    target->color.g = source->color.g;
    target->color.b = source->color.b;
    target->color.a = source->color.a;
    target->round = source->round;
}


void close_Particle(particle **p){
    for(int i = 0; i < PARTICLES_NUMBER ; i++){
        if((p)[i]->seed == NULL)
            printf("line %d hasn't been destroyed yet\n", i);
        else
            free(p[i]->seed);
    }
    if(*p == NULL)
        printf("particle array hasn't been destroyed yet\n");
    else
        free(*p);
}