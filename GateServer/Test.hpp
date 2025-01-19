//
// Created by Beyond on 2024/11/27.
//

#ifndef TEST_H
#define TEST_H
#include "const.h"
#include "RedisMgr.h"

class Test : public Singleton<Test> {
    friend class Singleton<Test>;

public:
    void testRedisMgr() {
        assert(RedisMgr::getInstance()->set("blogwebsite","llfc.club"));
        std::string value = "";
        assert(RedisMgr::getInstance()->get("blogwebsite", value));
        assert(RedisMgr::getInstance()->get("nonekey", value) == false);
        assert(RedisMgr::getInstance()->hSet("bloginfo","blogwebsite", "llfc.club"));
        assert(RedisMgr::getInstance()->existsKey("bloginfo"));
        assert(RedisMgr::getInstance()->del("bloginfo"));
        //assert(RedisMgr::getInstance()->del("bloginfo"));
        assert(RedisMgr::getInstance()->existsKey("bloginfo") == false);
        assert(RedisMgr::getInstance()->lPush("lPushkey1", "lPushvalue1"));
        assert(RedisMgr::getInstance()->lPush("lPushkey1", "lPushvalue2"));
        assert(RedisMgr::getInstance()->lPush("lPushkey1", "lPushvalue3"));
        assert(RedisMgr::getInstance()->rPop("lPushkey1", value));
        assert(RedisMgr::getInstance()->rPop("lPushkey1", value));
        assert(RedisMgr::getInstance()->lPop("lPushkey1", value));
        assert(RedisMgr::getInstance()->lPop("lPushkey2", value)==false);
    }

    void RedisConnectPool_reconnect() {
        while (!(RedisMgr::getInstance()->set("blogwebsite", "llfc.club"))) {
            std::cout << "failed to set" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout<<"succeed to set"<<std::endl;
        std::cout<<RedisMgr::getInstance()->connectCount()<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout<<RedisMgr::getInstance()->connectCount()<<std::endl;
    }

private:
    Test(){};

    Test(const Test &) = delete;

    Test &operator=(const Test &) = delete;
};


#endif //TEST_H
