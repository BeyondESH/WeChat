//
// Created by Beyond on 2024/11/27.
//

#ifndef TEST_H
#define TEST_H
#include "const.h"
#include "RedisMgr.h"
#include "Crypto.h"
#include "MySQLMgr.h"
class Test{
public:
    static void testRedisMgr() {
        assert(RedisMgr::getInstance()->set("blogwebsite","beyondesh.top"));
        std::string value = "";
        assert(RedisMgr::getInstance()->get("blogwebsite", value));
        assert(RedisMgr::getInstance()->get("nonekey", value) == false);
        assert(RedisMgr::getInstance()->hSet("bloginfo","blogwebsite", "beyondesh.top"));
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

    static void RedisConnectPool_reconnect() {
        while (!(RedisMgr::getInstance()->set("blogwebsite", "beyondesh.top"))) {
            std::cout << "failed to set" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout<<"succeed to set"<<std::endl;
        std::cout<<RedisMgr::getInstance()->connectCount()<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout<<RedisMgr::getInstance()->connectCount()<<std::endl;
    }
    static void testCrypto(std::string str){
        std::string sha256=Crypto::stringToSha256(str);
        std::cout<<sha256<<std::endl;
    }

    static void testQueryUser(const std::string &user){
        bool result=MySQLMgr::getInstance()->queryUser(user);
        if (result==true) {
            std::cout<<"user is exist"<<std::endl;
        }else {
            std::cout<<"user is nonexistent"<<std::endl;
        }
    }

    static void testQueryEmail(const std::string &email){
        bool result=MySQLMgr::getInstance()->queryEmail(email);
        if (result==true) {
            std::cout<<"email is exist"<<std::endl;
        }else {
            std::cout<<"email is nonexistent"<<std::endl;
        }
    }
};


#endif //TEST_H
