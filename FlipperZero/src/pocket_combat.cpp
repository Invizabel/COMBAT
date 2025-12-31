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

// assets. asset-1 is right attack and asset-2 is left attack

// skinny boy
int fighter_1[][2] = {{13,12},{14,12},{13,13},{14,13},{11,14},{12,14},{15,14},{16,14},{11,15},{12,15},{15,15},{16,15},{9,16},{10,16},{13,16},{14,16},{17,16},{18,16},{9,17},{10,17},{13,17},{14,17},{17,17},{18,17},{13,18},{14,18},{11,19},{12,19},{15,19},{16,19},{11,20},{12,20},{15,20},{16,20},{11,21},{12,21},{15,21},{16,21}};
// meat boy
int fighter_2[][2] = {{8,10},{9,10},{18,10},{19,10},{7,11},{10,11},{13,11},{14,11},{17,11},{20,11},{6,12},{10,12},{12,12},{15,12},{17,12},{21,12},{6,13},{8,13},{9,13},{12,13},{15,13},{18,13},{19,13},{21,13},{6,14},{9,14},{12,14},{15,14},{18,14},{21,14},{6,15},{10,15},{11,15},{16,15},{17,15},{21,15},{6,16},{21,16},{7,17},{12,17},{15,17},{20,17},{8,18},{11,18},{16,18},{18,18},{19,18},{9,19},{13,19},{14,19},{18,19},{9,20},{12,20},{15,20},{18,20},{10,21},{11,21},{16,21},{17,21}};
int fighter_2_1[][2] = {{8,10},{9,10},{7,11},{10,11},{13,11},{14,11},{6,12},{10,12},{12,12},{15,12},{6,13},{8,13},{9,13},{12,13},{15,13},{6,14},{9,14},{12,14},{15,14},{6,15},{10,15},{11,15},{16,15},{17,15},{18,15},{19,15},{20,15},{6,16},{21,16},{7,17},{12,17},{15,17},{22,17},{8,18},{11,18},{16,18},{18,18},{19,18},{23,18},{9,19},{13,19},{14,19},{18,19},{20,19},{23,19},{9,20},{12,20},{15,20},{18,20},{21,20},{23,20},{10,21},{11,21},{16,21},{17,21},{20,21},{21,21},{23,21},{19,22},{23,22},{19,23},{22,23},{20,24},{21,24}};
int fighter_2_2[][2] = {{18,10},{19,10},{13,11},{14,11},{17,11},{20,11},{12,12},{15,12},{17,12},{21,12},{12,13},{15,13},{18,13},{19,13},{21,13},{12,14},{15,14},{18,14},{21,14},{7,15},{8,15},{9,15},{10,15},{11,15},{16,15},{17,15},{21,15},{6,16},{21,16},{5,17},{12,17},{15,17},{20,17},{4,18},{8,18},{9,18},{11,18},{16,18},{18,18},{19,18},{4,19},{7,19},{9,19},{13,19},{14,19},{18,19},{4,20},{6,20},{9,20},{12,20},{15,20},{18,20},{4,21},{6,21},{7,21},{10,21},{11,21},{16,21},{17,21},{4,22},{8,22},{5,23},{8,23},{6,24},{7,24}};
// jellyfish
int fighter_3[][2] = {{11,10},{12,10},{13,10},{14,10},{15,10},{16,10},{10,11},{17,11},{10,12},{17,12},{11,13},{12,13},{13,13},{14,13},{15,13},{16,13},{11,14},{14,14},{16,14},{12,15},{14,15},{17,15},{10,16},{12,16},{15,16},{11,17}};

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

    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    
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
        if (dt.second % 3 == 0)
        {
            draw_enemy(canvas,fighter_2_1, sizeof(fighter_2_1) / sizeof(fighter_2_1[0]));
        }
        else if (dt.second % 5 == 0)
        {
            draw_enemy(canvas,fighter_2_2, sizeof(fighter_2_2) / sizeof(fighter_2_2[0]));
        }
        else
        {
            draw_enemy(canvas,fighter_2, sizeof(fighter_2) / sizeof(fighter_2[0]));
        }
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
