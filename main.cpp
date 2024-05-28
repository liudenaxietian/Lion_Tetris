#include <raylib.h>
#include "grid.h"

int main()
{

    Color darkBlue = Color{44, 44, 127, 255};
    InitWindow(300, 600, "Lion");
    SetTargetFPS(60);
    Grid grid = Grid();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        ClearBackground(darkBlue);
        grid.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}