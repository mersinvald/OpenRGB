
#pragma once

#include <string>
#include <hidapi/hidapi.h>
#include "RGBController.h"

class MoonlanderController
{
public:
    MoonlanderController(hid_device* dev, char* location) {
        this->dev = dev;
        this->location = location;
    };

    void SetName(const std::string& name) {
        this->name = name;
    }

    void SendColors(unsigned int* keys, unsigned int keys_len, RGBColor* colors);
    void ClearRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    ~MoonlanderController();

private:
    hid_device* dev;
    std::string location;
    std::string name = "Moonlander";
};
