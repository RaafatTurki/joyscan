#include <stdio.h>
#include "raylib.h"

#define DEBUG_MODE true
#include "input.h"
#include "log.h"
#include "vector.h"

#if defined(__linux__)
#define PLATFORM "linux"
#endif


void display_gamepad(int id) {
    // defining the buttons

	Input LX = input_new(GAMEPAD_AXIS_LEFT_X,              YELLOW, 1, 18, 20, ANALOG, 0, "Left X Axis");
	Input LY = input_new(GAMEPAD_AXIS_LEFT_Y,              YELLOW, 1, 19, 20, ANALOG, 0, "Left Y Axis");
	Input RX = input_new(GAMEPAD_AXIS_RIGHT_X,             YELLOW, 1, 20, 20, ANALOG, 0, "Right X Axis");
	Input RY = input_new(GAMEPAD_AXIS_RIGHT_Y,             YELLOW, 1, 21, 20, ANALOG, 0, "Right Y Axis");

    Input inputs[] = {
        input_new(GAMEPAD_BUTTON_MIDDLE_RIGHT,      RED,    1, 1, 20, BUTTON, 0, "Start"),
        input_new(GAMEPAD_BUTTON_MIDDLE_LEFT,       RED,    1, 2, 20, BUTTON, 0, "Select"),
        input_new(GAMEPAD_BUTTON_MIDDLE,            GREEN,  1, 3, 20, BUTTON, 0, "Home"),

        input_new(GAMEPAD_BUTTON_RIGHT_FACE_UP,     GOLD,   1, 4, 20, BUTTON, 0, "Y"),
        input_new(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,  LIME,   1, 5, 20, BUTTON, 0, "B"),
        input_new(GAMEPAD_BUTTON_RIGHT_FACE_DOWN,   BLUE,   1, 6, 20, BUTTON, 0, "A"),
        input_new(GAMEPAD_BUTTON_RIGHT_FACE_LEFT,   MAROON, 1, 7, 20, BUTTON, 0, "X"),

        input_new(GAMEPAD_BUTTON_LEFT_FACE_UP,      RED,    1, 8, 20, BUTTON, 0, "Up"),
        input_new(GAMEPAD_BUTTON_LEFT_FACE_DOWN,    RED,    1, 9, 20, BUTTON, 0, "Down"),
        input_new(GAMEPAD_BUTTON_LEFT_FACE_LEFT,    RED,    1, 10, 20, BUTTON, 0, "Left"),
        input_new(GAMEPAD_BUTTON_LEFT_FACE_RIGHT,   RED,    1, 11, 20, BUTTON, 0, "Right"),

        input_new(GAMEPAD_BUTTON_RIGHT_THUMB,       RED,    1, 12, 20, BUTTON, 0, "Right Thumb"),
        input_new(GAMEPAD_BUTTON_LEFT_THUMB,        RED,    1, 13, 20, BUTTON, 0, "Left Thumb"),

        input_new(GAMEPAD_BUTTON_LEFT_TRIGGER_1,    RED,    1, 14, 20, BUTTON, 0, "Left Trigger 1"),
        input_new(GAMEPAD_BUTTON_LEFT_TRIGGER_2,    RED,    1, 15, 20, BUTTON, 0, "Left Trigger 2"),
        input_new(GAMEPAD_BUTTON_RIGHT_TRIGGER_1,   RED,    1, 16, 20, BUTTON, 0, "Right Trigger 1"),
        input_new(GAMEPAD_BUTTON_RIGHT_TRIGGER_2,   RED,    1, 17, 20, BUTTON, 0, "Right Trigger 2"),

		LX,
        LY,
        RX,
        RY,

        input_new(GAMEPAD_AXIS_RIGHT_TRIGGER,       YELLOW, 1, 22, 20, ANALOG, -1, "Right Trigger Axis"),
        input_new(GAMEPAD_AXIS_LEFT_TRIGGER,        YELLOW, 1, 23, 20, ANALOG, -1, "Left Trigger Axis"),
    };

	// dot in circle stuff
	int rad = 100;
	int c1x = 400;
	int c1y = 200;

	int c2x = 700;
	int c2y = 200;

	DrawRectangle(c1x, c1y, rad*2, rad*2, WHITE);
	DrawRectangle(c2x, c2y, rad*2, rad*2, WHITE);

	float LXx = (float)GetGamepadAxisMovement(0, LX.id) * rad;
	float LYy = (float)GetGamepadAxisMovement(0, LY.id) * rad;

	float RXx = (float)GetGamepadAxisMovement(0, RX.id) * rad;
	float RYy = (float)GetGamepadAxisMovement(0, RY.id) * rad;

    DrawCircle((c1x) + LXx + rad, (c1y) + LYy + rad, 10, BLACK);
    DrawCircle((c2x) + RXx + rad, (c2y) + RYy + rad, 10, BLACK);

    /* DrawRingLines(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);    // Draw ring outline */

	/* Input xbox_grid[][] = { */
	/* 	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, */
	/* 	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, */
	/* 	{NULL, GL, NULL, NULL, NULL, NULL, NULL, Y,	 NULL}, */
	/* 	{NULL, NULL, NULL, NULL, NULL, NULL, X,	   NULL, B}, */
	/* 	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, A,	 NULL}, */
	/* 	{NULL, NULL, NULL, NULL, NULL, GR, NULL, NULL, NULL}, */
	/* } */


    const int INPUTS_COUNT = sizeof inputs / sizeof inputs[0];

    for (int i = 0; i < INPUTS_COUNT; i++) {
        Input input = inputs[i];

        if (input.type == BUTTON) {
            Color color = WHITE;
            if(IsGamepadButtonDown(id, input.id)) color = input.color;
            DrawText(TextFormat("%s", input.name), input.x*20, input.y*20, input.s, color);
        }

        if (input.type == ANALOG) {
            Color color = WHITE;
            if(GetGamepadAxisMovement(id, input.id) != input.idle_value) color = input.color;
            /* DrawText(TextFormat("%s %f", input.name, (float)GetGamepadAxisMovement(id, input.id)), input.x, input.y, input.s, color); */
            DrawText(TextFormat("%s", input.name), input.x*20, input.y*20, input.s, color);
            DrawText(TextFormat("%f", (float)GetGamepadAxisMovement(id, input.id)), (input.x*20)+(12*20), input.y*20, input.s, color);
        }
    }

    // TODO take a look at this detection stuff
    /*             DrawText(TextFormat("DETECTED AXIS [%i]:", GetGamepadAxisCount(0)), 10, 50, 10, MAROON); */
    /*  */
    /*             for (int i = 0; i < GetGamepadAxisCount(0); i++) { */
    /*                 DrawText(TextFormat("AXIS %i: %.02f", i, GetGamepadAxisMovement(0, i)), 20, 70 + 20*i, 10, DARKGRAY); */
    /*             } */
    /*  */
    /*             if (GetGamepadButtonPressed() != -1) DrawText(TextFormat("DETECTED BUTTON: %i", GetGamepadButtonPressed()), 10, 430, 10, RED); */
    /*             else DrawText("DETECTED BUTTON: NONE", 10, 430, 10, GRAY); */
}

