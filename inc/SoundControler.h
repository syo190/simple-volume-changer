#include<windows.h>
#include<endpointvolume.h>
#include<mmdeviceapi.h>
#include<atlbase.h>
#include<vector>
#include<atlstr.h>

class SoundControler{
    UINT m_ChannelCnt;
    std::vector<PROPVARIANT> m_DeviceInfo;
    std::vector<CComPtr<IAudioEndpointVolume>> m_aepVolume;
public:    
    SoundControler();
    ~SoundControler();

    BOOL SetChannelVolume(float normalizedVol, UINT ch);
    
    float GetChannelNormalizedVolume(UINT ch) const;
    CString DeviceName(UINT ch) const;
    UINT ChannelCount() const;
};
