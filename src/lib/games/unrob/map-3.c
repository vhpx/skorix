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
        .final_position = {.x = 150, .y = 900},
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
    {
        .id = 2,
        .name = "Item 2",
        .final_position = {.x = 800, .y = 900},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m3_2,
            },
    },
    {
        .id = 3,
        .name = "Item 3",
        .final_position = {.x = 155, .y = 545},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m3_3,
            },
    },
    {
        .id = 4,
        .name = "Item 4",
        .final_position = {.x = 155, .y = 400},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m3_4,
            },
    },
    {
        .id = 5,
        .name = "Item 5",
        .final_position = {.x = 800, .y = 545},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m3_5,
            },
    },
    {
        .id = 6,
        .name = "Item 6",
        .final_position = {.x = 800, .y = 400},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m3_6,
            },
    },
    {
        .id = 7,
        .name = "Item 7",
        .final_position = {.x = 790, .y = 220},
        .entity =
            {
                .position = {.x = 100, .y = 100},
                .size =
                    {
                        .width = GENGINE_ITEM_SIZE,
                        .height = GENGINE_ITEM_SIZE,
                    },
                .sprite = item_m3_7,
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
                .sprite = item_m1_1,
            },
    },
};

const Position map_3_map_wall_boundaries[] = {
    {.x = 435, .y = 998}, {.x = 435, .y = 950}, {.x = 115, .y = 950},
    {.x = 115, .y = 495}, {.x = 200, .y = 495}, {.x = 200, .y = 465},
    {.x = 115, .y = 465}, {.x = 115, .y = 350}, {.x = 215, .y = 350},
    {.x = 215, .y = 280}, {.x = 245, .y = 280}, {.x = 245, .y = 260},
    {.x = 325, .y = 260}, {.x = 325, .y = 235}, {.x = 245, .y = 235},
    {.x = 245, .y = 280}, {.x = 115, .y = 280}, {.x = 115, .y = 150},
    {.x = 360, .y = 150}, {.x = 360, .y = 185}, {.x = 450, .y = 185},
    {.x = 450, .y = 160}, {.x = 465, .y = 160}, {.x = 465, .y = 95},
    {.x = 540, .y = 95},  {.x = 540, .y = 160}, {.x = 555, .y = 160},
    {.x = 555, .y = 185}, {.x = 595, .y = 185}, {.x = 595, .y = 160},
    {.x = 880, .y = 160}, {.x = 880, .y = 290}, {.x = 755, .y = 290},
    {.x = 755, .y = 235}, {.x = 655, .y = 235}, {.x = 655, .y = 260},
    {.x = 755, .y = 260}, {.x = 755, .y = 290}, {.x = 780, .y = 290},
    {.x = 780, .y = 350}, {.x = 880, .y = 350}, {.x = 880, .y = 460},
    {.x = 800, .y = 460}, {.x = 800, .y = 500}, {.x = 880, .y = 500},
    {.x = 880, .y = 700}, {.x = 810, .y = 700}, {.x = 810, .y = 715},
    {.x = 880, .y = 715}, {.x = 880, .y = 950}, {.x = 560, .y = 950},
    {.x = 560, .y = 998}, {.x = 435, .y = 998},
};

const Position map_3_bottom_left_set_boundaries[] = {
    {.x = 230, .y = 840},
    {.x = 230, .y = 815},
    {.x = 315, .y = 815},
    {.x = 315, .y = 840},
};

const Position map_3_bottom_right_set_boundaries[] = {
    {.x = 650, .y = 840},
    {.x = 650, .y = 815},
    {.x = 780, .y = 815},
    {.x = 780, .y = 840},
};

const Position map_3_bottom_center_set_boundaries[] = {
    {.x = 465, .y = 770},
    {.x = 465, .y = 710},
    {.x = 535, .y = 710},
    {.x = 535, .y = 770},
};

const Position map_3_center_left_set_boundaries[] = {
    {.x = 225, .y = 610},
    {.x = 225, .y = 580},
    {.x = 315, .y = 580},
    {.x = 315, .y = 610},
};

const Position map_3_center_right_set_boundaries[] = {
    {.x = 720, .y = 610},
    {.x = 720, .y = 560},
    {.x = 780, .y = 560},
    {.x = 780, .y = 610},
};

const Position map_3_top_left_set_boundaries[] = {
    {.x = 315, .y = 400}, {.x = 315, .y = 380}, {.x = 445, .y = 380},
    {.x = 445, .y = 430}, {.x = 410, .y = 430}, {.x = 410, .y = 400},
};

const Position map_3_top_right_set_boundaries[] = {
    {.x = 685, .y = 400}, {.x = 685, .y = 380}, {.x = 550, .y = 380},
    {.x = 550, .y = 430}, {.x = 585, .y = 430}, {.x = 585, .y = 400},
};

const Boundary map_3_map_boundaries[] = {
    {
        .positions = map_3_map_wall_boundaries,
        .num_positions = sizeof(map_3_map_wall_boundaries) / sizeof(Position),
    },
    {
        .positions = map_3_bottom_left_set_boundaries,
        .num_positions =
            sizeof(map_3_bottom_left_set_boundaries) / sizeof(Position),
    },
    {
        .positions = map_3_bottom_right_set_boundaries,
        .num_positions =
            sizeof(map_3_bottom_right_set_boundaries) / sizeof(Position),
    },
    {
        .positions = map_3_bottom_center_set_boundaries,
        .num_positions =
            sizeof(map_3_bottom_center_set_boundaries) / sizeof(Position),
    },
    {
        .positions = map_3_center_left_set_boundaries,
        .num_positions =
            sizeof(map_3_center_left_set_boundaries) / sizeof(Position),
    },
    {
        .positions = map_3_center_right_set_boundaries,
        .num_positions =
            sizeof(map_3_center_right_set_boundaries) / sizeof(Position),
    },
    {
        .positions = map_3_top_left_set_boundaries,
        .num_positions =
            sizeof(map_3_top_left_set_boundaries) / sizeof(Position),
    },
    {
        .positions = map_3_top_right_set_boundaries,
        .num_positions =
            sizeof(map_3_top_right_set_boundaries) / sizeof(Position),
    },
};

Guard map_3_guards[] = {

};

GameMap map3 = {
    .spawn_point = map_3_spawn_point,
    .player =
        {
            .position = map_3_spawn_point,
            .size = {.width = PLAYER_WIDTH, .height = PLAYER_HEIGHT},
        },
    .boundaries = map_3_map_boundaries,
    .bitmap = game_map_3_bitmap,
    .size = map_3_map_size,
    .items = map_3_map_items,
    .guards = map_3_guards,
    .num_boundaries = sizeof(map_3_map_boundaries) / sizeof(Boundary),
    .num_guards = sizeof(map_3_guards) / sizeof(Guard),
    .num_items = sizeof(map_3_map_items) / sizeof(Item),
};
