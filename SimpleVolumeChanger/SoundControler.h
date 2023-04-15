#ifndef SoundControler_HEADER_
#define SoundControler_HEADER_

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
    CComPtr<IMMDeviceEnumerator> m_pMMDeviceEnu;
public:    
    SoundControler();
    ~SoundControler();

    BOOL SetChannelVolume(float normalizedVol, std::size_t ch);
    float GetChannelNormalizedVolume(UINT ch) const;
    CString DeviceName(UINT ch) const;
    UINT ChannelCount() const;
    UINT GetCurrentCannek() const;
};

#endif
