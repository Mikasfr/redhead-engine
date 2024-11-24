#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>  // For loading images (PNG, JPG, etc.)
#include <iostream>
#include <vector>

const int WINDOW_WIDTH = 2560;
const int WINDOW_HEIGHT = 1440;
const int POINT_SIZE = 180;
const int CELL_SIZE = 180; // Size of each cell in the maze

// Define the maze layout (1 = wall, 0 = path)
std::vector<std::vector<int>> maze = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

bool isWall(int x, int y) {
    // Convert pixel coordinates to maze coordinates
    int mazeX = x / CELL_SIZE;
    int mazeY = y / CELL_SIZE;

    // Check if the coordinates are within the maze bounds
    if (mazeY < 0 || mazeY >= maze.size() || mazeX < 0 || mazeX >= maze[0].size()) {
        return true; // Out of bounds is considered a wall
    }
    return maze[mazeY][mazeX] == 1; // Return true if it's a wall
}

// Function to load an image and create a texture
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(path);  // Load image as surface
    if (!tempSurface) {
        printf("Failed to load image: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);  // Convert surface to texture
    SDL_FreeSurface(tempSurface);  // Free the surface after creating the texture
    return texture;
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    if (IMG_Init(IMG_INIT_WEBP) == 0) {  // Initialize PNG support (can also initialize JPG, TIF, etc.)
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return -1;
    }


    // Create a window
    SDL_Window* window = SDL_CreateWindow("SDL2 Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* playerTexture = loadTexture("pucas.webp", renderer);

    // Game loop
    bool running = true;
    int pointX = CELL_SIZE; // Start position
    int pointY = CELL_SIZE; // Start position
    while (running) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                int newX = pointX;
                int newY = pointY;
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        newX -= 180;
                        break;
                    case SDLK_RIGHT:
                        newX += 180;
                        break;
                    case SDLK_UP:
                        newY -= 180;
                        break;
                    case SDLK_DOWN:
                        newY += 180;
                        break;
                }
                // Check for wall collision before moving
                if (!isWall(newX, newY)) {
                    pointX = newX;
                    pointY = newY;
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set the background color to black
        SDL_RenderClear(renderer);

        // Draw the maze
        for (int y = 0; y < maze.size(); ++y) {
            for (int x = 0; x < maze[y].size(); ++x) {
                if (maze[y][x] == 1) {
                    // Draw walls in white
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                } else {
                    // Draw paths in black
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                }
                SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        // Draw the point (player) in red
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
        SDL_Rect destRect = {pointX, pointY, POINT_SIZE, POINT_SIZE};
        SDL_RenderCopy(renderer, playerTexture, NULL, &destRect);

        // Present the renderer (show the updated frame)
        SDL_RenderPresent(renderer);
        // Delay to control the frame rate
        SDL_Delay(16); // Roughly 60 frames per second
    }

    // Clean up and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

