#ifndef EditBox_HEADER_
#define EditBox_HEADER_
#ifndef UNICODE
#define UNICODE
#endif
#include<windows.h>
#include<vector>


/*
コンストラクタもしくはInitializeで初期化をすること。
引数の無いコンストラクタでは初期化を行いません。

初期化を完了したらCreate関数でエディットボックスを生成します。
使用を終えたら自らDestroy関数を呼ぶこと。
*/
class EditBox{
    HWND m_hWndParent;
    HWND m_hwnd;
    HINSTANCE m_hInstance;
    int m_ctrlID;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    int vk_cache;
public:
    EditBox();
    EditBox(HWND hWndParent, HINSTANCE hInstance, int x, int y, int width, int height, int ctrlID);
    
    void Initialize(HWND hWndParent, HINSTANCE hInstance, int x, int y, int width, int height, int ctrlID);
    void Create();

    // 見つかったときはtrueを返す
    bool GetVK(int& vk);
};

#endif