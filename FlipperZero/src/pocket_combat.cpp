#include "dolphin/dolphin.h"
#include <furi.h>
#include <furi_hal_rtc.h>
#include <gui/gui.h>
#include <stdlib.h>

char player_str[16];
char enemy_str[16];
int SCALE = 1;

bool moving_right = false;
bool moving_left = false;

int player_x = 26; // 128 / 5 = 25.6
int player_y = 32;
int player_hp = 10;

int enemy_x = 102; // 25.6 * 4 = 102.4
int enemy_y = 32;
int enemy_hp = 10;

int player_sword[2] = {player_x+1, player_y};
int enemy_sword[2] = {enemy_x-1, enemy_y};

int random_enemy = rand() % 3 + 1;
bool is_first = true;
int choice = 1;

int box_ = 1;

// skinny boy
int fighter_1[][2] = {{9,8},{10,8},{9,9},{10,9},{7,10},{8,10},{11,10},{12,10},{7,11},{8,11},{11,11},{12,11},{5,12},{6,12},{9,12},{10,12},{13,12},{14,12},{5,13},{6,13},{9,13},{10,13},{13,13},{14,13},{9,14},{10,14},{7,15},{8,15},{11,15},{12,15},{7,16},{8,16},{11,16},{12,16},{7,17},{8,17},{11,17},{12,17}};
// meat boy
int fighter_2[][2] = {{4,6},{5,6},{14,6},{15,6},{3,7},{6,7},{9,7},{10,7},{13,7},{16,7},{2,8},{6,8},{8,8},{11,8},{13,8},{17,8},{2,9},{4,9},{5,9},{8,9},{11,9},{14,9},{15,9},{17,9},{2,10},{5,10},{8,10},{11,10},{14,10},{17,10},{2,11},{6,11},{7,11},{12,11},{13,11},{17,11},{2,12},{17,12},{3,13},{8,13},{11,13},{16,13},{4,14},{7,14},{12,14},{14,14},{15,14},{5,15},{9,15},{10,15},{14,15},{5,16},{8,16},{11,16},{14,16},{6,17},{7,17},{12,17},{13,17}};
// jellyfish
int fighter_3[][2] = {{3,2},{4,2},{5,2},{6,2},{7,2},{8,2},{2,3},{9,3},{2,4},{9,4},{3,5},{4,5},{5,5},{6,5},{7,5},{8,5},{3,6},{6,6},{8,6},{4,7},{6,7},{9,7},{2,8},{4,8},{7,8},{3,9}};

void collide_rect()
{
    int player_sword[2] = {player_x, player_y};
    int enemy_sword[2] = {enemy_x, enemy_y};
    
    int player_sword_left = player_sword[0] - 8;
    int player_sword_top = player_sword[1] - 8;
    int player_sword_right = player_sword[0] + 8;
    int player_sword_bottom = player_sword[1] + 8;
    
    int enemy_sword_left = enemy_sword[0] - 8;
    int enemy_sword_top = enemy_sword[1] - 8;
    int enemy_sword_right = enemy_sword[0] + 8;
    int enemy_sword_bottom = enemy_sword[1] + 8;
    
    bool sword_collision = enemy_sword_left <= player_sword_right && enemy_sword_right >= player_sword_left && enemy_sword_top <= player_sword_bottom && enemy_sword_bottom >= player_sword_top;

    if (sword_collision)
    {
        if (is_first && enemy_hp > 0)
        {
            enemy_hp -= 1;
        }

        if (!is_first && player_hp > 0)
        {
            player_hp -= 1;
        }
    }
}


void draw_player(Canvas* canvas, int data[][2], int array_size)
{   
    for(int i = 0; i < array_size; i++)
    {
        int x = (player_x + data[i][0] * SCALE);
        int y = (player_y + data[i][1] * SCALE);

        if(x >= 0 && y >= 0 && x < 128 && y < 64)
        {
            canvas_draw_box(canvas, x, y, SCALE, SCALE);
        }
    }
}

void draw_enemy(Canvas* canvas, int data[][2], int array_size)
{    
    for(int i = 0; i < array_size; i++)
    {
        int x = (enemy_x + data[i][0] * SCALE);
        int y = (enemy_y + data[i][1] * SCALE);

        if(x >= 0 && y >= 0 && x < 128 && y < 64)
        {
            canvas_draw_box(canvas, x, y, SCALE, SCALE);
        }
    }
}

static void input_callback(InputEvent* event, void* context)
{
    FuriMessageQueue* queue = (FuriMessageQueue*)context;
    if(event->type == InputTypeShort || event->type == InputTypeRepeat || event->type == InputTypePress)
    {
        if (event->key == InputKeyLeft)
        {
             player_x -= 2;
        }

        if (event->key == InputKeyRight)
        {
            player_x += 2;
        }
    }
    
    
    furi_message_queue_put(queue, event, FuriWaitForever);
}


static void draw_callback(Canvas* canvas, void* context)
{
    UNUSED(context);
    
    canvas_clear(canvas);

    // player
    if (choice == 1)
    {
        draw_player(canvas,fighter_1, sizeof(fighter_1) / sizeof(fighter_1[0]));   
    }

    if (choice == 2)
    {   
        draw_player(canvas,fighter_2,sizeof(fighter_2) / sizeof(fighter_2[0]));
    }

    if (choice == 3)
    {
        draw_player(canvas,fighter_3,sizeof(fighter_3) / sizeof(fighter_3[0]));   
    }

    // enemy
    if (random_enemy == 1)
    {
        draw_enemy(canvas,fighter_1, sizeof(fighter_1) / sizeof(fighter_1[0]));   
    }

    if (random_enemy == 2)
    {
        draw_enemy(canvas,fighter_2, sizeof(fighter_2) / sizeof(fighter_2[0]));
    }

    if (random_enemy == 3)
    {   
        draw_enemy(canvas,fighter_3,sizeof(fighter_3) / sizeof(fighter_3[0]));   
    }

    collide_rect();
    snprintf(player_str, sizeof(player_str), "%d", player_hp);
    canvas_draw_str(canvas,2,8,player_str);

    snprintf(enemy_str, sizeof(enemy_str), "%d", enemy_hp);
    canvas_draw_str(canvas,112,8,enemy_str);

    canvas_commit(canvas);
}

int main()
{   
    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    unsigned int seed = dt.hour * 3600 + dt.minute * 60 + dt.second;
    srand(seed);

    FuriMessageQueue* queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, queue);
    Gui* gui = (Gui*)furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    dolphin_deed(DolphinDeedPluginGameStart);
    InputEvent event;
    bool running = true;
    while(running)
    {
        if(furi_message_queue_get(queue, &event, FuriWaitForever) == FuriStatusOk)
        {
            if(event.type == InputTypeShort && event.key == InputKeyBack)
            {
                running = false;
            }
        }
        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    furi_message_queue_free(queue);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}
