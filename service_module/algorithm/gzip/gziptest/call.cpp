#include "call.h"


Call& Call::instance()
{
    static Call  handler;
    return handler;
}

Call::Call()
{
}

Call::~Call()
{

}

void  Call::message(const msgbus::Message& msg)
{
    std::ifstream  ifs;
    ifs.open("inputtwo.txt", std::ios::in);

    std::string file;
    std::string  ss;

    while (ifs >> ss)
    {
        file += ss;
        file += "\n";
    }

    std::vector<char> packed;
    boost::iostreams::filtering_ostream out;
    out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip::best_compression));
    out.push(boost::iostreams::back_inserter(packed));
    out.write(file.c_str(), file.length());
    boost::iostreams::close(out);

    msgbus::Message rly;
    rly.SetRaw("body", &(packed[0]), packed.size());
    m_connect.send("NCDBrokerExchange", rly);
}

bool Call::init()
{
    name("broker.NCD.repair.package.queue");
    type(msgbus::DeliverDirect);
    m_connect.connect("192.168.1.234");

    return  m_connect.setHandler(this);
}


