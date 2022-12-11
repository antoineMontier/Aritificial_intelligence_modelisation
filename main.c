#include "SDL_Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define FRAMES_PER_SECOND 24
#define PARTICLES_NUMBER 50
#define NB_COLORS 6
#define ADN_TRANSMISSION 0.5
#define AUTO_TRANSMISSION 0.1
#define PARTICLE_SPEED 2
#define DAY_LENGTH 10
#define TRANSITION_TIME 2
#define HOUSE_SIZE 70
#define HOUSE_X (WIDTH*0.25 - HOUSE_SIZE/2)
#define HOUSE_Y (HEIGHT*0.75 - HOUSE_SIZE/2)
#define DIED_TIME 30
#define PARTICLE_SIZE 10
#define PARTICLE_VISION (10.0)
#define FOOD_NUMBER 50
#define FOOD_SIZE 5

typedef struct
{
    int id;
    double vision_field;
    int food;
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

typedef struct{
    double size;
    double x;
    double y;
    SDL_Color color;
    int particle;//-1 if not eaten, else the index of the particle in the array
}food;



void display_box(SDL_Renderer *r);
void draw_particle(SDL_Renderer *r, particle p);
int update_particle(particle *p, double animation, int home, food*array_f); // returns 0 if particle in the box and 1 if outside
void display_informations(SDL_Renderer *r, TTF_Font *f, particle *p, char *tmp, int timer, double transition, double fps);
int initialise_particle(particle *p, SDL_Color *c, int i);
void inherit_particle(particle *source, particle *target);
void copy_particle(particle *source, particle *target);
void close_Particle(particle **p);
void draw_path(SDL_Renderer *r, SDL_Point *p, int bestmove);
void setDestination(particle *p, int x, int y);
int initialise_food(food*f, SDL_Color *c);
void update_food(particle *array_p, food*f, double animation, int home, SDL_Color *c);


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
        initialise_particle(&part[i], colors, i);

    food *foods = malloc(FOOD_NUMBER * sizeof(food));
    for(int i = 0; i < FOOD_NUMBER; i++)
        initialise_food(&foods[i], colors);

    int iterations = 0, start_time = 0, go_home = 0;
    start_time = time(0);
    while (program_launched)
    {
        // day & nigth management
        if ((time(0) - start_time) % (DAY_LENGTH * 2 + TRANSITION_TIME*2) <= DAY_LENGTH)
        { // day
            transition = 1.0;
            go_home = 0;
        }
        else if ((time(0) - start_time) % (DAY_LENGTH * 2 + TRANSITION_TIME*2) <= DAY_LENGTH + TRANSITION_TIME)
        { // switch to night
            transition -= 1 / (float)(TRANSITION_TIME * FRAMES_PER_SECOND);
            go_home = 1;
        }
        else if ((time(0) - start_time) % (DAY_LENGTH * 2 + TRANSITION_TIME*2) <= DAY_LENGTH * 2 + TRANSITION_TIME)
        { // night
            transition = 0.0;
            go_home = 1;
        }
        else if ((time(0) - start_time) % (DAY_LENGTH * 2 + TRANSITION_TIME*2) <= DAY_LENGTH * 2 + TRANSITION_TIME * 2)
        { // switch to day
            transition += 1 / (float)(TRANSITION_TIME * FRAMES_PER_SECOND);
            go_home = 0;
        }

        oldtime = SDL_GetTicks();






        for (int i = 0; i < PARTICLES_NUMBER; i++)
            update_particle(&part[i], transition, go_home, foods);
        for(int i = 0; i < FOOD_NUMBER; i++)
            update_food(part, &foods[i], transition, go_home, colors);

        background(r, transition * 200 + 25, transition * 200 + 25, transition * 200 + 25, WIDTH, HEIGHT);
        display_informations(r, param_font, part, tmp, start_time, transition, real_fps);
        display_box(r);
        for (int i = 0; i < PARTICLES_NUMBER; i++)
            draw_particle(r, part[i]);
        for(int i = 0 ; i < FOOD_NUMBER ; i++)
            draw_food(r, foods[i]);













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
            case SDL_MOUSEBUTTONDOWN:
                if(rollover(evt.button.x, evt.button.y, WIDTH*0.015, HEIGHT*0.09, WIDTH*0.07, HEIGHT*0.07)){
                    //respawn
                    for(int i = 0; i < PARTICLES_NUMBER; i++)
                        if(part[i].alive != 1)
                            initialise_particle(&part[i], colors, -1);
                }
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
            printf("iterations = %d\tcalcul-time = %.1lf\t program has been running for %d sec\n", real_fps, iterations, calcul_time, time(0) - start_time);
            SDL_Delay(1000 / FRAMES_PER_SECOND - calcul_time); //);
        }
        else
            printf("max calculation power reached ! iterations = %d\tcalcul-time = %.1lf\t program has been running for %d sec\n", real_fps, iterations, calcul_time, time(0) - start_time);
        SDL_RenderPresent(r); // refresh the render
    }
    free(foods);
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
    if(p.alive < -DIED_TIME)
        return;
    color(r, p.color.r, p.color.g, p.color.b, p.color.a);
    if (p.alive <= 0)
        color(r, 0, 0, 0, 1);
    circle(r, p.x, p.y, p.size, 1);
    if(p.food == 1){
        color(r, 0, 0, 0, 1);
        circle(r, p.x, p.y, p.size/3, 1);
    }
}

