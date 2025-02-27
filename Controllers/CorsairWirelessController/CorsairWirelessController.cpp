/*-----------------------------------------*\
|  CorsairWirelessController.cpp            |
|                                           |
|  Driver for Corsair RGB wireless keyboard |
|  lighting controller                      |
|                                           |
|  Adam Honse (CalcProgrammer1) 5/8/2021    |
\*-----------------------------------------*/

#include "CorsairWirelessController.h"

#include <cstring>

using namespace std::chrono_literals;

CorsairWirelessController::CorsairWirelessController(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;

    type        = DEVICE_TYPE_KEYBOARD;

    EnterDirectMode();
}

CorsairWirelessController::~CorsairWirelessController()
{
    hid_close(dev);
}

device_type CorsairWirelessController::GetDeviceType()
{
    return type;
}

std::string CorsairWirelessController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string CorsairWirelessController::GetFirmwareString()
{
    return firmware_version;
}

std::string CorsairWirelessController::GetName()
{
    return name;
}

std::string CorsairWirelessController::GetSerialString()
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

void CorsairWirelessController::SetLEDs(std::vector<RGBColor>colors)
{
    unsigned char buf[3 * 137];

    for(int color = 0; color < 137; color++)
    {
        buf[0 + color]   = RGBGetRValue(colors[color]);
        buf[137 + color] = RGBGetGValue(colors[color]);
        buf[274+color]   = RGBGetBValue(colors[color]);
    }

    StartDirectMode();

    SendDirectFrame(true,  &buf[0]);
    SendDirectFrame(false, &buf[57]);
    SendDirectFrame(false, &buf[118]);
    SendDirectFrame(false, &buf[179]);
    SendDirectFrame(false, &buf[240]);
    SendDirectFrame(false, &buf[301]);
    SendDirectFrame(false, &buf[362]);
}

void CorsairWirelessController::SetLEDsKeyboardFull(std::vector<RGBColor> colors)
{
}

void CorsairWirelessController::SetName(std::string device_name)
{
    name = device_name;
}

/*-------------------------------------------------------------------------------------------------*\
| Private packet sending functions.                                                                 |
\*-------------------------------------------------------------------------------------------------*/

void CorsairWirelessController::EnterDirectMode()
{
    char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Submit Mouse Colors packet                     |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = 0x00;
    usb_buf[0x01]   = 0x80;
    usb_buf[0x02]   = 0x01;
    usb_buf[0x03]   = 0x03;
    usb_buf[0x04]   = 0x00;
    usb_buf[0x05]   = 0x02;

    /*-----------------------------------------------------*\
    | Send packet using feature reports, as headset stand   |
    | seems to not update completely using HID writes       |
    \*-----------------------------------------------------*/
    hid_write(dev, (unsigned char *)usb_buf, 65);

}

void CorsairWirelessController::StartDirectMode()
{
    char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Submit Mouse Colors packet                     |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = 0x00;
    usb_buf[0x01]   = 0x80;
    usb_buf[0x02]   = 0x0D;
    usb_buf[0x03]   = 0x00;
    usb_buf[0x04]   = 0x01;

    /*-----------------------------------------------------*\
    | Send packet using feature reports, as headset stand   |
    | seems to not update completely using HID writes       |
    \*-----------------------------------------------------*/
    hid_write(dev, (unsigned char *)usb_buf, 65);

}

void CorsairWirelessController::ExitDirectMode()
{
    char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Submit Mouse Colors packet                     |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = 0x00;
    usb_buf[0x01]   = 0x80;
    usb_buf[0x02]   = 0x01;
    usb_buf[0x03]   = 0x03;
    usb_buf[0x04]   = 0x00;
    usb_buf[0x05]   = 0x01;

    /*-----------------------------------------------------*\
    | Send packet using feature reports, as headset stand   |
    | seems to not update completely using HID writes       |
    \*-----------------------------------------------------*/
    hid_write(dev, (unsigned char *)usb_buf, 65);

}

void CorsairWirelessController::SendDirectFrame(bool first_frame, unsigned char* data)
{
    char usb_buf[65];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Submit Mouse Colors packet                     |
    \*-----------------------------------------------------*/
    usb_buf[0x00]   = 0x00;
    usb_buf[0x01]   = 0x80;
    usb_buf[0x02]   = first_frame ? 0x06 : 0x07;
    usb_buf[0x03]   = 0x00;

    if(first_frame)
    {
        usb_buf[0x04]   = 0x9B;
        usb_buf[0x05]   = 0x01;
    }

    /*-----------------------------------------------------*\
    | Copy in colors in <RED> <GREEN> <BLUE> order          |
    \*-----------------------------------------------------*/
    if(first_frame)
    {
        memcpy(&usb_buf[0x08], data, 57);
    }
    else
    {
        memcpy(&usb_buf[0x04], data, 61);
    }

    /*-----------------------------------------------------*\
    | Send packet using feature reports, as headset stand   |
    | seems to not update completely using HID writes       |
    \*-----------------------------------------------------*/
    hid_write(dev, (unsigned char *)usb_buf, 65);
}
