#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <deque>
#include <algorithm>
#include <utility>
#include <set>

#define SIZE 25
#define TICKS 1000
#define SCREEN_WIDTH SIZE * SIZE
#define SCREEN_HEIGHT SIZE * SIZE
#define APPLE_COUNT 20
#define st first
#define nd second

int snakeSize = 1;
int act_apples;
int movesLeft = 0;
int act_index;

int vis[SIZE][SIZE];

int dx = SIZE / APPLE_COUNT;
int dx_mod = SIZE % APPLE_COUNT;

struct Snake {
    int x, y;
    Snake(int x1 = 0, int y1 = 0) {
        x = x1;
        y = y1;
    }
};
enum SnakeDirection {
    UP, DOWN, LEFT, RIGHT 
} snakeDirection;

std::pair<int, int> par[SIZE][SIZE];
int turns[SIZE][SIZE];

int dist[SIZE][SIZE];
int path[SIZE * SIZE];


std::vector < sf::RectangleShape > apples_rects;
std::vector < struct Apple > apples_structs;
sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML Snake");
std::vector < std::pair< Snake, int > > got;


SnakeDirection direction = SnakeDirection::RIGHT;
int snakeSpeed = 1;

std::deque < Snake > wonsz;

struct Apple {
    int x, y;
    Apple(int x1 = 0, int y1 = 0) {
        x = x1;
        y = y1;
    }
} apple;

void normalize(Snake& s1) {
    if (s1.x < 0) {
        s1.x = SIZE - 1;
    }
    if (s1.y < 0) {
        s1.y = SIZE - 1;
    }
}

void change_coord(struct Snake s1, SnakeDirection direct, struct Snake& wyn) {

    wyn.x = s1.x;
    wyn.y = s1.y;

    switch (direct) {
    case SnakeDirection::UP:
        wyn.y--;
        break;
    case SnakeDirection::DOWN:
        wyn.y = (wyn.y + 1) % SIZE;
        break;
    case SnakeDirection::LEFT:
        wyn.x--;
        break;
    case SnakeDirection::RIGHT:
        wyn.x = (wyn.x + 1) % SIZE;
        break;
    }
    normalize(wyn);
}


SnakeDirection convert(int k)
{
    SnakeDirection dir;
    switch (k) {
    case 0:
        dir = SnakeDirection::UP;
        break;
    case 1:
        dir = SnakeDirection::DOWN;
        break;
    case 2:
        dir = SnakeDirection::LEFT;
        break;
    case 3:
        dir = SnakeDirection::RIGHT;
        break;
    }
    return dir;
}


std::vector<std::pair< struct Snake, int>> go_to_nearest(struct Snake source) {
    int act_max = 0;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {

            vis[i][j] = 0;
            par[i][j].first = -1;
            par[i][j].second = -1;
            dist[i][j] = 0;
        }
    }

    for (int i = 0; i < APPLE_COUNT; i++) {
        Apple app = apples_structs[i];
        vis[app.x][app.y] = 2;
    }

    std::deque<Snake> wonsz_copy(wonsz.size());
    std::copy(wonsz.begin(), wonsz.end(), wonsz_copy.begin());

    std::deque<Snake> Q;
    Q.push_back(source);

    dist[source.x][source.y] = 0;
    par[source.x][source.y] = std::make_pair(-1, -1);
    vis[source.x][source.y] = 1;


    std::vector<std::pair<struct Snake, int>> res;
    while (Q.size()) {
        Snake X = Q.front();
        Q.pop_front();

        for (int i = 0; i < 4; i++) {

            SnakeDirection orient_i = convert(i);
            struct Snake temp_X;
            change_coord(X, orient_i, temp_X);

            int new_x = temp_X.x;
            int new_y = temp_X.y;
            int flag = vis[new_x][new_y];
            int dd = dist[X.x][X.y] + 1;

            bool found1 = false;
            int start = std::max(0, (int)wonsz_copy.size() - dd - 1);
            for (int j = start; j < wonsz_copy.size(); j++) {
                if (new_x == wonsz_copy[j].x && new_y == wonsz_copy[j].y) {
                    found1 = true;
                    break;
                }
            }

            if (!found1 && (flag == 2 || flag == 0))
            {
                par[new_x][new_y].st = X.x;
                par[new_x][new_y].nd = X.y;
                turns[new_x][new_y] = i;

                if (flag == 2){

                    res.push_back(std::make_pair(Snake(new_x, new_y), i));

                    int x1 = new_x;
                    int y1 = new_y;

                    while (par[x1][y1].st != -1 || par[x1][y1].nd != -1) {

                        int prev_x = x1; int prev_y = y1;
                        x1 = par[prev_x][prev_y].st;
                        y1 = par[prev_x][prev_y].nd;
                        res.push_back(std::make_pair(Snake(x1, y1), turns[x1][y1]));

                    }
                    return res;
                }
                else {
                    dist[new_x][new_y] = dist[X.x][X.y] + 1;
                    vis[new_x][new_y] = 1;
                    Q.push_back(Snake(new_x, new_y));

                }
            }
        }
    }
    return res;
}


