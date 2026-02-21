#include "logger.h"
#include "ds/dynamic_array.h"
#include "raylib.h"

int main(void)
{
    LoggerConfig config = getDefaultLoggerConfig();
    loggerInit(&config);
    LOG_DEBUG("Logger Initialized!");

    InitWindow(800, 600, "PathFinder");
    LOG_DEBUG("Raylib window initialized!");
    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        // Update

        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(0, 0);
        // Render

        EndDrawing();
    }

    LOG_DEBUG("Raylib window closing...");
    CloseWindow();

    LOG_DEBUG("Logger Termination...");
    loggerTerminate();

    return 0;
}