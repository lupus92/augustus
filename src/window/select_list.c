#include "select_list.h"

#include "graphics/button.h"
#include "graphics/color.h"
#include "graphics/generic_button.h"
#include "graphics/lang_text.h"
#include "graphics/panel.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "input/input.h"

#define MAX_ITEMS_PER_LIST 20
#define BASE_LIST_WIDTH 200
#define MAX_LIST_WIDTH 496

enum {
    MODE_TEXT,
    MODE_GROUP,
};

static void select_item(int id, int list_id);

static generic_button buttons_list1[MAX_ITEMS_PER_LIST] = {
    {5, 8, 190, 18, select_item, button_none, 0, 0},
    {5, 28, 190, 18, select_item, button_none, 1, 0},
    {5, 48, 190, 18, select_item, button_none, 2, 0},
    {5, 68, 190, 18, select_item, button_none, 3, 0},
    {5, 88, 190, 18, select_item, button_none, 4, 0},
    {5, 108, 190, 18, select_item, button_none, 5, 0},
    {5, 128, 190, 18, select_item, button_none, 6, 0},
    {5, 148, 190, 18, select_item, button_none, 7, 0},
    {5, 168, 190, 18, select_item, button_none, 8, 0},
    {5, 188, 190, 18, select_item, button_none, 9, 0},
    {5, 208, 190, 18, select_item, button_none, 10, 0},
    {5, 228, 190, 18, select_item, button_none, 11, 0},
    {5, 248, 190, 18, select_item, button_none, 12, 0},
    {5, 268, 190, 18, select_item, button_none, 13, 0},
    {5, 288, 190, 18, select_item, button_none, 14, 0},
    {5, 308, 190, 18, select_item, button_none, 15, 0},
    {5, 328, 190, 18, select_item, button_none, 16, 0},
    {5, 348, 190, 18, select_item, button_none, 17, 0},
    {5, 368, 190, 18, select_item, button_none, 18, 0},
    {5, 388, 190, 18, select_item, button_none, 19, 0},
};

static generic_button buttons_list2[MAX_ITEMS_PER_LIST] = {
    {205, 8, 190, 18, select_item, button_none, 0, 1},
    {205, 28, 190, 18, select_item, button_none, 1, 1},
    {205, 48, 190, 18, select_item, button_none, 2, 1},
    {205, 68, 190, 18, select_item, button_none, 3, 1},
    {205, 88, 190, 18, select_item, button_none, 4, 1},
    {205, 108, 190, 18, select_item, button_none, 5, 1},
    {205, 128, 190, 18, select_item, button_none, 6, 1},
    {205, 148, 190, 18, select_item, button_none, 7, 1},
    {205, 168, 190, 18, select_item, button_none, 8, 1},
    {205, 188, 190, 18, select_item, button_none, 9, 1},
    {205, 208, 190, 18, select_item, button_none, 10, 1},
    {205, 228, 190, 18, select_item, button_none, 11, 1},
    {205, 248, 190, 18, select_item, button_none, 12, 1},
    {205, 268, 190, 18, select_item, button_none, 13, 1},
    {205, 288, 190, 18, select_item, button_none, 14, 1},
    {205, 308, 190, 18, select_item, button_none, 15, 1},
    {205, 328, 190, 18, select_item, button_none, 16, 1},
    {205, 348, 190, 18, select_item, button_none, 17, 1},
    {205, 368, 190, 18, select_item, button_none, 18, 1},
    {205, 388, 190, 18, select_item, button_none, 19, 1},
};

static struct {
    int x;
    int y;
    int mode;
    int group;
    const uint8_t **items;
    unsigned int num_items;
    int width;
    void (*callback)(int);
    unsigned int focus_button_id;
} data;

static void init_group(int x, int y, int group, int num_items, void (*callback)(int))
{
    data.x = x;
    data.y = y;
    data.mode = MODE_GROUP;
    data.group = group;
    data.width = BASE_LIST_WIDTH;
    data.num_items = num_items;
    data.callback = callback;
    for (int i = 0; i < MAX_ITEMS_PER_LIST; i++) {
        buttons_list1[i].width = data.width - 10;
    }
}

