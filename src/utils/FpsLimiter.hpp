#pragma once

#include <chrono>
#include <list>

// static std::chrono::steady_clock::time_point dbgTmrBegin;
// struct DbgTimer {
//     static void start() {dbgTmrBegin = std::chrono::steady_clock::now();}
//     static void stop(bool print=true) {
//         auto end = std::chrono::steady_clock::now();
//         auto d1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end - dbgTmrBegin).count();
//         if (print) log::info("Timer: {} mks", d1);
//     }
// };


class FpsLimiter {
private:
    std::chrono::nanoseconds m_lastRefresh{};
    std::chrono::nanoseconds m_minDelay{};
    std::list<std::chrono::nanoseconds> m_frameTimes{};
public:
    void setFpsLimit(int maxFps) {
        using namespace std::chrono;
        uint64_t nsIn1Sec = (uint64_t) 1e+9;
        uint64_t oneCycleInNs = nsIn1Sec / maxFps;
        m_minDelay = nanoseconds(oneCycleInNs);
    }
    bool goodToGo() {
        using namespace std::chrono;
        nanoseconds now = steady_clock::now().time_since_epoch();
        return now >= m_lastRefresh + m_minDelay;
    }
    void refresh() {
        using namespace std::chrono;
        nanoseconds now = steady_clock::now().time_since_epoch();
        nanoseconds delta = now - m_lastRefresh;

        m_frameTimes.push_back(delta);
        if (m_frameTimes.size() > 60)
            m_frameTimes.pop_front();

        m_lastRefresh = duration_cast<nanoseconds>(now);
    }
    int getActualRefreshRate() {
        using namespace std::chrono;
        if (m_frameTimes.size() == 0) return 1;
        float sum = 0;
        for (auto delta : m_frameTimes)
            sum += duration<float>(delta).count();
        float avgDelta = sum / m_frameTimes.size();
        float fps = 1.0f / avgDelta;
        return int(fps+0.5);
    }
};
