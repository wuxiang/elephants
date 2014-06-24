//
// MessagePack for C++ deflate buffer implementation
//
// Copyright (C) 2010 FURUHASHI Sadayuki
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
#ifndef MSGPACK_ZBUFFER_HPP__
#define MSGPACK_ZBUFFER_HPP__

#include "zbuffer.h"
#include <stdexcept>

#ifndef MSGPACK_ZBUFFER_INIT_SIZE
#define MSGPACK_ZBUFFER_INIT_SIZE 8192
#endif

#ifndef MSGPACK_ZBUFFER_RESERVE_SIZE
#define MSGPACK_ZBUFFER_RESERVE_SIZE 512
#endif



namespace msgpack 
{
class msgpack_zbuffer 
{
public:
    z_stream stream;
    char* data;
    size_t init_size;

public:
    bool msgpack_zbuffer_init(int level, size_t init_size);
    void msgpack_zbuffer_destroy();

    char* msgpack_zbuffer_flush();

    const char* msgpack_zbuffer_data();
    size_t msgpack_zbuffer_size();

    bool msgpack_zbuffer_reset();
    void msgpack_zbuffer_reset_buffer();
    char* msgpack_zbuffer_release_buffer();
    int msgpack_zbuffer_write(const char* buf, size_t len);

    bool msgpack_zbuffer_expand();


};

bool msgpack_zbuffer::msgpack_zbuffer_init(int level, size_t init_size)
{
    init_size = init_size;
    if(deflateInit(&stream, level) != Z_OK) 
    {
        free(data);
        return false;
    }
    return true;
}

void msgpack_zbuffer::msgpack_zbuffer_destroy()
{
    deflateEnd(&stream);
    free(data);
}

bool msgpack_zbuffer::msgpack_zbuffer_expand()
{
    size_t used = (char*)stream.next_out - data;
    size_t csize = used + stream.avail_out;
    size_t nsize = (csize == 0) ? init_size : csize * 2;

    char* tmp = (char*)realloc(data, nsize);
    if(tmp == NULL) 
    {
        return false;
    }

    data = tmp;
    stream.next_out  = (Bytef*)(tmp + used);
    stream.avail_out = nsize - used;

    return true;
}

int msgpack_zbuffer::msgpack_zbuffer_write(const char* buf, size_t len)
{
    stream.next_in = (Bytef*)buf;
    stream.avail_in = len;

    do {
        if(stream.avail_out < MSGPACK_ZBUFFER_RESERVE_SIZE) 
        {
            if(!msgpack_zbuffer_expand()) 
            {
                return -1;
            }
        }

        if(deflate(&stream, Z_NO_FLUSH) != Z_OK) 
        {
            return -1;
        }
    } while(stream.avail_in > 0);

    return 0;
}

char* msgpack_zbuffer::msgpack_zbuffer_flush()
{
    while(true) {
        switch(deflate(&stream, Z_FINISH)) 
        {
    case Z_STREAM_END:
        return data;
    case Z_OK:
        if(!msgpack_zbuffer_expand()) 
        {
            return NULL;
        }
        break;
    default:
        return NULL;
        }
    }
}

const char* msgpack_zbuffer::msgpack_zbuffer_data()
{
    return data;
}

size_t msgpack_zbuffer::msgpack_zbuffer_size()
{
    return (char*)stream.next_out - data;
}

void msgpack_zbuffer::msgpack_zbuffer_reset_buffer()
{
    stream.avail_out += (char*)stream.next_out - data;
    stream.next_out = (Bytef*)data;
}

bool msgpack_zbuffer::msgpack_zbuffer_reset()
{
    if(deflateReset(&stream) != Z_OK) 
    {
        return false;
    }
    msgpack_zbuffer_reset_buffer();
    return true;
}

char* msgpack_zbuffer::msgpack_zbuffer_release_buffer()
{
    char* tmp = data;
    data = NULL;
    stream.next_out = NULL;
    stream.avail_out = 0;
    return tmp;
}


class zbuffer : public msgpack_zbuffer 
{
public:
	zbuffer(int level = Z_DEFAULT_COMPRESSION,
			size_t init_size = MSGPACK_ZBUFFER_INIT_SIZE)
	{
		if (!msgpack_zbuffer_init(this, level, init_size)) 
        {
			throw std::bad_alloc();
		}
	}

	~zbuffer()
	{
		msgpack_zbuffer_destroy(this);
	}

public:
	void write(const char* buf, size_t len)
	{
		if(msgpack_zbuffer_write(this, buf, len) < 0) {
			throw std::bad_alloc();
		}
	}

	char* flush()
	{
		char* buf = msgpack_zbuffer_flush(this);
		if(!buf) {
			throw std::bad_alloc();
		}
		return buf;
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
		return msgpack_zbuffer_size(this);
	}

	void reset()
	{
		if(!msgpack_zbuffer_reset(this)) {
			throw std::bad_alloc();
		}
	}

	void reset_buffer()
	{
		msgpack_zbuffer_reset_buffer(this);
	}

	char* release_buffer()
	{
		return msgpack_zbuffer_release_buffer(this);
	}

private:
	typedef msgpack_zbuffer base;

private:
	zbuffer(const zbuffer&);
};


}  // namespace msgpack

#endif /* msgpack/zbuffer.hpp */

