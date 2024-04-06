#include <Windows.h>
#include <stdio.h>
#include <iostream>

using namespace std;

static constexpr const double xwidth = 2560.;
static constexpr const double xheight = 1440.;

constexpr const COLORREF CrossColor = RGB(255, 0, 0);
constexpr const COLORREF ApproxColor = RGB(255, 1, 0);
constexpr const COLORREF TabColor = RGB(255, 124, 0);
constexpr const COLORREF EndColor = RGB(23, 124, 255);


template<typename T>
struct GDIObjectRef
{
    T h;

    constexpr GDIObjectRef(T h) :
        h(h)
    {}

    constexpr operator T() const
    {
        return h;
    }

    GDIObjectRef(const GDIObjectRef&) = delete;
    GDIObjectRef& operator=(const GDIObjectRef&) = delete;

    ~GDIObjectRef()
    {
        if (h != nullptr)
        {
            DeleteObject(h);
            h = nullptr;
        }
    }
};

int    gState = 0;
bool   gEnable = false;
double gSWidth = -1.;
double gSHeight = -1.;

RECT center920(int y, int w, int h, double offset = 0)
{
    const double x = xwidth / 2 - w / 2 + offset;
    const double wt = w / xwidth;
    const double ht = h / xheight;
    const double xt = x / xwidth;
    const double yt = y / xheight;
    return {
        int(xt * gSWidth),
        int(yt * gSHeight),
        int((xt + wt) * gSWidth),
        int((yt + ht) * gSHeight)
    };
}


RECT rect920(int x, int y, int w, int h)
{
    const double wt = w / xwidth;
    const double ht = h / xheight;
    const double xt = x / xwidth;
    const double yt = y / xheight;
    return {
        int(xt * gSWidth),
        int(yt * gSHeight),
        int((xt + wt) * gSWidth),
        int((yt + ht) * gSHeight)
    };
}


void renderCross(HWND hwnd)
{
    PAINTSTRUCT ps;
    GDIObjectRef<HDC> hdc = BeginPaint(hwnd, &ps);
    {
        RECT rect = { 0, 0, (int)gSWidth, (int)gSHeight };
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, brush);
    }
    {
        RECT rect = rect920(2560 / 2 - 3, 1440 / 2 - 3, 6, 6);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(CrossColor);
        FrameRect(hdc, &rect, brush);
    }

    if (!gEnable)
        return; 

    double offset;
    switch (gState)
    {
    case 1:
        offset = xwidth / 5;
        break;
    case 2:
        offset = -xwidth / 5;
        break;
    case 0:
    default:
        offset = 0;
        break;
    }

    {
        RECT rect = rect920(2560 / 2 - 3 + offset, 1440 / 2 - 3, 6, 6);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(CrossColor);
        FrameRect(hdc, &rect, brush);
    }

    {
        RECT rect = center920(768, 740, 10, offset);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(TabColor);
        FrameRect(hdc, &rect, brush);
    }

    //Approx
    {
        RECT rect = center920(828, 570, 10, offset);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(ApproxColor);
        FrameRect(hdc, &rect, brush);
    }

    {
        RECT rect = center920(899, 337, 10, offset);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(TabColor);
        FrameRect(hdc, &rect, brush);
    }
    {
        RECT rect = center920(931, 248, 10, offset);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(TabColor);
        FrameRect(hdc, &rect, brush);
    }

    //Approx
    {
        RECT rect = center920(1174, 142, 10, offset);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(ApproxColor);
        FrameRect(hdc, &rect, brush);
    }

    {
        RECT rect = center920(1418, 92, 10, offset);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(EndColor);
        FrameRect(hdc, &rect, brush);
    }

    {
        RECT rect = rect920(2560 / 2 - 1 + offset, 1440 / 2 - 1 + 15, 2, 800);
        GDIObjectRef<HBRUSH> brush = CreateSolidBrush(RGB(0, 199, 0));
        FillRect(hdc, &rect, brush);
    }


    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    { 
        renderCross(hwnd);
    }
    break;
    case WM_SIZE:
    {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, gSWidth, gSHeight, SWP_SHOWWINDOW);
    }
    break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
};

VOID CALLBACK UpdateProc(HWND hwnd, UINT, UINT_PTR, DWORD)
{
    static bool clickStateA = false;
    static bool clickStateB = false;

    if (GetKeyState(VK_LMENU) & 0x8000)
    {
        if (!clickStateA)
        {
            clickStateA = true;
            gEnable = !gEnable;
            InvalidateRect(hwnd, nullptr, true);
        }
    }
    else
        if (clickStateA)
            clickStateA = false;


    if (GetKeyState(VK_RMENU) & 0x8000)
    {
        if (!clickStateB)
        {
            clickStateB = true;
            gState = ++gState % 3;
            InvalidateRect(hwnd, nullptr, true);
        }
    }
    else
        if (clickStateB)
            clickStateB = false;
}

int main() 
{
    gSWidth = GetSystemMetrics(SM_CXSCREEN);
    gSHeight = GetSystemMetrics(SM_CYSCREEN);

    HINSTANCE hinst = GetModuleHandleW(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hinst;
    wc.lpszClassName = L"SOTHelper";

    RegisterClassW(&wc);

    GDIObjectRef<HWND> hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, L"SOTHelper - helper",
        WS_POPUP & ~WS_BORDER, 0, 0, gSWidth, gSHeight, nullptr, nullptr, hinst, nullptr);

    SetTimer(hwnd, 0, 10, UpdateProc);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_COLORKEY | LWA_ALPHA);
    if (hwnd == nullptr) 
	{
        UnregisterClassW(wc.lpszClassName, hinst);
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {};

    while (GetMessageW(&msg, nullptr, 0, 0)) 
	{
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterClassW(wc.lpszClassName, hinst);
}