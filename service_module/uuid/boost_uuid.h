string get_uuid()
{
boost::uuids::random_generator rgen;
boost::uuids::uuid u = rgen();
std::stringstream ss;
ss << u;
string id = ss.str();
//erase all '-'
id.erase(8, 1);
id.erase(12, 1);
id.erase(16, 1);
id.erase(20, 1);
return id;
}


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>