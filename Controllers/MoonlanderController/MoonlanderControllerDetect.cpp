#include "Detector.h"
#include "MoonlanderController.h"
#include "RGBController.h"
#include "RGBController_Moonlander.h"
#include "SettingsManager.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>

#define VID 0x3297
#define PID 0x1969
#define PAGE 0xFF60
#define USAGE 0x61

/******************************************************************************************\
*                                                                                          *
*   DetectMoonlanderControllers                                                              *
*                                                                                          *
*       Detect devices supported by the Moonlander driver                                    *
*                                                                                          *
\******************************************************************************************/
void DetectMoonlanderControllers(hid_device_info* info, const std::string& name)
{
    if(info->usage != USAGE || info->usage_page != PAGE) return;

    hid_device* dev = hid_open_path(info->path);
    if(dev) {
        MoonlanderController* controller = new MoonlanderController(dev, info->path);
        controller->SetName(name);

		RGBController_Moonlander* rgb_controller = new RGBController_Moonlander(controller);
		ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}   

REGISTER_HID_DETECTOR_PU("Moonlander", DetectMoonlanderControllers, VID, PID, PAGE, USAGE);
