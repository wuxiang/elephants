//
// MessagePack for C++ simple buffer implementation
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef MSGPACK_SBUFFER_HPP__
#define MSGPACK_SBUFFER_HPP__
#include <stdlib.h>
#include <string.h>

#include "sbuffer.h"
#include <stdexcept>

#ifndef MSGPACK_SBUFFER_INIT_SIZE
#define MSGPACK_SBUFFER_INIT_SIZE 8192
#endif

namespace msgpack 
{

class msgpack_sbuffer 
{
protected:
    size_t size;
    char* data;
    size_t alloc;

private:
    void msgpack_sbuffer_init();
    void msgpack_sbuffer_destroy();

public:
    int msgpack_sbuffer_write(const char* buf, size_t len);
    void msgpack_sbuffer_clear();
    msgpack_sbuffer();
    ~msgpack_sbuffer();
};

msgpack_sbuffer::msgpack_sbuffer()
{
    msgpack_sbuffer_init();
}

msgpack_sbuffer::~msgpack_sbuffer()
{
    msgpack_sbuffer_destroy();
}


void msgpack_sbuffer::msgpack_sbuffer_init()
{
    if (data)
    {
        ::free(data);
    }
    
    size = 0;
    data = NULL;
    alloc = 0;
}

void msgpack_sbuffer::msgpack_sbuffer_destroy()
{
    size = 0;
    alloc = 0;
    free(data);
}

int msgpack_sbuffer::msgpack_sbuffer_write(const char* buf, size_t len)
{
    if(alloc - size < len) 
    {
        size_t nsize = (alloc) ? alloc * 2 : MSGPACK_SBUFFER_INIT_SIZE;

        while(nsize < size + len) { nsize *= 2; }

        void* tmp = realloc(data, nsize);
        if(!tmp) { return -1; }

        memset((char*)tmp + size, 0, nsize - size);
        data = (char*)tmp;
        alloc = nsize;
    }

    memcpy(data + size, buf, len);
    size += len;
    return 0;
}

void msgpack_sbuffer::msgpack_sbuffer_clear()
{
    size = 0;
    memset(data, 0, size);
}




class sbuffer : public msgpack_sbuffer 
{
private:
    typedef msgpack_sbuffer base;

public:
	sbuffer(size_t initsz = MSGPACK_SBUFFER_INIT_SIZE)
	{
		if(initsz > 0) 
        {
			base::data = (char*)::malloc(initsz);
			if(!base::data) 
            {
				throw std::bad_alloc();
			}
		}

		base::size = 0;
		base::alloc = initsz;
	}

	~sbuffer()
	{
	}

public:
	void write(const char* buf, size_t len)
	{
		if(base::alloc - base::size < len) 
        {
			expand_buffer(len);
		}
		memcpy(base::data + base::size, buf, len);
		base::size += len;
	}

	char* data()
	{
		return base::data;
	}

	const char* data() const
	{
		return base::data;
	}

	size_t size() const
	{
		return base::size;
	}

	void clear()
	{
		msgpack_sbuffer_clear();
	}

private:
	void expand_buffer(size_t len)
	{
		size_t nsize = (base::alloc > 0) ?base::alloc * 2 : MSGPACK_SBUFFER_INIT_SIZE;
	
		while(nsize < base::size + len) { nsize *= 2; }
	
		void* tmp = realloc(base::data, nsize);
		if(!tmp) {
			throw std::bad_alloc();
		}
	
        //memset((char*)tmp + size, 0, nsize - size);
        memset((char*)tmp + base::size, 0, nsize - base::size);
		base::data = (char*)tmp;
		base::alloc = nsize;
	}


private:
	sbuffer(const sbuffer&);
};


}  // namespace msgpack

#endif /* msgpack/sbuffer.hpp */

