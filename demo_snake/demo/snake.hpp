#include<bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include<SDL_mixer.h>
#include <deque>
using namespace std;

class Snake
{
public:
    Snake();
    ~Snake();
    int exec();
    int playGame();
    bool tick();
    void draw();
    Mix_Chunk* blip;
    int const HeadOpenMouth = 0;
    int const Tail = 1;
    int const Turn = 2;
    int const Straight = 3;
    int const Head = 4;
    int const Fruit = 5;
    int const Width = 1280;
    int const Height = 704;
    int score = 0;

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *snake;
    SDL_Surface* scores;

    deque<pair<int, int> > partList;
    unsigned ticks = 0;
    int dx = 1;
    int dy = 0;
    int fruitX;
    int fruitY;
    void generateFruit();
    void renderScore();
    void gameOver();
};




















