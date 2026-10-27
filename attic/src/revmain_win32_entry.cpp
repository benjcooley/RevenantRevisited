// *************************************************************************
// *  Retired from src/revmain.cpp during the Subsystem 1 windowing port   *
// *  (Win32 -> sokol_app). These are the blocks that are replaced by      *
// *  sokol_main / init_cb / frame_cb / event_cb / cleanup_cb.             *
// *                                                                       *
// *  Kept here for reference only -- not part of the build.               *
// *************************************************************************

#if 0

// Multi-monitor enumeration. Replaced by sokol_app's display abstraction:
// sokol creates the window on the user's active display; we no longer pick
// a monitor by index from the command line.

bool CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hDC, LPRECT lpRect, LPARAM)
{
    MonitorInfo.cbSize = sizeof(MonitorInfo);
    Monitor = hMonitor;
    GetMonitorInfo(hMonitor, (MONITORINFO*)&MonitorInfo);
    MonitorX = MonitorInfo.rcMonitor.left;
    MonitorY = MonitorInfo.rcMonitor.top;
    MonitorW = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;
    MonitorH = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;

    char *ptr = strstr(MonitorInfo.szDevice, "Display");
    if (!ptr)
        ptr = strstr(MonitorInfo.szDevice, "display");
    if (!ptr)
        ptr = strstr(MonitorInfo.szDevice, "DISPLAY");
    int32_t dispnum = atoi(ptr + 7);

    if (MonitorNum <= 1 && ((dispnum == 1) || (dispnum == 0)))
    {
        if (MonitorNum == 0)
            strcpy(MonitorInfo.szDevice, "display");
        return false;
    }

    if (MonitorNum == dispnum)
        return false;

    return true;
}

bool InitMonitor()
{
    if (MonitorNum > GetSystemMetrics(SM_CMONITORS) || MonitorNum < 0)
        return false;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, nullptr);
    return true;
}

// CPUID-based MMX detection. MMX is irrelevant for the port -- the MMX
// code paths are not shipped and are scheduled for deletion (see
// feedback_mmx_unused). IsMMX is hard-coded false at startup now.
bool HasMMX()
{
    uint32_t version;
    uint32_t features;

    __asm
    {
        mov eax, 1
        cpuid
        mov version, eax
        mov features, edx
    }

    return (features & MMXBIT) != 0;
}

// The Win32 entry point. Replaced by sokol_main() in revmain.cpp.
// sokol_app creates the window for us; we initialize our subsystems in
// init_cb, run one frame per frame_cb, and teardown in cleanup_cb.
int32_t PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int32_t nCmdShow)
{
    IsMMX = HasMMX();

    StartMemory.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&StartMemory);

    ::hInstance = hInstance;

    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    _set_sbh_threshold( MAXSMALLHEAP );
    srand( (unsigned)time( nullptr ) );
    strupr(lpCmdLine);

    GetProgramPaths(lpCmdLine, RunPath, SavePath);
    _chdir(RunPath);
    INISetPath(RunPath);

    uint32_t ver  = GetVersion();
    uint8_t major = LOBYTE(ver);
    if (major <= 3) FatalError("This game requires Windows '95/NT 4.0 (or higher)!", nullptr);

    GetINISettings();
    GetParameters(lpCmdLine);

    PauseMutex = CreateMutex(nullptr, false, nullptr);

    if (!InitMonitor())
        FatalError("Invalid monitor selected", nullptr);

    if (!InitLanguage());

    if (!MainWindow.Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
        FatalError("Couldn't create main window", nullptr);

    if (!InitSystem())
        return 0;

    TScreen* NextScreen = &PlayScreen;
    while (NextScreen)
        NextScreen = TScreen::ShowScreen(NextScreen, 0);

    CloseSystem();
    return 0;
}

#endif
