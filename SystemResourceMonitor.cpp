#include "SystemResourceMonitor.h"

#if defined(Q_OS_WIN)
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include <QFile>
#include <QStringList>
#endif

SystemResourceMonitor::SystemResourceMonitor() = default;

double SystemResourceMonitor::sampleCpuUsagePercent()
{
#if defined(Q_OS_WIN)
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0;
    }

    auto toULL = [](const FILETIME &ft) -> unsigned long long {
        return (static_cast<unsigned long long>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    };

    const unsigned long long idle = toULL(idleTime);
    const unsigned long long kernel = toULL(kernelTime); // شامل زمان idle هم می‌شود
    const unsigned long long user = toULL(userTime);
    const unsigned long long total = kernel + user;

    if (!m_hasPrevSample) {
        m_prevIdle = idle;
        m_prevKernel = kernel;
        m_prevUser = user;
        m_hasPrevSample = true;
        return 0.0;
    }

    const unsigned long long idleDelta = idle - m_prevIdle;
    const unsigned long long totalDelta = total - (m_prevKernel + m_prevUser);

    m_prevIdle = idle;
    m_prevKernel = kernel;
    m_prevUser = user;

    if (totalDelta == 0) return 0.0;
    double usage = 100.0 * (1.0 - (static_cast<double>(idleDelta) / static_cast<double>(totalDelta)));
    if (usage < 0.0) usage = 0.0;
    if (usage > 100.0) usage = 100.0;
    return usage;

#elif defined(Q_OS_LINUX)
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1.0;
    }
    QString firstLine = QString::fromUtf8(file.readLine());
    file.close();

    // فرمت: cpu  user nice system idle iowait irq softirq steal guest guest_nice
    QStringList parts = firstLine.simplified().split(' ');
    if (parts.size() < 5 || parts.at(0) != "cpu") {
        return -1.0;
    }

    unsigned long long total = 0;
    for (int i = 1; i < parts.size(); ++i) {
        total += parts.at(i).toULongLong();
    }
    const unsigned long long idle = parts.at(4).toULongLong();

    if (!m_hasPrevSample) {
        m_prevIdle = idle;
        m_prevTotal = total;
        m_hasPrevSample = true;
        return 0.0;
    }

    const unsigned long long idleDelta = idle - m_prevIdle;
    const unsigned long long totalDelta = total - m_prevTotal;

    m_prevIdle = idle;
    m_prevTotal = total;

    if (totalDelta == 0) return 0.0;
    double usage = 100.0 * (1.0 - (static_cast<double>(idleDelta) / static_cast<double>(totalDelta)));
    if (usage < 0.0) usage = 0.0;
    if (usage > 100.0) usage = 100.0;
    return usage;
#else
    return -1.0;
#endif
}

double SystemResourceMonitor::currentRamUsagePercent() const
{
#if defined(Q_OS_WIN)
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx(&status)) {
        return -1.0;
    }
    return static_cast<double>(status.dwMemoryLoad);

#elif defined(Q_OS_LINUX)
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1.0;
    }

    unsigned long long memTotal = 0, memAvailable = 0;
    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine());
        if (line.startsWith("MemTotal:")) {
            memTotal = line.simplified().split(' ').value(1).toULongLong();
        } else if (line.startsWith("MemAvailable:")) {
            memAvailable = line.simplified().split(' ').value(1).toULongLong();
        }
    }
    file.close();

    if (memTotal == 0) return -1.0;
    double usedPercent = 100.0 * (1.0 - (static_cast<double>(memAvailable) / static_cast<double>(memTotal)));
    if (usedPercent < 0.0) usedPercent = 0.0;
    if (usedPercent > 100.0) usedPercent = 100.0;
    return usedPercent;
#else
    return -1.0;
#endif
}