static void init_text(int x, int y, const uint8_t **items, int num_items, void (*callback)(int))
{
    data.x = x;
    data.y = y;
    data.mode = MODE_TEXT;
    data.items = items;
    data.num_items = num_items;
    data.callback = callback;
    data.width = BASE_LIST_WIDTH;
    if (data.num_items <= MAX_ITEMS_PER_LIST) {
        for (int i = 0; i < num_items; i++) {
            int width = text_get_width(data.items[i], FONT_NORMAL_PLAIN) + 10;
            if (width > data.width) {
                data.width = width;
                data.width += BLOCK_SIZE - (data.width % BLOCK_SIZE);
                if (width > MAX_LIST_WIDTH) {
                    data.width = MAX_LIST_WIDTH;
                }
            }
        }
        for (int i = 0; i < num_items; i++) {
            buttons_list1[i].width = data.width;
        }
    } else {
        for (int i = 0; i < MAX_ITEMS_PER_LIST; i++) {
            buttons_list1[i].width = data.width - 10;
        }
    }
}

static unsigned int items_in_first_list(void)
{
    return data.num_items / 2 + data.num_items % 2;
}

static void draw_item(int item_id, int x, int y, int selected)
{
    color_t color = selected ? COLOR_FONT_BLUE : COLOR_BLACK;
    if (data.mode == MODE_GROUP) {
        lang_text_draw_centered_colored(data.group, item_id, data.x + x, data.y + y, data.width - 10,
            FONT_NORMAL_PLAIN, color);
    } else {
        if (data.width == BASE_LIST_WIDTH) {
            text_draw_centered(data.items[item_id], data.x + x, data.y + y, BASE_LIST_WIDTH - 10, FONT_NORMAL_PLAIN, color);
        } else {
            text_draw_ellipsized(data.items[item_id], data.x + x + 5, data.y + y,
                data.width - 10, FONT_NORMAL_PLAIN, color);
        }
    }
}

static void draw_foreground(void)
{
    if (data.num_items > MAX_ITEMS_PER_LIST) {
        unsigned int max_first = items_in_first_list();
        outer_panel_draw(data.x, data.y, 26, (20 * max_first + 24) / BLOCK_SIZE);
        for (unsigned int i = 0; i < max_first; i++) {
            draw_item(i, 5, 11 + 20 * i, i + 1 == data.focus_button_id);
        }
        for (unsigned int i = 0; i < data.num_items - max_first; i++) {
            draw_item(i + max_first, 205, 11 + 20 * i, MAX_ITEMS_PER_LIST + i + 1 == data.focus_button_id);
        }
    } else {
        int width_blocks = (data.width + BLOCK_SIZE - 1) / BLOCK_SIZE;
        outer_panel_draw(data.x, data.y, width_blocks, (20 * data.num_items + 24) / BLOCK_SIZE);
        for (unsigned int i = 0; i < data.num_items; i++) {
            draw_item(i, 5, 11 + 20 * i, i + 1 == data.focus_button_id);
        }
    }
}

static int click_outside_window(const mouse *m)
{
    int width;
    int height;
    if (data.num_items > MAX_ITEMS_PER_LIST) {
        width = 26 * BLOCK_SIZE;
        height = 20 * items_in_first_list() + 24;
    } else {
        width = data.width + BLOCK_SIZE - 1;
        height = 20 * data.num_items + 24;
    }
    return m->left.went_up && (m->x < data.x || m->x >= data.x + width || m->y < data.y || m->y >= data.y + height);
}

static void handle_input(const mouse *m, const hotkeys *h)
{
    if (data.num_items > MAX_ITEMS_PER_LIST) {
        unsigned int items_first = items_in_first_list();
        if (generic_buttons_handle_mouse(m, data.x, data.y, buttons_list1, items_first, &data.focus_button_id)) {
            return;
        }
        unsigned int second_id = 0;
        generic_buttons_handle_mouse(m, data.x, data.y, buttons_list2, data.num_items - items_first, &second_id);
        if (second_id > 0) {
            data.focus_button_id = second_id + MAX_ITEMS_PER_LIST;
        }
    } else {
        if (generic_buttons_handle_mouse(m, data.x, data.y, buttons_list1, data.num_items, &data.focus_button_id)) {
            return;
        }
    }
    if (input_go_back_requested(m, h) || click_outside_window(m)) {
        window_go_back();
    }
}

void select_item(int id, int list_id)
{
    window_go_back();
    if (list_id == 0) {
        data.callback(id);
    } else {
        data.callback(id + items_in_first_list());
    }
}

void window_select_list_show(int x, int y, int group, int num_items, void (*callback)(int))
{
    window_type window = {
        WINDOW_SELECT_LIST,
        window_draw_underlying_window,
        draw_foreground,
        handle_input
    };
    init_group(x, y, group, num_items, callback);
    window_show(&window);
}

void window_select_list_show_text(int x, int y, const uint8_t **items, int num_items, void (*callback)(int))
{
    window_type window = {
        WINDOW_SELECT_LIST,
        window_draw_underlying_window,
        draw_foreground,
        handle_input
    };
    init_text(x, y, items, num_items, callback);
    window_show(&window);
}
