#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp> 
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>  
#include <boost/iostreams/device/file.hpp>

#include <Windows.h>

#include "msgbus/connection.h"
#include "call.h"


//struct My_InOut{
//    typedef char char_type;
//    typedef boost::iostreams::seekable_device_tag category;
//
//    My_InOut(char*Buf, std::streamsize size)
//        :m_pCur(Buf),m_pBuf(Buf),m_size(size), m_used(0){;}
//
//    std::streamsize read(char_type* s, std::streamsize n)
//    {
//        //��min��ѡ��n����ʣ���С�Է�ֹ���
//        std::streamsize nCount = std::min(n, (m_pCur-m_pBuf));
//        if(nCount)
//        {
//            memcpy(s,m_pBuf,nCount);
//        }
//        return nCount;
//    }
//
//    std::streamsize write(const char_type* s, std::streamsize n)
//    {
//        std::streamsize nCount = std::min(n,m_size - (m_pCur-m_pBuf));
//        if(nCount)
//        {
//            memcpy(m_pCur,s,nCount);
//            m_pCur += nCount;
//            m_used += nCount;
//        }
//        return nCount;
//    }
//
//    boost::iostreams::stream_offset seek(boost::iostreams::stream_offset off, std::ios_base::seekdir way)
//    {
//        // ��λ
//        char *pNewPos;
//        switch(way)
//        {
//        case std::ios_base::beg:
//            pNewPos = m_pBuf;
//            break;
//        case std::ios_base::cur:
//            pNewPos = m_pCur;
//            break;
//        case std::ios_base::end:
//            pNewPos = m_pBuf + m_size;
//            break;
//        }
//
//        pNewPos += off;
//        if(pNewPos < m_pBuf || pNewPos - m_pBuf > m_size)
//            throw std::ios_base::failure("bad seek offset");
//
//        m_pCur = pNewPos;
//        return m_pCur - m_pBuf;     
//    }
//
//    std::streamsize getsize()
//    {
//        return m_used;
//    }
//private:
//    char *m_pCur;
//    char *m_pBuf;
//    std::streamsize m_size;
//    std::streamsize m_used;
//};


int main()
{
    //msgbus::Connection  conne;
    //conne.connect("192.168.1.234");
    //msgbus::Message  repairMsg;
    //repairMsg.SetString("version",  "-100");
    //repairMsg.SetReplyTo("NCDBrokerExchange");
    //conne.send("complement.quota.vesion.queue", repairMsg);
    //return 0;

    //if (!Call::instance().init())
    //{
    //    return -1;
    //}

    msgbus::Connection conn;
    conn.connect("192.168.1.234");

    std::ifstream  ifs;
    ifs.open("input.txt", std::ios::in);

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

    msgbus::Message msg;
    msg.SetRaw("body", &(packed[0]), packed.size());
    conn.send("broker.NCD.quota.queue", msg);

    std::vector<char> unpacked;

    boost::iostreams::filtering_ostream dec;
    dec.push(boost::iostreams::gzip_decompressor());
    dec.push(boost::iostreams::back_inserter(unpacked));
    dec.write(&packed[0], packed.size());
    boost::iostreams::close(dec);

    std::string  val(&unpacked[0], unpacked.size());
    std::cerr << val << std::endl;

    //char buffer[8192] = {0};
    //My_InOut  tbuff(buffer, 8192);
    //// ����filtering_ostream��ϳ�һ��bzip2��ѹ�������
    //boost::iostreams::filtering_ostream out;
    //out.push(boost::iostreams::gzip_compressor());  //��ѹ��
    //out.push(tbuff); //�ٴ����ļ���ע��˳���
    //out << file;      // ���10��Hello World!
    //msgbus::VariantRaw raw;
    //raw.Data = buffer;
    //raw.Length = tbuff.getsize();
    ////conn.send()
    //// �����ý�ѹ�����ѹhello.bz2���ü��±���ȷ��һ���Ƿ���ȷѹ��
    //

    //// �Ѹղŵ�bz2ѹ���ļ�������
    //boost::iostreams::filtering_istream in;
    //in.push(boost::iostreams::gzip_decompressor());  //�ٽ�ѹ��ע��˳������������Ƿ����
    //in.push(My_InOut(buffer,8192));  //�ȴ��ļ������

    //std::string s;
    //for (int i = 0; i < 10; ++i)
    //{
    //    getline(in,s);
    //    std::cout << s << std::endl;
    //}
    ////��ȷ���10��Hello World!
    //while (true)
    //{
    //    Sleep(10);
    //}
    
    return 0;
}