void move() {

    struct Snake first = wonsz.front();
    wonsz.pop_back();

    if (movesLeft == 0)
    {
        got.clear();
        got = go_to_nearest(Snake(first.x, first.y));
        movesLeft = got.size() - 1;
        act_index = 0;
    }

    if (movesLeft > 0) {
        direction = convert(got[act_index].nd);
        act_index++;
        movesLeft--;
    }

    struct Snake buff;

    //wonsz.pop_back();

    change_coord(first, direction, buff);


    wonsz.push_front(buff);

    for (int i = 0; i < APPLE_COUNT; i++) {

        if ((wonsz[0].x == apples_structs[i].x) && (wonsz[0].y == apples_structs[i].y)) {

            struct Snake temp_;
            change_coord(buff, SnakeDirection::LEFT, temp_);

            wonsz.push_front(temp_);
            int mod = (i == APPLE_COUNT - 1) ? dx_mod : 0;
            struct Apple prev = apples_structs[i];

            int x1 = i * dx + mod + rand() % dx;
            int y1 = rand() % SIZE;

            srand(time(0));

            while (x1 == prev.x && y1 == prev.y) {
                x1 = i * dx + mod + rand() % dx;
                y1 = rand() % SIZE;
            }

            apples_structs[i] = struct Apple(x1, y1);
            apples_rects[i].setPosition(x1 * SIZE, y1 * SIZE);
            printf("act_len: %d\n", wonsz.size());
            snakeSpeed++;

           /* bool one_valid = false;

            for (int o = 3; o >= 0; o--) {

                SnakeDirection orient_i = convert(o);
                struct Snake temp;
                change_coord(buff, orient_i, temp);

                std::cout << o << ": " << temp.x << ' ' << temp.y << "\n";

                bool found_inter = false;

                for (int k = 1; k < wonsz.size(); k++) {
                    if (temp.x == wonsz[k].x && temp.y == wonsz[k].y) {
                        found_inter = true;
                        break;
                    }
                }
                
                if (!found_inter) {
                    std::cout << "EAT: " << temp.x << ' ' << temp.y << "\n";
                    wonsz.push_front(temp);
                    one_valid = true;
                    break;
                }
            }

            if (one_valid) {

                int mod = (i == APPLE_COUNT - 1) ? dx_mod : 0;
                struct Apple prev = apples_structs[i];

                int x1 = i * dx + mod + rand() % dx;
                int y1 = rand() % SIZE;

                srand(time(0));

                while (x1 == prev.x && y1 == prev.y) {
                    x1 = i * dx + mod + rand() % dx;
                    y1 = rand() % SIZE;
                }

                apples_structs[i] = struct Apple(x1, y1);
                apples_rects[i].setPosition(x1 * SIZE, y1 * SIZE);
                printf("act_len: %d\n", wonsz.size());
                snakeSpeed++;
                break;
            }
            else {

                snakeSpeed = 1;
                wonsz.clear();
                struct Snake pocz(10, 10);
                wonsz.push_front(pocz);
                direction = SnakeDirection::RIGHT;
                printf("Restart\n");
                break;
            }
            */
        }
    }

    for (int k = 1; k < wonsz.size(); k++) {
        if (wonsz[0].x == wonsz[k].x && wonsz[0].y == wonsz[k].y) {

            snakeSpeed = 1;
            wonsz.clear();
            Snake pocz(10, 10);
            wonsz.push_front(pocz);
            direction = SnakeDirection::RIGHT;
            printf("Restart\n");
        }
    }
}


int main() {

    sf::Clock ticker;

    sf::RectangleShape snakeShape(sf::Vector2f(SIZE, SIZE));
    snakeShape.setFillColor(sf::Color::Green);

    for (int i = 0; i < APPLE_COUNT; i++) {
        sf::RectangleShape appleShape(sf::Vector2f(SIZE, SIZE));
        appleShape.setFillColor(sf::Color::Red);
        apples_rects.push_back(appleShape);
        apples_structs.push_back( struct Apple(i * dx + rand() % dx, rand() % SIZE) );
    }

    act_apples = APPLE_COUNT;


    wonsz.push_back(struct Snake());
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed){
                window.close();
            }
            /*
            else if (e.type == sf::Event::KeyPressed) {
                switch (e.key.code) {
                case sf::Keyboard::Up:
                    direction = SnakeDirection::UP;
                    printf("UP\n");
                    break;
                case sf::Keyboard::Down:
                    direction = SnakeDirection::DOWN;
                    printf("DOWN\n");
                    break;
                case sf::Keyboard::Left:
                    direction = SnakeDirection::LEFT;
                    printf("LEFT\n");
                    break;
                case sf::Keyboard::Right:
                    direction = SnakeDirection::RIGHT;
                    printf("RIGHT\n");
                    break;
                }
            }
            */
        }
        window.clear();

        if (ticker.getElapsedTime().asMilliseconds() > TICKS) {
            move();
            ticker.restart();
        }

       
        for (int i = 0; i < wonsz.size(); i++) {
            if (i == 0){
                sf::RectangleShape snake_0(sf::Vector2f(SIZE, SIZE));
                snake_0.setFillColor(sf::Color::Yellow);
                snake_0.setPosition(wonsz[i].x * SIZE, wonsz[i].y * SIZE);
                window.draw(snake_0);
            }
            else if( i < wonsz.size() - 1) {
                snakeShape.setPosition(wonsz[i].x * SIZE, wonsz[i].y * SIZE);
                window.draw(snakeShape);
            }
            else {
                sf::RectangleShape snake_last(sf::Vector2f(SIZE, SIZE));
                snake_last.setFillColor(sf::Color::Blue);
                snake_last.setPosition(wonsz[i].x * SIZE, wonsz[i].y * SIZE);
                window.draw(snake_last);
            }
        }
        
        for (int i = 0; i < apples_structs.size(); i++){
            Apple apple1 = apples_structs[i];
            apples_rects[i].setPosition(apple1.x * SIZE, apple1.y * SIZE);
            window.draw(apples_rects[i]);
        }

        window.display();
    }
    return 0;
}
