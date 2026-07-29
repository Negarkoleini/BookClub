#pragma once
#include <QObject>

// اندازه‌گیری تقریبیِ میزان استفاده از CPU و RAM سیستم برای نمایش در
// داشبورد سرور. مقادیر برگشتی بین 0 تا 100 (درصد) هستند.
// در صورتی که پلتفرم پشتیبانی نشود، مقدار -1 برگردانده می‌شود.
class SystemResourceMonitor {
public:
    SystemResourceMonitor();

    // باید به‌صورت دوره‌ای (مثلا هر ۲-۳ ثانیه) صدا زده شود تا نمونه‌ی
    // جدید بگیرد و دلتای مصرف CPU را نسبت به فراخوانی قبلی محاسبه کند.
    double sampleCpuUsagePercent();
    double currentRamUsagePercent() const;

private:
#if defined(Q_OS_WIN)
    unsigned long long m_prevIdle = 0;
    unsigned long long m_prevKernel = 0;
    unsigned long long m_prevUser = 0;
#elif defined(Q_OS_LINUX)
    unsigned long long m_prevIdle = 0;
    unsigned long long m_prevTotal = 0;
#endif
    bool m_hasPrevSample = false;
};
