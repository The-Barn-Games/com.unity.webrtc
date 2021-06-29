#include "pch.h"
#include "DummyAudioDevice.h"
#include "system_wrappers/include/sleep.h"

namespace unity
{
namespace webrtc
{

    bool DummyAudioDevice::RecThreadProcess()
    {
        int64_t currentTime = rtc::TimeMillis();

        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (audio_transport_ == nullptr) {
                return false;
            }

            if (!isRecording) {
                return false;
            }

            if (_lastCallRecordMillis == 0 || currentTime - _lastCallRecordMillis >= 10) {
                _lastCallRecordMillis = currentTime;

                const int kBytesPerSample = 2;
                const int kChannels = 2;

                const int kSamplingRate = 48000;

                // Webrtc uses 10ms frames.
                const int kFrameLengthMs = 10;
                const int kSamplesPerFrame = kSamplingRate * kFrameLengthMs / 1000;

                int64_t elapsed_time_ms = -1;
                int64_t ntp_time_ms = -1;
                char data[kBytesPerSample * kChannels * kSamplesPerFrame];

                audio_transport_->PullRenderData(kBytesPerSample * 8, kSamplingRate,
                    kChannels, kSamplesPerFrame, data,
                    &elapsed_time_ms, &ntp_time_ms);
            }
        }

        int64_t deltaTimeMillis = rtc::TimeMillis() - currentTime;
        if (deltaTimeMillis < 10) {
            SleepMs(10 - deltaTimeMillis);
        }
        return true;
    }

} // end namespace webrtc
} // end namespace unity
