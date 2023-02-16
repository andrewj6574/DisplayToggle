// DisplaySwitcher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

void ToggleDisplay(int monitor)
{
   DWORD           DispNum = 0;
   DISPLAY_DEVICE  DisplayDevice;
   TCHAR           szTemp[200];
   int             i = 0;
   DEVMODE   defaultMode;

   // initialize DisplayDevice
   ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
   DisplayDevice.cb = sizeof(DisplayDevice);

   std::wstring monitorToToggle = (L"\\\\.\\DISPLAY") + std::to_wstring(monitor);

   // get all display devices
   while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
   {
      ZeroMemory(&defaultMode, sizeof(DEVMODE));
      defaultMode.dmSize = sizeof(DEVMODE);
      if (!EnumDisplaySettings(DisplayDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &defaultMode))
         OutputDebugString(L"Store default failed\n");

      if (std::wstring(DisplayDevice.DeviceName) == monitorToToggle && !(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
      {
         auto toggle = (DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP);
         if (toggle)
         {
            // turn off
            DEVMODE    DevMode;
            ZeroMemory(&DevMode, sizeof(DevMode));
            DevMode.dmSize = sizeof(DevMode);
            DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_POSITION | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;
            ChangeDisplaySettingsEx(DisplayDevice.DeviceName, &DevMode, NULL, CDS_UPDATEREGISTRY, NULL);
            }
         else
         {
            // turn on
            DEVMODE    DevMode;
            ZeroMemory(&DevMode, sizeof(DevMode));
            DevMode.dmSize = sizeof(DevMode);
            DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
            //DevMode.dmPelsWidth = 3840;
            //DevMode.dmPelsHeight = 2160;
            ChangeDisplaySettingsEx((LPCWSTR)DisplayDevice.DeviceName, &DevMode, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
            ChangeDisplaySettingsEx(NULL, NULL, NULL, 0, NULL);
         }
      }

      // Reinit DisplayDevice just to be extra clean
      ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
      DisplayDevice.cb = sizeof(DisplayDevice);
      DispNum++;
   } // end while for all display devices
}


int main()
{
   // Hide the console window
   HWND hWnd;
   AllocConsole();
   hWnd = FindWindowA("ConsoleWindowClass", NULL);
   ShowWindow(hWnd, 0);

   ToggleDisplay(3);

   return 0;
}