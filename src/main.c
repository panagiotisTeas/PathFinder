#include "logger.h"
#include "raylib.h"

#include "grid.h"

#define WINDOW_WIDTH    1200
#define WINDOW_HEIGHT   800
#define WINDOW_TITLE    "PathFinder"
#define WINDOW_FPS      60

int main(void)
{
    LoggerConfig config = getDefaultLoggerConfig();
    loggerInit(&config);
    LOG_DEBUG("Logger Initialized!");

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    LOG_DEBUG("Raylib window initialized!");
    SetTargetFPS(WINDOW_FPS);

    gridCreate(WINDOW_WIDTH, WINDOW_HEIGHT, 20, 32);
    LOG_DEBUG("Grid created!");

    while(!WindowShouldClose())
    {
        // Update

        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(0, 0);

        // Render
        gridDraw();

        EndDrawing();
    }

    LOG_DEBUG("Grid destruction...");
    gridDestroy();

    LOG_DEBUG("Raylib window closing...");
    CloseWindow();

    LOG_DEBUG("Logger Termination...");
    loggerTerminate();

    return 0;
}