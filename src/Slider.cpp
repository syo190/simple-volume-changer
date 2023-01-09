#include"..\inc\Slider.h"
#include"..\inc\ID.h"

Slider::Slider(HWND hWndParent, HINSTANCE hInstance, int x, int y, int width, int height, int ctrlID):
    m_hWndParent(hWndParent), m_hInstance(hInstance), m_x(x), m_y(), m_width(width), m_height(height), m_ctrlID(ctrlID)
{
    // initialize for comctrl dll
    INITCOMMONCONTROLSEX comCtrl = {};
    comCtrl.dwSize = sizeof(INITCOMMONCONTROLSEX);
    comCtrl.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&comCtrl);

    this->m_hSlider = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE,
        this->m_x, this->m_y, this->m_width, this->m_height,
        this->m_hWndParent,
        HMENU(this->m_ctrlID),
        this->m_hInstance,
        NULL
    );
}

HWND Slider::GetHandler() const{
    return this->m_hSlider;
}

int Slider::ID() const{
    return this->m_ctrlID;
}