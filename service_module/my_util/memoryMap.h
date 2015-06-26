#ifndef BROKER_UTIL_
#define BROKER_UTIL_
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

const std::size_t  FILELENGTH=4096;

struct VersionSequence
{
	// situation: recv a message, but failed fanout it out
	std::string ff;
	boost::shared_ptr<boost::interprocess::file_mapping>  m_file;
	boost::shared_ptr<boost::interprocess::mapped_region>  m_region;

	char*  m_version;

	VersionSequence(const std::string& fileName)
	{
		if (fileName.empty())
		{
			throw std::runtime_error("VersionSequence file name is empty");
		}
		ff = fileName;

		// if file is not exist, create it
		const std::size_t filesize = boost::interprocess::mapped_region::get_page_size();
		if (!boost::filesystem::exists(ff))
		{
			std::filebuf  fbuf;
			fbuf.open(ff.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
			fbuf.pubseekoff(filesize - 1, std::ios_base::beg);
			fbuf.sputc(0);
		}

		// map file
		m_file.reset(new (std::nothrow) boost::interprocess::file_mapping(ff.c_str(), boost::interprocess::read_write));
		m_region.reset(new (std::nothrow) boost::interprocess::mapped_region(*m_file, boost::interprocess::read_write));

		void* addr = m_region->get_address();
		std::size_t sz = m_region->get_size();
		if (sz < FILELENGTH)
		{
			throw std::runtime_error("map File size less than 4 * unit64_t");
		}

		m_version = (char*)addr;
	}

	std::string getversion()
	{
		boost::interprocess::file_lock     f_lock(ff.c_str());
		boost::interprocess::scoped_lock<boost::interprocess::file_lock>   uq_lock(f_lock);

		char* pHeader = m_version;
		std::string version;
		while (pHeader && *pHeader >= '0' && *pHeader <= '9')
		{
			version += *pHeader++;
		}
		return version;
	}

	void clearData()
	{
		boost::interprocess::file_lock     f_lock(ff.c_str());
		boost::interprocess::scoped_lock<boost::interprocess::file_lock>   uq_lock(f_lock);
		std::memset((void*)m_version, 0, FILELENGTH);
	}

	void setVersion(const std::string& ver)
	{
		boost::interprocess::file_lock     f_lock(ff.c_str());
		boost::interprocess::scoped_lock<boost::interprocess::file_lock>   uq_lock(f_lock);

		std::strncpy(m_version, ver.c_str(), ver.length());
		std::memset(m_version + ver.length(), 0, FILELENGTH - ver.length());

		m_region->flush(0, FILELENGTH);
	}
};

class BrokerUtil
{
public:
    static std::string  convertDealTime(const std::string& logtime);
    static std::string  converDate(const std::string& logtime);
};

#endif


