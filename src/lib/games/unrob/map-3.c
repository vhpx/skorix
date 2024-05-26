#include "map-3.h"

const Position map_3_spawn_point = {
    .x = (SCREEN_WIDTH - PLAYER_WIDTH) / 2,
    .y = (SCREEN_HEIGHT - MARGIN - PLAYER_HEIGHT) - 40,
};

// const Position guard_1_spawn_point = {
//     .x = 750,
//     .y = (SCREEN_HEIGHT) / 2 - 200,
// };

// const Position guard_2_spawn_point = {
//     .x = 250,
//     .y = (SCREEN_HEIGHT) / 2 + 100,
// };

const Size map_3_map_size = {
    .width = SCREEN_WIDTH,
    .height = SCREEN_HEIGHT,
};

Item map_3_map_items[] = {
    {
        .id = 1,
        .name = "Item 1",
        .final_position = {.x = 125, .y = 150},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m1_1,
            },
    },
    {
        .id = 2,
        .name = "Item 2",
        .final_position = {.x = 320, .y = 150},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m1_2,
            },
    },
    {
        .id = 3,
        .name = "Item 3",
        .final_position = {.x = 690, .y = 150},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m1_3,
            },
    },
    {
        .id = 4,
        .name = "Item 4",
        .final_position = {.x = 830, .y = 150},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m1_4,
            },
    },
    {
        .id = 5,
        .name = "Item 5",
        .final_position = {.x = 120, .y = 810},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m1_5,
            },
    },
    {
        .id = 6,
        .name = "Item 6",
        .final_position = {.x = 330, .y = 810},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m1_6,
            },
    },
    {
        .id = 7,
        .name = "Item 7",
        .final_position = {.x = 850, .y = 810},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m1_7,
            },
    },
};

const Position map_3_map_wall_boundaries[] = {
    {.x = 105, .y = 145}, {.x = 105, .y = 305}, {.x = 155, .y = 305},
    {.x = 155, .y = 415}, {.x = 95, .y = 415},  {.x = 95, .y = 635},
    {.x = 155, .y = 635}, {.x = 155, .y = 735}, {.x = 105, .y = 735},
    {.x = 105, .y = 865}, {.x = 215, .y = 865}, {.x = 215, .y = 805},
    {.x = 275, .y = 805}, {.x = 275, .y = 865}, {.x = 435, .y = 865},
    {.x = 435, .y = 745}, {.x = 455, .y = 745}, {.x = 455, .y = 805},
    {.x = 465, .y = 805}, {.x = 465, .y = 805}, {.x = 465, .y = 995},
    {.x = 535, .y = 995}, {.x = 535, .y = 935}, {.x = 545, .y = 935},
    {.x = 545, .y = 805}, {.x = 555, .y = 805}, {.x = 555, .y = 745},
    {.x = 575, .y = 745}, {.x = 575, .y = 865}, {.x = 905, .y = 865},
    {.x = 905, .y = 745}, {.x = 855, .y = 745}, {.x = 855, .y = 635},
    {.x = 915, .y = 635}, {.x = 915, .y = 425}, {.x = 855, .y = 425},
    {.x = 855, .y = 305}, {.x = 905, .y = 305}, {.x = 905, .y = 145},
    {.x = 805, .y = 145}, {.x = 805, .y = 205}, {.x = 755, .y = 205},
    {.x = 755, .y = 145}, {.x = 675, .y = 145}, {.x = 675, .y = 205},
    {.x = 555, .y = 205}, {.x = 555, .y = 215}, {.x = 455, .y = 215},
    {.x = 455, .y = 205}, {.x = 375, .y = 205}, {.x = 375, .y = 145},
    {.x = 305, .y = 145}, {.x = 305, .y = 205}, {.x = 205, .y = 205},
    {.x = 205, .y = 145}, {.x = 105, .y = 145},
};

const Position map_3_map_fountain_boundaries[] = {
    {.x = 445, .y = 480}, {.x = 445, .y = 550}, {.x = 470, .y = 575},
    {.x = 530, .y = 575}, {.x = 555, .y = 550}, {.x = 555, .y = 480},
    {.x = 530, .y = 455}, {.x = 470, .y = 455},
};

const Boundary map_3_map_boundaries[] = {
    {.positions = map_3_map_wall_boundaries,
     .num_positions = sizeof(map_3_map_wall_boundaries) / sizeof(Position)},
    {.positions = map_3_map_fountain_boundaries,
     .num_positions = sizeof(map_3_map_fountain_boundaries) / sizeof(Position)},
};

Guard map_3_guards[] = {

};

GameMap map3 = {
    .player_position = map_3_spawn_point,
    .boundaries = map_3_map_boundaries,
    .bitmap = game_map_3_bitmap,
    .size = map_3_map_size,
    .items = map_3_map_items,
    .guards = map_3_guards,
    .num_boundaries = sizeof(map_3_map_boundaries) / sizeof(Boundary),
    .num_guards = sizeof(map_3_guards) / sizeof(Guard),
    .num_items = sizeof(map_3_map_items) / sizeof(Item),
};
