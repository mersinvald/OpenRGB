#include "MoonlanderController.h"
#include "string.h"

MoonlanderController::~MoonlanderController()
{

}

typedef struct rgba {
    uint32_t r : 8;
    uint32_t g : 8;
    uint32_t b : 8;
    uint32_t a : 8;
} rgba;

typedef struct __attribute__ ((__packed__)) key_color {
    uint8_t led;
    rgba color;
} key_color;

typedef struct __attribute__ ((__packed__)) set_key_colors {
    uint8_t length;
    key_color updates[6];
} set_key_colors;

typedef struct __attribute__ ((__packed__)) header {
    uint8_t mid : 4;
    uint8_t sid : 4;
} hid_header_t;

#define PACKET_SIZE 32

void MoonlanderController::SendColors(unsigned int* keys, unsigned int keys_len, RGBColor* colors) {
    uint8_t packet[PACKET_SIZE];

    // setup header
    header h { .mid = 0, .sid = 1 };

    for(unsigned int i = 0; i < keys_len; i += 6) {
       set_key_colors body;
       body.length = std::min(keys_len - i, (uint) 6);

       for(unsigned int j = 0; j < body.length; j++) {
           RGBColor color = colors[i + j];
           body.updates[j] = key_color {
               .led = (uint8_t) keys[i + j],
               .color = rgba {
                   .r = RGBGetRValue(color),
                   .g = RGBGetGValue(color),
                   .b = RGBGetBValue(color),
                   .a = 255,
               }
           };
       }

       memset(&packet, 0, PACKET_SIZE);
       memcpy(&packet, &h, sizeof(header));
       memcpy(&packet[1], &body, sizeof(set_key_colors));

       hid_write(dev, (const unsigned char*) &packet, PACKET_SIZE);
    }
}

void MoonlanderController::ClearRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    uint8_t packet[PACKET_SIZE];

    // setup header
    header h { .mid = 1, .sid = 1 };

    rgba color = {
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    };

    memset(&packet, 0, PACKET_SIZE);
    memcpy(&packet, &h, sizeof(header));
    memcpy(&packet[1], &color, sizeof(rgba));

    hid_write(dev, (const unsigned char*) &packet, PACKET_SIZE);
}