int main(void) {
    int w = 1000;
    int h = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(w, h, "gamepad");
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    Vector gamepad_ids;
    vec_init(&gamepad_ids, 0);
    int gamepad_id_curr = -1;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        w = GetScreenWidth();
        h = GetScreenHeight();

        // detect connected and disconnected devices
        for (int id = 0; id <= vec_len(&gamepad_ids)+1; id++) {
            if (IsGamepadAvailable(id) && vec_has_val(&gamepad_ids, id) == -1) {
                vec_append(&gamepad_ids, id);
                gamepad_id_curr = id;
                print_log(MAIN, TextFormat("Connected gamepad id %d", id));

            } else if (!IsGamepadAvailable(id) && vec_has_val(&gamepad_ids, id) > -1) {
                vec_remove(&gamepad_ids, id);

                if (id == gamepad_id_curr) {
                    bool is_reconnected = false;
                    for (int new_id = 0; id <= vec_len(&gamepad_ids); id++) {
                        if (IsGamepadAvailable(new_id)) {
                            gamepad_id_curr = new_id;
                            is_reconnected = true;
                        }
                    }
                    if (!is_reconnected) gamepad_id_curr = -1;
                }
                print_log(MAIN, TextFormat("Disconnected gamepad id %d", id));
            }
        }

        // drawing everything
        if (gamepad_id_curr == -1) {
            DrawText("No Controller Detected :c", 10, 10, 20, WHITE);
        } else {
            DrawText(TextFormat("%d gamepads connected", vec_len(&gamepad_ids)), 400, (1)*20, 20, WHITE);

            for (int i = 0; i < vec_len(&gamepad_ids); i++) {
                int id = vec_get(&gamepad_ids, i);
                Color color = WHITE;
                if (id == gamepad_id_curr) color = RED;
                DrawText(TextFormat("%s", GetGamepadName(id)), 400, (i+4)*20, 20, color);
            }
            display_gamepad(gamepad_id_curr);
        }

        // gamepad switching
        if (IsKeyPressed(KEY_DOWN)) {
            int i = vec_has_val(&gamepad_ids, gamepad_id_curr);
            int next_i = vec_next(&gamepad_ids, i);
            gamepad_id_curr = vec_get(&gamepad_ids, next_i);
        }
        if (IsKeyPressed(KEY_UP)) {
            int i = vec_has_val(&gamepad_ids, gamepad_id_curr);
            int prev_i = vec_prev(&gamepad_ids, i);
            gamepad_id_curr = vec_get(&gamepad_ids, prev_i);
        }

        /* DrawRectangle(317, 202, 19, 71, BLACK); */

        // calibration
        if (PLATFORM != "linux") {
            DrawText("Calibration is only available on linux :P", w-420, h-40, 20, WHITE);
        } else {
            
            DrawText("Calibration", w-130, h-40, 20, WHITE);
        }

        DrawText("raafat.turki@protonmail.com", 10, h-30, 10, WHITE);
        DrawText("muhammedturki@protonmail.com", 10, h-20, 10, WHITE);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
