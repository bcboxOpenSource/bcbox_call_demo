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
    *   ��ȡ��ǰ��
    */
    double getElapsedSecond()
    {
        return  getElapsedTimeInMicroSec() * 0.000001;
    }
    /**
    *   ��ȡ����
    */
    double getElapsedTimeInMilliSec()
    {
        return this->getElapsedTimeInMicroSec() * 0.001;
    }
    /**
    *   ��ȡ΢��
    */
    long long getElapsedTimeInMicroSec()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
    }
protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> _begin;
};