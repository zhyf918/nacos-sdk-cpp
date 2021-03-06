#ifndef __BEAT_REACTOR_H_
#define __BEAT_REACTOR_H_

#include <map>
#include "src/naming/NamingProxy.h"
#include "naming/Instance.h"
#include "NacosString.h"
#include "NacosExceptions.h"
#include "src/thread/ThreadPool.h"
#include "src/thread/Thread.h"
#include "src/thread/Mutex.h"
#include "BeatTask.h"
#include "constant/ConfigConstant.h"
#include "constant/UtilAndComs.h"
#include "src/factory/ObjectConfigData.h"

namespace nacos{
class BeatReactor {
private:
    ObjectConfigData *_objectConfigData;
    int _threadCount;
    ThreadPool *_threadPool;
    Thread *_beatMaster;
    Mutex _beatInfoLock;//TODO:rwlock
    std::map<NacosString, BeatTask *> _beatInfoList;
    volatile bool _stop;
    volatile long _clientBeatInterval;

    static void *beatMaster(void *param);

public:
    void setClientBeatInterval(long interval) { _clientBeatInterval = interval; };

    long getClientBeatInterval() { return _clientBeatInterval; };

    BeatReactor(ObjectConfigData *objectConfigData, int threadCount)
            : _objectConfigData(objectConfigData), _threadCount(threadCount), _beatInfoLock(), _stop(true),
              _clientBeatInterval(5 * 1000) {
        _threadPool = new ThreadPool("HeartbeatDaemonPool", _threadCount);
        _beatMaster = new Thread("BeatMaster", beatMaster, this);
    };

    BeatReactor(ObjectConfigData *objectConfigData)
            : _objectConfigData(objectConfigData), _threadCount(UtilAndComs::DEFAULT_CLIENT_BEAT_THREAD_COUNT), _beatInfoLock(),
              _stop(true), _clientBeatInterval(5 * 1000) {
        _threadPool = new ThreadPool("HeartbeatDaemonPool", _threadCount);
        _beatMaster = new Thread("BeatMaster", beatMaster, this);
    };

    ~BeatReactor() {
        stop();
        delete _beatMaster;
        delete _threadPool;
        _threadPool = NULL;
        _beatMaster = NULL;
        removeAllBeatInfo();
    };

    void start();

    void stop();

    void addBeatInfo(const NacosString &serviceName, BeatInfo &beatInfo);


    bool modifyBeatInfo(const NacosString &serviceName, BeatInfo &beatInfo);

    bool getBeatInfo(const NacosString &serviceName, const NacosString &ip, int port, BeatInfo &beatInfo);

    bool removeBeatInfo(const NacosString &serviceName, const NacosString &ip, int port);

    //NOTICE:Should be invoked ONLY when the working threads are ALL STOPPED
    void removeAllBeatInfo();

    NacosString buildKey(const NacosString &serviceName, const NacosString &ip, int port);
};
}//namespace nacos

#endif