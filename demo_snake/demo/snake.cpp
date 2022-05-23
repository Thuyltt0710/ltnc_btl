#include "snake.hpp"
using namespace std;
Snake::Snake()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(Width, Height, SDL_WINDOW_BORDERLESS, &window, &renderer);
    SDL_SetWindowPosition(window, 50, 100);
    snake= SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("snake.bmp"));
    SDL_FreeSurface(SDL_LoadBMP("snake3.bmp"));
    TTF_Init();
    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096);
    blip = Mix_LoadWAV("Blip.wav");
    partList.push_back(make_pair(5, 5));
    partList.push_back(make_pair(5, 6));
    partList.push_back(make_pair(4, 6));
}



//
void Snake::generateFruit() //random vị trí quả táo mới
{
    bool done = false;
    do
    {
        fruitX = rand() % ((Width) / 64);
        fruitY = rand() % ((Height) / 64);
        done = true;
        for (const auto &part: partList) //kiểm tra vị trí quả táo
        {
            if ((fruitX == part.first && fruitY == part.second) || (fruitX*64>=500 && fruitX*64<=780 && fruitY*64>=0 && fruitY*64<=175))
            {
                done = false;
                break;
            }
        }
    }
    while (!done);
}
void Snake :: renderScore()
{
    TTF_Font* font =  TTF_OpenFont((char*)"COOLVETI.ttf", 100);
    if (font == NULL)
    {
        cout << "Font loading error" << endl;
        return;
    }

    SDL_Surface* scores = TTF_RenderText_Solid(font, (string("Score: ") + to_string(score* 10)).c_str(), {102, 102, 0 });
    SDL_Texture* scoreMessage = SDL_CreateTextureFromSurface(renderer, scores);
    SDL_Rect scoreRect;
    scoreRect.w = 180;
    scoreRect.h = 100;
    scoreRect.x = 550;
    scoreRect.y = 25;
    SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);
    TTF_CloseFont(font);
    SDL_DestroyTexture(scoreMessage);
    SDL_FreeSurface(scores);
}

//
void Snake :: gameOver()
{
    TTF_Font* font =  TTF_OpenFont((char*)"COOLVETI.ttf", 100);
    if (font == NULL)
    {
        cout << "Font loading error" << endl;
        return;
    }
    cout<< "Game Over\n";
    SDL_Surface* gameOver = TTF_RenderText_Solid(font, "Game Over!!!", {102, 102, 0 });
    SDL_Texture* gameOverMessage = SDL_CreateTextureFromSurface(renderer, gameOver);
    SDL_Rect overRect;
    overRect.w = 380;
    overRect.h = 350;
    overRect.x = 450;
    overRect.y = 200;
    SDL_RenderCopy(renderer, gameOverMessage, NULL, &overRect);
    SDL_RenderPresent(renderer);
    TTF_CloseFont(font);
    SDL_DestroyTexture(gameOverMessage);
    SDL_FreeSurface(gameOver);
}


Snake::~Snake()
{

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(snake);
    SDL_FreeSurface(scores);
    Mix_FreeChunk(blip);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

int Snake::playGame()
{
    SDL_RenderClear(renderer);
    TTF_Font* font =  TTF_OpenFont((char*)"COOLVETI.ttf", 100);
    if (font == NULL)
    {
        cout << "Font loading error" << endl;
        return 1;
    }
    SDL_Surface* stop = TTF_RenderText_Solid(font, "Press Space To Play Game!!!", {102, 102, 0 });
    SDL_Texture* stopMessage = SDL_CreateTextureFromSurface(renderer, stop);
    SDL_Rect playRect;
    playRect.w = 480;
    playRect.h = 200;
    playRect.x = 400;
    playRect.y = 310;
    SDL_RenderCopy(renderer, stopMessage, NULL, &playRect);
    TTF_CloseFont(font);
    SDL_DestroyTexture(stopMessage);
    SDL_FreeSurface(stop);
    return 1;
}


int Snake::exec()
{
    int oldTick = SDL_GetTicks();
    bool isDie = false, isPause = true;
    for (auto done = false; !done;)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e) && !isDie)
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    if (dx != 0)
                    {
                        dx = 0;
                        dy = -1;
                    }
                    break;
                case SDLK_RIGHT:
                    if (dy != 0 || dx!=-1)
                    {
                        dx = 1;
                        dy = 0;
                    }
                    break;
                case SDLK_DOWN:
                    if (dx !=0)
                    {
                        dx = 0;
                        dy = 1;
                    }
                    break;
                case SDLK_LEFT:
                    if (dy != 0 || dx != 1)
                    {
                        dx = -1;
                        dy = 0;
                    }
                    break;
                }
                case SDLK_SPACE:
                    if(isPause)
                    {
                        isPause = false;
                        isDie = false;
                        generateFruit();
                    }
                }
                break;
            case SDL_QUIT:
                done = true;
                break;
            }
        }
        if(isDie)
        {
            isPause = true;
            isDie = false;
            partList.clear();
            partList.push_back(make_pair(5, 5)); //đầu
            partList.push_back(make_pair(5, 6)); //thân
            partList.push_back(make_pair(4, 6)); //đuôi
            dx = 1;
            dy = 0;
            score = 0;
        }

        int currentTick = SDL_GetTicks();
        for (int t = oldTick; t < currentTick; ++t)
        {
            if(!isPause)
            {
                if (!tick())
                {
                    isDie = true;
                }
            }
        }
        oldTick = currentTick;
        SDL_SetRenderDrawColor(renderer, 255, 255, 224, 0xff);
        SDL_RenderClear(renderer);
        if(isPause)
        {
            playGame();
        }
        renderScore();
        if(isDie)
        {
            gameOver();
            fruitX = -1; //xoá quả táo cũ
            fruitY = -1;
            SDL_Delay(1000);
        }
        draw();
        SDL_RenderPresent(renderer);
    }
    return 0;
}

