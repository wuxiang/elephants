
#include "SSBlock.h"

namespace ss {

    block::block()
    {
        head_ = NULL ;
        length_ = 0 ;
    }

    block::block(const block& bc)
    {
        head_ = bc.head_ ;
        length_ = bc.length_ ;

        char * buf = (char *)malloc(length_) ;
        if(buf == NULL)
        {
            head_ = NULL ;
            length_ = 0 ;
        }
        else
        {
            memcpy(buf , head_ , length_) ;
            head_ = buf ;
        }
    }

    block::block(int len)
    {
        head_ = NULL ;
        length_ = 0 ;

        if(len <= 0)
            return ;

        char * pchar = (char *)malloc(len) ;
        if(pchar == NULL)
            return ;

        head_ = pchar ;
        length_ = len ;
    }

    block::~block()
    {
        clear() ;
    }

    char * block::buffer() const
    {
        return head_ ;
    }

    void block::reset()
    {
        memset(head_, 0, length_) ;
    }

    void block::clear()
    {
       if(head_ != NULL)
        {
            ::free(head_) ;
        }

        head_ = NULL ;
        length_ = 0 ;
    }

    bool block::resize(int size)
    {
        if(size <= 0)
            return false ;

        char *pchar = (char *)realloc(head_ , size) ;
        if(pchar == NULL)
            return false ;

        head_ = pchar ;
        length_ = size ;
        return true ;
    }

    bool block::copy(const void * buf , int len)
    {
        if(buf == NULL || len <= 0)
            return false ;

        if(length_ < len)
        {
            if(resize(len) == false)
                return false ;
        }

        memcpy(head_ , buf , len) ;
        return true ;
    }

    void block::attach(void * buf , int len)
    {
        if(head_ != NULL)
        {
            ::free(head_) ;
        }
        
        head_ = (char *)buf ;
        length_ = len ;
    }


    block_ref::block_ref()
    {
        offset_ = 0 ;
        length_ = 0 ;
    }

    block_ref::block_ref(const block_ref& ref)
    {
        offset_ = ref.offset_ ;
        length_ = ref.length_ ;

        attach(ref.get() , ref.get_auto_free()) ;
    }

    block_ref::block_ref(block * bk , bool autofree)
    {
        offset_ = 0 ;
        length_ = 0 ;

        if(bk == NULL)
            return ;

        offset_ = 0 ;
        length_ = bk->length() ;

        attach(bk , autofree) ;
    }

    block * block_ref::get_block() const 
    {
        return (block *)get() ;
    }

    char * block_ref::buffer() const
    {
        block * bk = get_block() ;
        if(bk == NULL)
            return NULL ;

        return (bk->head() + offset_) ;
    }

    void block_ref::reset()
    {
        offset_ = 0 ;
        length_ = 0 ;

        detach() ;
    }

    bool block_ref::check_valid() const
    {
        return (get() != NULL) ;
    }

    block_ref& block_ref::operator=(const block_ref& ref)
    {
        attach(ref.get() , ref.get_auto_free()) ;

        offset_ = ref.offset() ;
        length_ = ref.length() ;

        return (*this) ;
    }

    void block_ref::attach_block(block * bk , bool autofree)
    {
        attach(bk , autofree) ;

        offset_ = 0 ;
        length_ = bk->length() ;
    }


}
