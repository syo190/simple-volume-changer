#include<windows.h>
#include<commctrl.h>

class Slider{
    HWND m_hWndParent;
    HINSTANCE m_hInstance;
    HWND m_hSlider;
    int m_ctrlID;
public:
    Slider(HWND hWndParent, HINSTANCE hInstance, int x, int y, int width, int height, int ctrlID);

    int ID() const;
    HWND GetHandler() const;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
};
