
#ifndef __SS_BLOCK_H
#define __SS_BLOCK_H 1

#include <stdio.h>
#include <stdlib.h>
#include "SSType.h"
#include "SSSafeObject.h"

namespace ss {

    class block : public safe_object{
    public:
        block() ;
        block(const block& bc) ;
        block(int len) ;
        virtual ~block() ;

        inline int length() const {return length_ ;}
        inline void length(int len) {length_ = len ;}

        inline char * head() const {return head_ ;}

        char * buffer() const ;

        //只是清空有效数据，不释放缓冲区
        void reset() ;

        //释放缓冲区
        void clear() ;

        //重置缓冲区大小，只能当autofree为true的时候有效
        bool resize(int size) ;

        bool copy(const void * buf , int len) ;
        void attach(void * buf , int len) ;
    private:
        char * head_ ;          //缓冲区头部
        int length_ ;           //有效数据的大小
    } ;

    class block_ref : public safe_object_ref {
    private:
        int offset_ ;
        int length_ ;
    public:
        block_ref() ;
        block_ref(const block_ref& ref) ;
        block_ref(block * bk , bool autofree = true) ;

        block * get_block() const ;
        
        inline int offset() const {return offset_ ;}
        inline void offset(int of) {offset_ = of ;}
        inline int length() const {return length_ ;}
        inline void length(int len) {length_ = len ;}

        char * buffer() const ;

        void reset() ;

        bool check_valid() const ;

        block_ref& operator=(const block_ref& ref) ;

        void attach_block(block * bk , bool autofree = true) ;
    } ;
} 

#endif  /** __SS_BLOCK_H */

