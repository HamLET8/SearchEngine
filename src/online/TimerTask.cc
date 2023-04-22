#include "TimerTask.hh"
#include "CacheManager.hh"

namespace nycpp
{
    void TimerTask::process()
    {
        CacheManager::getInstance()->sync();
    }
}
