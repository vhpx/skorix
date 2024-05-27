#include "map-2.h"

const Position map_2_spawn_point = {
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

const Size map_2_map_size = {
    .width = SCREEN_WIDTH,
    .height = SCREEN_HEIGHT,
};

Item map_2_map_items[] = {
    {
        .id = 1,
        .name = "Item 1",
        .final_position = {.x = 145, .y = 230},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m2_1,
            },
    },
    {
        .id = 2,
        .name = "Item 2",
        .final_position = {.x = 270, .y = 230},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m2_2,
            },
    },
    {
        .id = 3,
        .name = "Item 3",
        .final_position = {.x = 865, .y = 315},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m2_3,
            },
    },
    {
        .id = 4,
        .name = "Item 4",
        .final_position = {.x = 865, .y = 710},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m2_4,
            },
    },
    {
        .id = 5,
        .name = "Item 5",
        .final_position = {.x = 640, .y = 930},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m2_5,
            },
    },
    {
        .id = 6,
        .name = "Item 6",
        .final_position = {.x = 90, .y = 925},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m2_6,
            },
    },
    {
        .id = 7,
        .name = "Item 7",
        .final_position = {.x = 90, .y = 575},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m2_7,
            },
    },
    {
        .id = 8,
        .name = "Item 8 (Outlier)",
        .final_position = {.x = -1, .y = -1},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m3_1,
            },
    },
};

const Position map_2_map_wall_boundaries[] = {
    {.x = 440, .y = 998}, {.x = 440, .y = 925}, {.x = 375, .y = 925},
    {.x = 375, .y = 975}, {.x = 60, .y = 975},  {.x = 60, .y = 795},
    {.x = 105, .y = 795}, {.x = 105, .y = 765}, {.x = 105, .y = 765},
    {.x = 60, .y = 765},  {.x = 60, .y = 675},  {.x = 120, .y = 675},
    {.x = 120, .y = 650}, {.x = 60, .y = 650},  {.x = 60, .y = 545},
    {.x = 120, .y = 545}, {.x = 120, .y = 490}, {.x = 60, .y = 490},
    {.x = 60, .y = 380},  {.x = 100, .y = 380}, {.x = 100, .y = 360},
    {.x = 60, .y = 360},  {.x = 60, .y = 270},  {.x = 120, .y = 270},
    {.x = 120, .y = 250}, {.x = 60, .y = 250},  {.x = 60, .y = 180},
    {.x = 215, .y = 180}, {.x = 215, .y = 240}, {.x = 250, .y = 240},
    {.x = 250, .y = 180}, {.x = 330, .y = 180}, {.x = 330, .y = 240},
    {.x = 360, .y = 240}, {.x = 360, .y = 195}, {.x = 360, .y = 195},
    {.x = 400, .y = 195}, {.x = 400, .y = 230}, {.x = 450, .y = 230},
    {.x = 450, .y = 180}, {.x = 535, .y = 180}, {.x = 535, .y = 235},
    {.x = 605, .y = 235}, {.x = 605, .y = 190}, {.x = 695, .y = 190},
    {.x = 695, .y = 250}, {.x = 880, .y = 250}, {.x = 880, .y = 275},
    {.x = 930, .y = 275}, {.x = 930, .y = 360}, {.x = 930, .y = 360},
    {.x = 900, .y = 360}, {.x = 900, .y = 390}, {.x = 930, .y = 390},
    {.x = 930, .y = 625}, {.x = 885, .y = 625}, {.x = 885, .y = 670},
    {.x = 930, .y = 670}, {.x = 930, .y = 760}, {.x = 930, .y = 760},
    {.x = 895, .y = 760}, {.x = 895, .y = 780}, {.x = 930, .y = 780},
    {.x = 930, .y = 840}, {.x = 750, .y = 840}, {.x = 750, .y = 980},
    {.x = 615, .y = 980}, {.x = 615, .y = 915}, {.x = 635, .y = 915},
    {.x = 635, .y = 880}, {.x = 690, .y = 880}, {.x = 690, .y = 850},
    {.x = 635, .y = 850}, {.x = 635, .y = 915}, {.x = 635, .y = 915},
    {.x = 550, .y = 915}, {.x = 550, .y = 998}, {.x = 440, .y = 998},
};

const Boundary map_2_map_boundaries[] = {
    {.positions = map_2_map_wall_boundaries,
     .num_positions = sizeof(map_2_map_wall_boundaries) / sizeof(Position)},
};

Guard map_2_guards[] = {

};

GameMap map2 = {
    .spawn_point = map_2_spawn_point,
    .player =
        {
            .position = map_2_spawn_point,
            .size = {.width = PLAYER_WIDTH, .height = PLAYER_HEIGHT},
        },
    .boundaries = map_2_map_boundaries,
    .bitmap = game_map_2_bitmap,
    .size = map_2_map_size,
    .items = map_2_map_items,
    .guards = map_2_guards,
    .num_boundaries = sizeof(map_2_map_boundaries) / sizeof(Boundary),
    .num_guards = sizeof(map_2_guards) / sizeof(Guard),
    .num_items = sizeof(map_2_map_items) / sizeof(Item),
};
