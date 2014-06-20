#include <iostream>
#include <vector>
#include <string>
#include "sbuffer.hpp"

int main(int argc, char* argv[])
{
    std::vector<std::string>  vec;
    vec.push_back(std::string("hello world"));
    vec.push_back(std::string("welcome"));

    msgpack::sbuffer  buf;

    std::string  value;
    //msgpack::packer<msgpack::sbuffer> packertool(buf);
    //packertool.pack<std::vector<std::string> >(vec);

    std::vector<std::string>  rvec;

    for (std::size_t it = 0; it < rvec.size(); ++it)
    {
        std::cout << rvec[it] << std::endl;
    }

    return 0;
}



