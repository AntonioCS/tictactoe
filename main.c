/* 
 * File:   main.c
 * Author: antoniocs
 *
 * Created on 16 de Julho de 2015, 21:46
 */

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_SQUARES 9
#define TOTAL_ROWS 3
#define TOTAL_SQUARES_PER_ROW 3

#define SQUARE_DEFAULT_COLOUR 0x00, 0x00, 0x00 //Black
#define SQUARE_SELECTED_COLOUR 0xFF, 0x00, 0x00 //Red

#define USER_MOVED_UP 1
#define USER_MOVED_DOWN 2
#define USER_MOVED_LEFT 3
#define USER_MOVED_RIGHT 4

struct colour {
    Uint8 a;
    Uint8 b;
    Uint8 c;
};

struct possible_moves {
    bool left;
    bool right;
    bool up;
    bool down;
};

struct possible_moves all_moves[3][3] = {

    {
        {.left = false, .right = true, .up = false, .down = true}, //0,0
        {.left = true, .right = true, .up = false, .down = true}, //0,1
        {.left = true, .right = false, .up = false, .down = true} //0,2
    },

    {
        {.left = false, .right = true, .up = true, .down = true}, //1,0
        {.left = true, .right = true, .up = true, .down = true}, //1,1
        {.left = true, .right = false, .up = true, .down = true} //1,2
    },

    {
        {.left = false, .right = true, .up = true, .down = false}, //2,0
        {.left = true, .right = true, .up = true, .down = false}, //2,1
        {.left = true, .right = false, .up = true, .down = false} //2,2
    },

};

struct gameSquare {
    SDL_Rect rectangle;
    bool selected;
    char state; //State can be O, X, - (- means it has nothing)
    struct possible_moves *moves;
};

void draw_rectangle(SDL_Renderer *renderer, struct colour colour, SDL_Rect rectangle, bool fill, Uint8 alpha) {
    SDL_SetRenderDrawColor(renderer, colour.a, colour.b, colour.c, alpha);

    if (fill) {
        SDL_RenderFillRect(renderer, &rectangle);
    } else {
        SDL_RenderDrawRect(renderer, &rectangle);
    }
}

void clear_screen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}


void printf_rect(SDL_Rect *);

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[]) {

    //The window renderer
    SDL_Renderer *renderer = NULL;

    //The window we'll be rendering to
    SDL_Window *gWindow;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        //Create window
        gWindow = SDL_CreateWindow("Tic tac toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (gWindow != NULL) {
            //Get window surface
            renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            int imgFlags = IMG_INIT_PNG;

            if (IMG_Init(imgFlags) & imgFlags) {
                bool quit = false;

                //Event handler
                SDL_Event e;

                struct colour default_colour = {SQUARE_DEFAULT_COLOUR};
                struct colour selected_colour = {SQUARE_SELECTED_COLOUR};

                int big_square_width = SCREEN_WIDTH - 20;
                int big_square_heigth = SCREEN_HEIGHT - 20;

                int smaller_squares_width = big_square_width / 4 + 35;
                int smaller_squares_heigth = big_square_heigth / 4 + 20;
                int smaller_squares_x = 15;
                int smaller_squares_y = 15;
                //Increase for loop that draws all the small squares
                int x_increase = smaller_squares_width + smaller_squares_x + 5;
                int y_increase = smaller_squares_heigth + smaller_squares_y + 8;


                SDL_Rect big_square_coords = {
                    .x = 10,
                    .y = 10,
                    .w = big_square_width,
                    .h = big_square_heigth
                };

                SDL_Rect smaller_square_coords = {
                    .x = smaller_squares_x,
                    .y = smaller_squares_y,
                    .w = smaller_squares_width,
                    .h = smaller_squares_heigth
                };

                printf("Initial square coords\n");
                printf_rect(&smaller_square_coords);

                struct gameSquare gameSquares[TOTAL_SQUARES];
                int square = 0;
                int selected_square = 0;

                for (int i = 0; i < TOTAL_ROWS; ++i) {
                    for (int ii = 0; ii < TOTAL_SQUARES_PER_ROW; ++ii) {
                        struct gameSquare newSquare = {
                            .rectangle = smaller_square_coords,
                            .selected = false,
                            .state = '-',
                            .moves = &(all_moves[i][ii])
                        };

                        if (square == selected_square) {
                            newSquare.selected = true;
                        }

                        gameSquares[square++] = newSquare;
                        smaller_square_coords.x += x_increase;

                        printf("Square: %d, Col: %d - Row: %d\n", square, i, ii);
                        printf_rect(&smaller_square_coords);
                    }

                    smaller_square_coords.x = smaller_squares_x;
                    smaller_square_coords.y += y_increase;
                }
                smaller_square_coords.y = smaller_squares_y;

                //While application is running
                while (!quit) {
                    int user_move = 0;
                    //Handle events on queue
                    while (SDL_PollEvent(&e) != 0) {
                        //User requests quit ----------- Quit also with ESCAPE key pressed
                        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                            quit = true;
                        }

                        if (e.type == SDL_KEYDOWN) {
                            switch (e.key.keysym.sym) {
                                case SDLK_UP:
                                case SDLK_w:
                                    user_move = USER_MOVED_UP;
                                    break;

                                case SDLK_DOWN:
                                case SDLK_s:
                                    user_move = USER_MOVED_DOWN;
                                    break;

                                case SDLK_LEFT:
                                case SDLK_a:
                                    user_move = USER_MOVED_LEFT;
                                    break;

                                case SDLK_RIGHT:
                                case SDLK_d:
                                    user_move = USER_MOVED_RIGHT;
                                    break;
                            }
                        }
                    }

                    clear_screen(renderer);

                    //Big square
                    draw_rectangle(renderer, default_colour, big_square_coords, false, 255);

                    if (user_move) {
                        int move_to = -1;

                        switch (user_move) {
                            case USER_MOVED_UP:
                                if (gameSquares[selected_square].moves->up) {
                                    move_to = selected_square - 3;
                                }
                                break;
                            case USER_MOVED_DOWN:
                                if (gameSquares[selected_square].moves->down) {
                                    move_to = selected_square + 3;
                                }
                                break;
                            case USER_MOVED_LEFT:
                                if (gameSquares[selected_square].moves->left) {
                                    move_to = selected_square - 1;
                                }
                                break;
                            case USER_MOVED_RIGHT:
                                if (gameSquares[selected_square].moves->right) {
                                    move_to = selected_square + 1;
                                }
                                break;
                        }


                        if (move_to >= 0) {
                            gameSquares[selected_square].selected = false;
                            gameSquares[move_to].selected = true;

                            selected_square = move_to;
                        }
                    }

                    //draw smaller rectangles
                    for (int i = 0; i < TOTAL_SQUARES; ++i) {
                        struct colour colour_used = default_colour;

                        if (gameSquares[i].selected) {
                            selected_square = i;
                            colour_used = selected_colour;
                        }

                        //little square                        
                        draw_rectangle(renderer, colour_used, gameSquares[i].rectangle, false, 255);
                    }

                    SDL_RenderPresent(renderer);                
                }
            }


        } else {
            printf("SDL_Init failed ON WINDOW: %s\n", SDL_GetError());
        }
    }

    //Destroy window
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();


    return (EXIT_SUCCESS);
}

void printf_rect(SDL_Rect *rect) {
    printf("Rect X: %d, Y: %d, W: %d, H: %d\n", rect->x, rect->y, rect->w, rect->h);
}