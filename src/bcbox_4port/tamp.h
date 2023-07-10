#include <chrono>
#include <string>

class CELLTimestamp
{
public:
    CELLTimestamp()
    {
        update();
    }
    ~CELLTimestamp()
    {}

    void  update()
    {
        //QueryPerformanceCounter(&_startCount);
        _begin =std::chrono::high_resolution_clock::now();
    }
    /**
    *   获取当前秒
    */
    double getElapsedSecond()
    {
        return  getElapsedTimeInMicroSec() * 0.000001;
    }
    /**
    *   获取毫秒
    */
    double getElapsedTimeInMilliSec()
    {
        return this->getElapsedTimeInMicroSec() * 0.001;
    }
    /**
    *   获取微妙
    */
    long long getElapsedTimeInMicroSec()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
    }
protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> _begin;
};