#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <math.h>

#define WIDTH 1000
#define HEIGHT 1000

void SDL_ExitWithError(const char *string);
void mark(SDL_Renderer* r, int x, int y, int thickness);
void point(SDL_Renderer* r, int x, int y);
void line(SDL_Renderer* r, int x1, int y1, int x2, int y2);
void color(SDL_Renderer* r, int red, int green, int blue, int alpha);
void rect(SDL_Renderer* r, int x, int y, int width, int height, int filled);
void circle(SDL_Renderer * r, int cx, int cy, int radius, int filled);
void openSDL(int x, int y, int mode, SDL_Window**w, SDL_Renderer**r);
void closeSDL(SDL_Window**w, SDL_Renderer**r);
void background(SDL_Renderer* r, int red, int green, int blue, int w, int h);
int rollover(int mx, int my, int x, int y, int w, int h);
int inTheTriangle(double x1, double y1, double x2, double y2, double x3, double y3, double a, double b);
double min(double a, double b, double c);
double max(double a, double b, double c);
void triangle(SDL_Renderer* r, int x1, int y1, int x2, int y2, int x3, int y3, int filled);
void roundRect(SDL_Renderer* r, int x, int y, int width, int height, int filled, int topleft, int topright, int downleft, int downright);
void setFont(TTF_Font**font, char*font_file, int size);
void text(SDL_Renderer*r, int x, int y, char*text, TTF_Font*font, int red, int green, int blue);
void toChar(char*c, int n);
double dist(double x1, double y1, double x2, double y2);