void draw_food(SDL_Renderer *r, food f){
    color(r, f.color.r, f.color.g, f.color.b, f.color.a);
    circle(r, f.x, f.y, f.size, 1);
}


int update_particle(particle *p, double animation, int home, food*array_f)
{
    if(p->alive < -DIED_TIME)
        return 0;
    else if (p->alive <= 0){
        p->alive -= 1;
        p->size -= PARTICLE_SIZE/(float)DIED_TIME;//decrease size if died
        return 0;
    }

    if (home == 0 && animation <= 1/(double)TRANSITION_TIME){
        p->food = 0;
        p->destination_distance = 1;
        p->time_to_go = 1;
    }

    if(p->food == 0){
        for(int i = 0; i < FOOD_NUMBER; i++){
            if(array_f[i].particle == -1 && dist(p->x, p->y, array_f[i].x, array_f[i].y) <= p->vision_field){//food avaible and in vision_field
                //printf("hop %d", p->id);
                array_f[i].particle = p->id;
                p->food = 1;
            }
        }
    }

    if (p->time_to_go <= 0 || p->destination_distance <= 2)
    {
        p->angle = rand() * 2 * 3.1415 / RAND_MAX;
        p->destination_distance = rand() % (int)fmin(HEIGHT, WIDTH);
        p->time_to_go = rand() % 60;
    }
    if (home == 1 || p->food == 1)
    {
        setDestination(p, HOUSE_X + HOUSE_SIZE / 2.0, HOUSE_Y + HOUSE_SIZE / 2.0);
        p->time_to_go = 2;
        if (animation == 0.0){
            if(p->alive == 1 && ((p->x + p->size < HOUSE_X || p->x - p->size > HOUSE_X + HOUSE_SIZE || p->y + p->size < HOUSE_Y || p->y - p->size > HOUSE_Y + HOUSE_SIZE)||p->food == 0))
            {
                p->alive = 0;
                return 0;
            }
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

void update_food(particle *array_p, food*f, double animation, int home, SDL_Color *c){
    if (f->particle!= -1 && home == 0 && animation < 1){
        initialise_food(f, c);
        f->particle = -1;
    }
    if(animation > 0 && f->size < FOOD_SIZE)
        f->size += FOOD_SIZE/(double)TRANSITION_TIME;//reappear the food if day is comming
    if(home == 0 || animation > 0){//everything but not dark night
        if(f->particle != -1){
            f->x = array_p[f->particle].x;//food is on particle
            f->y = array_p[f->particle].y;
        }
    }else{//dark night
        if(f->particle != -1){
            if(array_p[f->particle].alive == 0){//drop food if particle is dead
                f->particle = -1;
            }else{
                f->x = array_p[f->particle].x;
                f->y = array_p[f->particle].y;
                f->size -= FOOD_SIZE/(double)TRANSITION_TIME;//decrease food size with time as if it was eaten
            }
        }
    }
    
}

void display_informations(SDL_Renderer *r, TTF_Font *f, particle *p, char *tmp, int timer, double transition, double fps)
{
    //day and night bar :
        text(r, WIDTH*0.004, HEIGHT*0.002, "time", f, 255, 128, 0);

        //ending grey part
        color(r, 128, 128, 128, 1);
        roundRect(r, WIDTH*0.05 + WIDTH*0.24*(DAY_LENGTH*2 + TRANSITION_TIME)/(DAY_LENGTH * 2.0 + TRANSITION_TIME * 2.0), HEIGHT*0.01, WIDTH*0.24*TRANSITION_TIME/(DAY_LENGTH * 2.0 + TRANSITION_TIME * 2.0), 10, 1, 5, 5, 5, 5);
        
        //1 black part
        color(r, 50, 50, 50, 1);
        roundRect(r, WIDTH*0.05 + WIDTH*0.24*(DAY_LENGTH+TRANSITION_TIME)/(DAY_LENGTH * 2.0 + TRANSITION_TIME * 2.0) - 5, HEIGHT*0.01, WIDTH*0.24*DAY_LENGTH/(DAY_LENGTH * 2.0 + TRANSITION_TIME * 2.0) + 5*2, 10, 1, 0, 0, 0, 0);
        //1 white part (useless)
        color(r, 200, 200, 200, 1);
        roundRect(r, WIDTH*0.05 , HEIGHT*0.01, WIDTH*0.24*DAY_LENGTH/(DAY_LENGTH * 2.0 + TRANSITION_TIME * 2.0) + 5*2, 10, 1, 5, 0, 5, 0);

        //middle grey part
        color(r, 128, 128, 128, 1);
        roundRect(r, WIDTH*0.05 + WIDTH*0.24*DAY_LENGTH/(DAY_LENGTH * 2.0 + TRANSITION_TIME * 2.0), HEIGHT*0.01, WIDTH*0.24*TRANSITION_TIME/(DAY_LENGTH * 2.0 + TRANSITION_TIME * 2.0), 10, 1, 0, 0, 0, 0);


        //progression bar
        color(r, 255, 128, 0, 1);
        roundRect(r, WIDTH*0.05, HEIGHT*0.01, WIDTH*0.24*((time(0) - timer) % (DAY_LENGTH * 2 + TRANSITION_TIME * 2))/(DAY_LENGTH * 2.0 + TRANSITION_TIME * 2.0), 10, 1, 5, 5, 5, 5);
        
        //border
        color(r, (1-transition) * 255, (1-transition) * 255, (1-transition) * 255, 1);
        roundRect(r, WIDTH*0.05, HEIGHT*0.01, WIDTH*0.24, 10, 0, 5, 5, 5, 5);

    //alive bar
        text(r, WIDTH*0.004, HEIGHT*0.04, "alive", f, 0, 255, 48);
        //first let's count the number of alive particle
        int alive = 0;
        for (int i = 0; i < PARTICLES_NUMBER ; i++)
            if(p[i].alive == 1)
                alive++;
        gcvt(alive, 4, tmp);
        text(r, WIDTH*0.26, HEIGHT*0.042, tmp, f, 0, 255, 48);
        //filling bar
        color(r, 0, 255, 48, 1);
        roundRect(r, WIDTH*0.05, HEIGHT*0.047, WIDTH*0.2*(alive/(float)PARTICLES_NUMBER), 10, 1, 5, 5, 5, 5);
        //border
        color(r, (1-transition) * 255, (1-transition) * 255, (1-transition) * 255, 1);
        roundRect(r, WIDTH*0.05, HEIGHT*0.047, WIDTH*0.2, 10, 0, 5, 5, 5, 5);
    //FPS
        text(r, WIDTH*0.337, HEIGHT*0.075, "FPS", f, 255, 0, 0);
        gcvt(fps, 3, tmp);
        text(r, WIDTH*0.337, HEIGHT*0.045, tmp, f, 255, 0, 0);//actual
        gcvt(FRAMES_PER_SECOND*2, 3, tmp);
        text(r, WIDTH*0.355, 0, tmp, f, 255, 0, 0);//max
        text(r, WIDTH*0.285, HEIGHT*0.074, "0", f, 255, 0, 0);//0


        //filling bar
        color(r, 255, 0, 0, 1);
        roundRect(r, WIDTH*0.32,HEIGHT*0.094 - HEIGHT*0.09*fps/(2*FRAMES_PER_SECOND), 10, HEIGHT*0.09*fps/(2*FRAMES_PER_SECOND) , 1, 5, 5, 5, 5);
        //border
        color(r, (1-transition) * 255, (1-transition) * 255, (1-transition) * 255, 1);
        roundRect(r, WIDTH*0.32, HEIGHT*0.0035, 10, HEIGHT*0.09, 0, 5, 5, 5, 5);
        line(r, WIDTH*0.32, HEIGHT*0.009,WIDTH*0.35, HEIGHT*0.009);
        line(r, WIDTH*0.32+10, HEIGHT*0.087, WIDTH*0.3, HEIGHT*0.087);
    //respawn button
        color(r, 50, 50, 178, 0.5);
        roundRect(r, WIDTH*0.015, HEIGHT*0.09, WIDTH*0.07, HEIGHT*0.07, 1, 5, 5, 5, 5);
        text(r, WIDTH*0.042, HEIGHT*0.097, "re", f, (transition) * 255, (transition) * 255, (transition) * 255);
        text(r, WIDTH*0.021, HEIGHT*0.125, "spawn", f, (transition) * 255, (transition) * 255, (transition) * 255);


}

int initialise_particle(particle *p, SDL_Color *c, int i)
{
    p->vision_field = PARTICLE_VISION;
    p->food = 0; 
    p->speed = PARTICLE_SPEED;
    p->angle = rand() * 2 * 3.1415 / RAND_MAX;
    p->destination_distance = rand() * fmin(fabs(p->y - HEIGHT), fabs(p->x - WIDTH)) / RAND_MAX;
    p->time_to_go = rand() % 60;
    p->x = WIDTH * 0.2 + rand() % WIDTH * 0.6;
    p->y = HEIGHT * 0.2 + rand() % HEIGHT * 0.6;
    p->size = PARTICLE_SIZE;
    int col = 5; //rand() % NB_COLORS;
    p->color.r = c[col].r;
    p->color.g = c[col].g;
    p->color.b = c[col].b;
    p->color.a = c[col].a;
    p->round = 0;
    p->alive = 1;
    if(i != -1)
        p->id = i;
    return 1;
}

int initialise_food(food*f, SDL_Color *c){
    do{
        f->x = WIDTH * 0.11 + rand() % WIDTH * 0.78;
        f->y = HEIGHT * 0.11 + rand() % HEIGHT * 0.78;
    }while(rollover(f->x, f->y, HOUSE_X - 10, HOUSE_Y - 10, HOUSE_SIZE + 20, HOUSE_SIZE + 20));
    f->particle = -1;
    f->color = c[0];
    f->size = FOOD_SIZE;
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
    target->vision_field = source->vision_field;
    target->food = source->food;
}

void setDestination(particle *p, int x, int y)
{
    p->destination_distance = dist(p->x, p->y, x, y);
    p->angle = atan2(y - p->y, x - p->x);
}