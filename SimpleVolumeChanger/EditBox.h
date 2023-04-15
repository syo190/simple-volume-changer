#ifndef EditBox_HEADER_
#define EditBox_HEADER_
#include <afxwin.h>
#include<vector>

class CShortcutEditBox : public CEdit {
public:
    CShortcutEditBox() = default;
    ~CShortcutEditBox() = default;

    std::int32_t GetVKey() const { return m_vKey; }
    void SetVKey(std::int32_t vKey);
protected:
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {}
    afx_msg void PreSubclassWindow() override;
    DECLARE_MESSAGE_MAP()
private:
    std::int32_t m_vKey = NULL;
};

#endif