/*-----------------------------------------*\
|  RGBController_Moonlander.cpp              |
|                                           |
|  Generic RGB Interface for Linux LED      |
|                                           |
|  Mike Lubinets (mersinvald) 6/1/2021      |
\*-----------------------------------------*/

#include "RGBController_Moonlander.h"


//0xFFFFFFFF indicates an unused entry in matrix
#define NA 0xFFFFFFFF
#define RGB_RAW_BUFFER_SIZE 96

#define MATRIX_WIHTH 16
#define MATRIX_HEIGTH 8
#define MATRIX_SIZE MATRIX_WIHTH * MATRIX_HEIGTH

// Matrix for Moonlander
static unsigned int moonlander[MATRIX_HEIGTH][MATRIX_WIHTH] =
{
    {0,  5,  10, 15, 20, 25, 29, NA, NA, 65, 61, 56, 51, 46, 41, 36}, 
    {1,  6,  11, 16, 21, 26, 30, NA, NA, 66, 62, 57, 52, 47, 42, 37}, 
    {2,  7,  12, 17, 22, 27, 31, NA, NA, 67, 63, 58, 53, 48, 43, 38}, 
    {3,  8,  13, 18, 23, 28, NA, NA, NA, NA, 64, 59, 54, 49, 44, 39}, 
    {4,  9,  14, 19, 24, NA, NA, NA, NA, NA, NA, 60, 55, 50, 45, 40}, 
    {NA, NA, NA, NA, NA, 32, 35, NA, 71, 68, NA, NA, NA, NA, NA, NA}, 
    {NA, NA, NA, NA, NA, 33, NA, NA, NA, 69, NA, NA, NA, NA, NA, NA}, 
    {NA, NA, NA, NA, NA, 34, NA, NA, NA, 70, NA, NA, NA, NA, NA, NA}, 
};

static unsigned int matrix_map[MATRIX_HEIGTH][MATRIX_WIHTH] = {{0}};

static unsigned int matrix_to_moonlander[MATRIX_SIZE] = {0};

static const char *zone_names[] =
{
    "Keyboard"
};

static zone_type zone_types[] =
{
    ZONE_TYPE_MATRIX,
};

static const unsigned int zone_sizes[] =
{
    MATRIX_SIZE
};


void setup_matrix_map() {
    int led = 0;

    for (int w = 0; w < MATRIX_WIHTH; w++) {
        for (int h = 0; h < MATRIX_HEIGTH; h++) {
            matrix_map[h][w] = led;
            matrix_to_moonlander[led] = moonlander[h][w];
            led += 1;

            unsigned int moonlander_key_id = moonlander[h][w];
            if (moonlander_key_id == NA) {
                matrix_map[h][w] = NA; 
            }
        }    
    }
}

RGBController_Moonlander::RGBController_Moonlander(MoonlanderController* controller_ptr)
{
    controller = controller_ptr;

    name        = "Moonlander MK1";
    type        = DEVICE_TYPE_KEYBOARD;
    description = "Moonlander MK1 (RAWHID)";

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0xFFFF;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);
    setup_matrix_map();
    SetupZones();
}

RGBController_Moonlander::~RGBController_Moonlander()
{
    controller->ClearRGBA(0, 0, 0, 0);
    delete controller;
}

void RGBController_Moonlander::SetupZones()
{
     /*---------------------------------------------------------*\
    | Set up zones                                              |
    \*---------------------------------------------------------*/
    unsigned int total_led_count = 0;

    for (unsigned int zone_idx = 0; zone_idx < 1; zone_idx++)
    {
        zone new_zone;

        new_zone.name               = zone_names[zone_idx];
        new_zone.type               = zone_types[zone_idx];
        new_zone.leds_min           = zone_sizes[zone_idx];
        new_zone.leds_max           = zone_sizes[zone_idx];
        new_zone.leds_count         = zone_sizes[zone_idx];
        new_zone.matrix_map         = new matrix_map_type;
        new_zone.matrix_map->height = MATRIX_HEIGTH;
        new_zone.matrix_map->width  = MATRIX_WIHTH;
        new_zone.matrix_map->map    = (unsigned int *)&matrix_map;

        zones.push_back(new_zone);

        total_led_count += zone_sizes[zone_idx];
    }

    for (unsigned int led_idx = 0; led_idx < total_led_count; led_idx++)
    {
        led new_led;

        uint m_led_idx = matrix_to_moonlander[led_idx];

        if(m_led_idx == NA) {
            new_led.name = "Unused";
        } else {
            new_led.name = "Keyboard LED ";
            new_led.name.append(std::to_string(m_led_idx));
        }

        leds.push_back(new_led);
    }

    controller->ClearRGBA(0, 0, 0, 0);

    SetupColors();

    prev_colors = colors;
}


void RGBController_Moonlander::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_Moonlander::DeviceUpdateLEDs()
{
    unsigned int diff[MATRIX_SIZE];
    RGBColor diff_colors[MATRIX_SIZE];
    unsigned int diff_size = 0;

    for(unsigned int i = 0; i < colors.size(); i++) {
        if(matrix_to_moonlander[i] == NA) {
            continue;
        }

        if(prev_colors[i] != colors[i]) {
            diff[diff_size] = matrix_to_moonlander[i];
            diff_colors[diff_size++] = colors[i];
        }
    }

    controller->SendColors(diff, diff_size, diff_colors);

    std::copy(colors.begin(), colors.end(), prev_colors.begin());
}

void RGBController_Moonlander::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_Moonlander::UpdateSingleLED(int /*led*/)
{
    
}

void RGBController_Moonlander::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_Moonlander::DeviceUpdateMode()
{
}
