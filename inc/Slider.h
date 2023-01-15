#ifndef Slider_HEADER_
#define Slider_HEADER_

#include<windows.h>
#include<commctrl.h>

class Slider{
    HWND m_hWndParent;
    HINSTANCE m_hInstance;
    HWND m_hSlider;
    int m_ctrlID;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
public:
    Slider(HWND hWndParent, HINSTANCE hInstance, int x, int y, int width, int height, int ctrlID);

    int ID() const;
    HWND GetHandler() const;
};

#endif
