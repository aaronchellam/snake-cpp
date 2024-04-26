#include <iostream>
#include <raylib.h>
#include <random>
#include <deque>
#include <raymath.h>

using namespace std;

// Define colours (R,G,B, Transparancy).
Color lightGreen = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

// Define game grid.
int cellSize = 30;
int cellCount = 25;

// Define offset to construct a game border.
int offset = 75;

int screenWidth = cellSize*cellCount + 2*offset;
int screenHeight = screenWidth;

int gameOverTextSize = 80;
int restartTextSize = 60;

// Variable to track the last time the snake position was updated.
double lastUpdateTime = 0;

// Global Texture Management.
Texture2D foodTexture;

// Method used to determine if the snake should move or not.
// Returns true if sufficient time (specified by the interval argument) has passed since the last update time. Returns false otherwise.
bool eventTriggered(double interval) {
    // Number of seconds since the game started.
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }

    return false;
}


bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}

void LoadTextures() {
    Image image = LoadImage("graphics/food.png");
    foodTexture = LoadTextureFromImage(image);
    UnloadImage(image);
}

void UnloadTextures() {
    UnloadTexture(foodTexture);
}


// Define Snake Class.
class Snake {
    public:
        deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        Vector2 direction = {1, 0};
        Vector2 oldTail = body.back();

        void Draw() {
            for(unsigned int i = 0; i < body.size(); i++) {
                int x = body[i].x;
                int y = body[i].y;

                // DrawRectangle(x*cellSize, y*cellSize, cellSize, cellSize, darkGreen);
                Rectangle rect = Rectangle{x*cellSize + offset, y*cellSize + offset, (float) cellSize, (float) cellSize};
                DrawRectangleRounded(rect, 0.5, 6, darkGreen);
            }
        }

        // Method to update the position of the snake?
        void Update() {
            // When the snake moves, the last part of the snake can be removed.
            oldTail = body.back();
            body.pop_back();

            // A new square must be added for the new position of the head, based on the direction chosen by the player.
            body.push_front(Vector2Add(body[0], direction));

        }


        void GrowSnake() {
            body.push_back(oldTail);
        }

};


// Define Food class.
class Food {
    public:
        Vector2 position;
        Texture2D texture;

        // Constructor.
        Food(deque<Vector2> snakeBody) {
            // // An image represents pixel data that is stored in memory. Operations performed on images are performed in system memory (RAM), rather than graphics memory.
            // Image image = LoadImage("graphics/food.png");

            // // A texture is a representation of an image that is optimised  for being used and manipulated by the graphics hardware (typicallt the GPU).
            // texture = LoadTextureFromImage(image);

            // // Unload image to free memory.
            // UnloadImage(image);

            // Assign random coordinates.
            position = GenerateRandomPos(snakeBody);
            texture = foodTexture;

        }

        // Destructor method to release unused resources once the object moves out of scope.
        // ~Food() {
        //     UnloadTexture(texture);
        // }

        void Draw() {
            // // Specify x position, y position, width, height, colour of food.
            // DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize, darkGreen);

            // Specify texture to draw, x pos, y post, colour tint.
            DrawTexture(texture, position.x * cellSize + offset, position.y * cellSize + offset, WHITE);

        }

        Vector2 GenerateRandomCell() {
            float x = GetRandomValue(0, cellCount-1);
            float y = GetRandomValue(0, cellCount-1);
            return Vector2{x, y};
        }

        Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
            Vector2 position = GenerateRandomCell();
            while(ElementInDeque(position, snakeBody)) {
                position = GenerateRandomCell();
            }
            return position;
        }
};


class Game {
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool gameover = false;
        int score = 0;
        string scoretext = "Score: " + to_string(score);



        void Draw() {
            food.Draw();
            snake.Draw();
        }

        void Update() {
            // Move the snake;
            snake.Update();

            // Check if the snake has collided with the border or itself.
            if (CheckBorderCollision() || CheckSnakeCollision()) {
                cout << "Collision Detected" << endl;
                gameover = true;
                return;
            }


            // Check if snake can eat food and grow.
            CheckFoodCollision();
            UpdateScoreText();

        }

        void CheckFoodCollision() {
            if(Vector2Equals(snake.body[0], food.position)) {
                food.position = food.GenerateRandomPos(snake.body);
                snake.GrowSnake();
                score++;
            }
        }


        bool CheckBorderCollision() {
            Vector2 head = snake.body[0];
            if(head.x < 0 || head.x > 24) {
                return true;
            }

            if (head.y < 0 || head.y > 24) {
                return true;
            }

            return false;
        }


        bool CheckSnakeCollision() {
            Vector2 head = snake.body[0];
            Vector2 bodypart;

            for (unsigned int i=1; i < snake.body.size(); i++) {
                bodypart = snake.body[i];
                if (head.x == bodypart.x && head.y == bodypart.y) {
                    return true;
                }
            }

            return false;
        }


        void HandleSnakeDirection() {
            if (IsKeyPressed(KEY_UP) && snake.direction.y != 1) {
                snake.direction = {0, -1};
            }
            
            if (IsKeyPressed(KEY_DOWN) && snake.direction.y != -1) {
                snake.direction = {0, 1};
            }
            
            if (IsKeyPressed(KEY_LEFT) && snake.direction.x != 1) {
                snake.direction = {-1, 0};
            }
            
            if (IsKeyPressed(KEY_RIGHT) && snake.direction.x != -1) {
                snake.direction = {1, 0};
            }
        }

        void UpdateScoreText() {
            scoretext = "Score: " + to_string(score);
        }

};

int main()
{
    cout << "Starting the game..." << endl;

    // Create game window - 750x750 pixels.
    InitWindow(cellCount * cellSize + 2*offset, cellCount * cellSize + 2*offset, "Retro Snake");

    // Define an ideal frame rate for the game. Without this, the computer will run the game as fast as it can.
    // The fps is the number of times per second the game loop runs.
    SetTargetFPS(60);

    // Load textures;
    LoadTextures();

    // Create a game object, which handles snake and food behaviour.
    Game game = Game();

    // WindowShouldClose() returns true if esc is pressed or the top right X is pressed.
    while(WindowShouldClose() == false) {
        // Handle user input.
        if (!game.gameover) {
            // Change snake direction according to user input.
            game.HandleSnakeDirection();
        } else {
            // Option to restart the game.
            if (IsKeyPressed(KEY_R)) {
                game = Game();
            }
        }

        // Update game logic.
        if (!game.gameover) {
            if (eventTriggered(0.2)) {
                game.Update();
            }
        }



        // Start drawing.
        BeginDrawing();
        // Clear any drawings from the previous loop iteration.
        ClearBackground(lightGreen);
        DrawRectangleLinesEx(Rectangle{(float) offset-5, (float) offset-5, (float) cellSize*cellCount+10, (float) cellSize*cellCount+10}, 5, darkGreen);
        DrawText("Classic Snake", offset-5, 20, 40, darkGreen);
        DrawText(game.scoretext.c_str(), offset + (cellSize*cellCount - MeasureText(game.scoretext.c_str(), 40)), 20, 40, darkGreen);

        if (!game.gameover) {
            // Draw the updated game objects.
            game.Draw();
        } else {
            DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", gameOverTextSize) / 2, screenHeight / 2 - gameOverTextSize, gameOverTextSize, RED);
            DrawText("Press R to Restart", screenWidth / 2 - MeasureText("Press R to Restart", restartTextSize) / 2, screenHeight / 2, restartTextSize, darkGreen);
        }


        EndDrawing();

    }

    UnloadTextures();
    CloseWindow();
    return 0;
}
