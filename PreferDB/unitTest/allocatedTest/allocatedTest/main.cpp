#include <iostream>
#include <stdio.h>
#include "singleton_pool.hpp"
#include "pool_alloc.hpp"
#include "object_pool.hpp"
#include <vector>
#include <string>
#include <map>

class  TestClass
{
public:
    char  tag; //4
    int   serial; // 4
    std::string  desc; // 32
    std::vector<std::string>  res;//20
    std::map<int, std::string>  common; //28

    TestClass(char t, int s, const std::string& d)
    {
        tag = t;
        serial = s;
        desc.assign("i am elephant");
        res.push_back(std::string("hello"));
        res.push_back(std::string(" world"));

        common.insert(std::make_pair(0, "hello"));
        common.insert(std::make_pair(1, " world"));
    }

    TestClass(const TestClass& val)
    {
        tag = val.tag;
        serial = val.serial;
        desc = val.desc;
        res = val.res;
        common = val.common;
    }
};

int main(int argc, char* argv[])
{
    std::cout << Elephants::static_gcd<28,140>::value << std::endl;
    std::cout << Elephants::static_lcm<21,14>::value << std::endl;
    std::cout << sizeof(TestClass) << std::endl;

    Elephants::object_pool<TestClass>  obj_pool;
    TestClass* pTestFromObj = obj_pool.malloc();
    bool res = obj_pool.is_from(pTestFromObj);
    std::string vStr = res ? "true" : "false";
    std::cout << vStr << std::endl;
    obj_pool.free(pTestFromObj);

    Elephants::pool_allocator<TestClass>  poolAllocator;
    TestClass*  pTestFromPool = poolAllocator.allocate(1);
    new (pTestFromPool) TestClass( 'a', 8, std::string("pool_allocator_Test"));

    std::cout << pTestFromPool->tag << std::endl;
    for (std::size_t i = 0; i < pTestFromPool->res.size(); ++i)
    {
        std::cout << (pTestFromPool->res)[i] << std::endl;
    }
    poolAllocator.destroy(pTestFromPool);
    poolAllocator.deallocate(pTestFromPool, 1);

    Elephants::fast_pool_allocator<TestClass> fasterpool;
    TestClass* pFaster = fasterpool.allocate();
    new (pFaster) TestClass( 'b', 88, std::string("pool_allocator_Test"));
    std::cout << pFaster->tag << std::endl;
    for (std::size_t i = 0; i < pFaster->res.size(); ++i)
    {
        std::cout << (pFaster->res)[i] << std::endl;
    }
    fasterpool.destroy(pFaster);
    fasterpool.deallocate(pFaster, 1);

    void* pSingleton = Elephants::singleton_pool<TestClass, sizeof(TestClass)>::malloc();
    fprintf(stderr, "singleton==>%p\n", pSingleton);
    bool resa = Elephants::singleton_pool<TestClass, sizeof(TestClass)>::is_from(pSingleton);
    std::string vStratom = resa ? "true" : "false";
    std::cout << vStratom << std::endl;


    while(1);
    return 0;
}



