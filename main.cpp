#include <raylib.h>
#include "game.h"

int main()
{

    Color darkBlue = Color{44, 44, 127, 255};
    InitWindow(300, 600, "Lion");
    SetTargetFPS(60);
    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        ClearBackground(darkBlue);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}