bool Snake::tick()
{
    if (ticks++ % 300 == 0)
    {
        //di chuyen tinh tien
        auto p = partList.front();
        p.first += dx;
        p.second += dy;
        if (p.first < 0 || p.first >= Width / 64 ||
                p.second < 0 || p.second >= (Height) / 64)
        {
            return false; //chạm vào tường
        }

        for (const auto &part: partList)
            if (p == part)
            {
                return false; //tự cắn chính mình
            }

        partList.push_front(p);
        if (p.first != fruitX || p.second != fruitY)
            partList.pop_back(); //xoá đi đuôi
        else
        {
            generateFruit();
            Mix_PlayChannel(-1,blip,0);
            score++;
            cout << score << endl;
        } //ăn quả táo, thì dài thêm 1 ô

    }
    return true;
}

void Snake::draw()
{

    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = 64;
    src.h = 64;

    SDL_Rect dest;
    dest.w = 64;
    dest.h = 64;
    // 4 truong hop gap khuc
    int ds[4][3] =
    {
        { -1, 0, 0 },
        { 0, -1, 90 },
        { 1, 0, 180 },
        { 0, 1, -90 },
    };
    for (auto iter = begin(partList); iter != end(partList); ++iter)
    {
        float direction = 0;
        const auto  &part = *iter;
        if (iter == begin(partList)) //head
        {
            if (iter->first + dx == fruitX && iter->second + dy == fruitY) //quả táo ở trước mặt
            {
                src.x = HeadOpenMouth * 64;
            }
            else
                src.x = Head * 64;
            if (iter + 1 != end(partList)) //body
            {
                const auto &nextPart = *(iter + 1);
                for (int d=0; d<=3; d++)
                {
                    if (part.first + ds[d][0] == nextPart.first && part.second + ds[d][1] == nextPart.second)
                    {
                        direction = ds[d][2];
                        break;
                    }
                }
            }
        }
        else if (iter + 1 == end(partList)) //tail
        {
            src.x = Tail * 64;
            const auto &prevPart = *(iter - 1);
            for (int d=0; d<=3; d++)
            {
                if (part.first == prevPart.first + ds[d][0] && part.second == prevPart.second + ds[d][1])
                {
                    direction = ds[d][2];
                    break;
                }
            }
        }
        else
        {
            const auto &nextPart = *(iter + 1);
            const auto &prevPart = *(iter - 1);
            if (nextPart.first == prevPart.first)
            {
                src.x = Straight * 64;
                direction = 90;
            }
            else if (nextPart.second == prevPart.second)
            {
                src.x = Straight * 64;
                direction = 0;
            }
            else
            {
                src.x = Turn * 64;
                bool up = false;
                if (part.first == nextPart.first && part.second - 1 == nextPart.second)
                    up = true;
                if (part.first == prevPart.first && part.second - 1 == prevPart.second)
                    up = true;
                bool right = false;
                if (part.first + 1 == nextPart.first && part.second == nextPart.second)
                    right = true;
                if (part.first + 1 == prevPart.first && part.second == prevPart.second)
                    right = true;
                bool down = false;
                if (part.first == nextPart.first && part.second + 1 == nextPart.second)
                    down = true;
                if (part.first == prevPart.first && part.second + 1 == prevPart.second)
                    down = true;
                bool left = false;
                if (part.first - 1 == nextPart.first && part.second == nextPart.second)
                    left = true;
                if (part.first - 1 == prevPart.first && part.second == prevPart.second)
                    left = true;
                if (up && right)
                    direction = 0;
                else if (right && down)
                    direction = 90;
                else if (down && left)
                    direction = 180;
                else if (left && up)
                    direction = 270;
            }
        }

        dest.x = 64 * part.first;
        dest.y = 64 * part.second;
        SDL_RenderCopyEx(renderer, snake, &src, &dest, direction, nullptr, SDL_FLIP_NONE);
    }
    src.x = Fruit * 64;
    dest.x = fruitX * 64;
    dest.y = fruitY * 64;
    SDL_RenderCopyEx(renderer, snake, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
}
