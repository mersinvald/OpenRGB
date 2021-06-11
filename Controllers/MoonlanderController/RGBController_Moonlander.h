/*-----------------------------------------*\
|  RGBController_LinuxLED.h                 |
|                                           |
|  Generic RGB Interface for Linux LED      |
|                                           |
|  Adam Honse (CalcProgrammer1) 9/25/2020   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "MoonlanderController.h"

class RGBController_Moonlander : public RGBController
{
public:
    RGBController_Moonlander(MoonlanderController* controller_ptr);
    ~RGBController_Moonlander();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);
    
    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    MoonlanderController* controller;
    std::vector<RGBColor> prev_colors;
};
