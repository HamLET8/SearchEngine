#include "MyTask.hh"
#include "CacheManager.hh"
#include "nlohmann/json.hpp"
#include "fifo_map.hpp"
#include "log4class.hh"
#include "ErrorCheck.hh"

using namespace nlohmann;
template <class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using my_json = basic_json<my_workaround_fifo_map>;
using Json = my_json;

extern __thread size_t __thread_id ;
namespace nycpp
{
void MyTask::process()
{
    logInfo("task process ");

    string response;
    
    if(_msg == "")
    {
        logError("msg is empty.");
        return;
    }

    Json root = json::parse(_msg); 
    size_t msgID = root["msgID"];

    if(1 == msgID)
    {
        string queryWord = root["msg"];
        string key = queryWord;

        auto result = _redis.get(key);

        if(result)
        {
            response = result.value();
            logInfo("word %s hited redis.", queryWord);
        }
        else
        {
            logInfo("redis miss query : %s .", key);
            response = _recommander.query(key);

            _redis.setex(key, 60, response);
            logInfo("query : %s is inserted into redis.", key);        
        }
    }
    else if(2 == msgID)
    {
        string queryWord = root["msg"];
        CacheManager* pCacheManager = CacheManager::getInstance();

        response = pCacheManager->getCacheGroup(__thread_id).getRecord(queryWord);
        
        if(response == "")
        {
            logInfo("LRU miss query : %s .", queryWord);
            response = _webPageSearcher.doQuery(queryWord);
            pCacheManager->getCacheGroup(__thread_id).setRecord(queryWord, response);
            logInfo("query %s: is inserted LRU", queryWord);

        }
        else
        {
            logInfo("LRU has query : %s .", queryWord);
        }
    }
    else
    {
        // error not fix ... 
        logError("msgID: %d is unacceptabel.", msgID);
        
        //respone = {{"msgID", 3}, {"msg", "msgID is unacceptabel."}

        Json root;
        root["msgID"] = 403;

        Json msg = "msgID is unacceptabel: " + _msg ;
        root["msg"] = msg;

        response = root.dump(4);

    }

    _connPtr->notifyLoop(response);

}
}


