// DisplaySwitcher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

void StartSoundProcess()
{
   // additional information
   STARTUPINFOA si = {0};
   PROCESS_INFORMATION pi = {0};

   // set the size of the structures
   si.cb = sizeof(si);
   //si.dwFlags = STARTF_USESHOWWINDOW;
   //si.wShowWindow = SW_HIDE;
   //si.wShowWindow = FALSE;
   char command_line[] = R"(C:\Windows\System32\cmd.exe /K control mmsys.cpl)";

   // start the program up
   auto ret = CreateProcessA
   (
      NULL,      // the path
      command_line,                // Command line
      NULL,                   // Process handle not inheritable
      NULL,                   // Thread handle not inheritable
      FALSE,                  // Set handle inheritance to FALSE
      CREATE_NO_WINDOW,     // Opens file in a separate console
      NULL,           // Use parent's environment block
      NULL,           // Use parent's starting directory 
      &si,            // Pointer to STARTUPINFO structure
      &pi           // Pointer to PROCESS_INFORMATION structure
   );

   // wait for sound window
   HWND	SoundsDialogHandle;
   while ((SoundsDialogHandle = FindWindow(L"#32770", L"Sound")) == NULL);

   // clean up and close sounds dialog window
   PostMessage(SoundsDialogHandle, WM_CLOSE, NULL, 0);

   TerminateProcess(pi.hProcess, 0);

   CloseHandle(pi.hThread);
   CloseHandle(pi.hProcess);
}

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
            DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_POSITION;
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

            //int result = system("control mmsys.cpl,,0");
            StartSoundProcess();
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

   //StartProcess("control mmsys.cpl");

   ToggleDisplay(3);

   return 0;
}