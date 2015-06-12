
#include "SSDBResult.h"
#include "SSLog.h"
#include <boost/algorithm/string.hpp>


namespace ss {

    static char __db_magic__[5] = {'D' , 'B' , 'R' , 'S' , '\0'} ;

    static const char * MAGIC()  
    {
        return __db_magic__;
    }

    struct head_{
        char dbrs[4];
        int buflen;
        uint64 row_count;
        int field_count;
        int reserved; //for 8 bytes aligned
    };

    db_row::db_row()
    {
        block_ = NULL ;
    }

    db_row::db_row(block * bk)
    {
        block_ = NULL ;
        attach(bk) ;
    }

    db_row::db_row(const db_row& row) 
    {
        block_ = NULL ;
        attach(row.block_) ;
        rlist::operator=(row);
    }

    db_row::~db_row()
    {
        detach() ;
// 		rlist::del_init(this) ;
    }

    void db_row::attach(block * bk)
    {
        if(bk != NULL)
            bk->increment() ;

        detach() ;

        block_ = bk ;
    }

    void db_row::detach()
    {
        if(block_ != NULL)
        {
            long v = block_->decrement() ;
            if(v == 0)
            {
                ss::__field_holder *fields = (ss::__field_holder *)block_->buffer() ;
                for(unsigned int idx = 0 ; idx < block_->length()/sizeof(ss::__field_holder) ; idx++){
                    if(fields[idx].type == ss::SSTYPE_STRING){
                        free(fields[idx].data.str);
                        fields[idx].data.str = NULL;
                    }
                    if(fields[idx].type == ss::SSTYPE_RAW){
                        free(fields[idx].data.raw);
                        fields[idx].data.raw = NULL;
                    }
                }//endif  for ....
                delete block_ ;
                block_ = NULL ;
            }
        }
    }

    db_row &db_row::operator=(const db_row &rhs)
    {
        if(this == &rhs){
            return *this;
        }
        attach(rhs.block_);
        rlist::operator=(rhs);
        return *this;
    }

    void db_row::clear()
    {
        attach(NULL);
        init();
    }


    db_result::db_result()
    {
        row_count_ = 0 ;
    }

    void db_result::clear()
    {
        db_row *row = NULL ;
        //int idx = 0 ;
        while((row = (db_row *)rows_.next) != &rows_)
        {
            rlist::del_init(row) ;
            //LOGDEBUG("idx[%d] row[%p] block[%p] head[%p]" , idx , row , row->get_block() , row->get_block()->head()) ;

            delete row ;
            //idx++ ;
        }

        row_count_ = 0 ;
    }

    void db_result::reset() 
    {
        clear() ;
        header_.clear() ;
    }

    db_result::~db_result()
    {
        clear() ;
    }

    void db_result::set_header(const header& h)
    {
        header_ = h ;
        lower_case_header_ = h;
        lower_case_header_.names_.clear();
        for(std::map<std::string , int>::iterator it = header_.names_.begin(); it != header_.names_.end(); ++it){
            lower_case_header_.names_[to_lower_copy(it->first)] = it->second;
        }
        for(std::vector<field_def>::iterator it = lower_case_header_.defs_.begin(); it != lower_case_header_.defs_.end(); ++it){
            to_lower(it->name);
        }
        for(std::vector<std::string>::iterator it = lower_case_header_.primary_.begin(); it != lower_case_header_.primary_.end(); ++it){
            to_lower(*it);
        }
    }

    void db_result::add_row(db_row * row) 
    {
        rows_.add_tail(row) ;
        //LOGDEBUG("row_count[%llu] row[%p] block[%p] head[%p]" , row_count_ , row , row->get_block() , row->get_block()->head()) ;
        row_count_++ ;
    }

    void db_result::del_row(db_row * row)
    {	
        rlist::del_init(row) ;
        delete row;
        row_count_-- ;
    }

    const db_row * db_result::next(const db_row * cur) const
    {
        if(cur == NULL)
        {
            if(rows_.empty() == true)
                return NULL ;
            else
                return (db_row *)rows_.next ;
        }
        else
        {
            if(cur == &rows_ || cur->next == &rows_ || cur->next == cur)
                return NULL ;
            else
                return (db_row *)cur->next ;
        }
    }

    const db_row * db_result::prev(const db_row * cur) const
    {
        if(cur == NULL)
        {
            if(rows_.empty() == true)
                return NULL ;
            else
                return (db_row *)rows_.prev ;
        }
        else
        {
            if(cur == &rows_ || cur->prev == &rows_ || cur->prev == cur)
                return NULL ;
            else
                return (db_row *)cur->prev ;
        }
    }

    int db_result::dump(std::vector<block_ref>& blocks) const
    {
        char *buffer = NULL;
        int buflen = 0;
        int tlen = 0 ;

        /*
            计算头部长度,包含所有field_def的长度之和+head_（表头信息）的长度
        */
        buflen = 0 ;
        int field_count = header_.field_count() ;
        for(int idx = 0 ; idx < field_count ; idx++)
        {
            const ss::field_def& def = header_.fields(idx) ;
            const char * name = def.name.c_str() ;
            int nlen = ::strlen(name) + 1 ;
            buflen += nlen ; //name
            buflen += ::strlen(def.table_name.c_str()) + 1; //table_name
            buflen += sizeof(def.type); //type
            buflen += sizeof(def.decimals); //decimals;
            buflen += sizeof(def.max_length); //max_length
            buflen += sizeof(def.length); //length
        }

        buflen += sizeof(head_); //14 bytes

        ss::block * block = new ss::block(buflen) ;
        buffer = block->buffer() ;

        //拷贝头部
        head_ head;
        memset(&head, 0, sizeof(head));
        memcpy(head.dbrs, "DBRS", 4);
        head.buflen = buflen;
        head.row_count = row_count_;
        head.field_count = field_count;
        memcpy(buffer, &head, sizeof(head));
        buflen = sizeof(head);
        for(int idx = 0 ; idx < field_count ; idx++)
        {
            const ss::field_def& def = header_.fields(idx) ;
            const char * name = def.name.c_str() ;
            ss::uint8 type = def.type ;
            int nlen = ::strlen(name) + 1 ;

            memcpy(buffer + buflen , name , nlen) ; //name
            buflen += nlen ;

            nlen = ::strlen(def.table_name.c_str()) + 1;
            memcpy(buffer + buflen , def.table_name.c_str() , nlen); //table_name
            buflen += nlen ;

            nlen = sizeof(type);
            memcpy(buffer + buflen , &type , nlen); //type
            buflen += nlen;;	

            nlen = sizeof(def.decimals);
            memcpy(buffer + buflen , &def.decimals , nlen); //decimals
            buflen += nlen;

            nlen = sizeof(def.max_length);
            memcpy(buffer + buflen , &def.max_length , nlen); //max_length
            buflen += nlen;

            nlen = sizeof(def.length);
            memcpy(buffer + buflen, &def.length, nlen);	//length
            buflen += nlen;
        }
        block->length(buflen) ;

        ss::block_ref ref(block , true) ;
        blocks.push_back(ref) ;

        tlen += buflen ;


        /*
            拷贝记录
        */
        const db_row * row = NULL ;
        int seq = 0 ;
        while((row = next(row)) != NULL)
        {
            buffer =  row->buffer() ;	
            ss::field_holder * fields = (ss::field_holder *)buffer ;

            /*
                先计算记录的长度
            */
            buflen = row->length();
            for(int idx = 0 ; idx < field_count ; idx++)
            {
                if(fields[idx].type == ss::SSTYPE_STRING || fields[idx].type == ss::SSTYPE_RAW ){
                    buflen += fields[idx].length ;
                }
            }

            block = new ss::block(buflen) ;
            buffer = block->buffer() ;

            memcpy(buffer , row->buffer() , row->length());
            buffer += row->length();
            for(int idx = 0 ; idx < field_count ; idx++)
            {
                if(fields[idx].type == ss::SSTYPE_STRING){
                    memcpy(buffer, fields[idx].data.str , fields[idx].length) ;
                    buffer += fields[idx].length ;
                }
                else if(fields[idx].type == ss::SSTYPE_RAW){
                    memcpy(buffer, fields[idx].data.raw , fields[idx].length) ;
                    buffer += fields[idx].length ;
                }
            }

            block->length(buflen) ;
            tlen += buflen ;
            ss::block_ref rref(block , true) ;

            blocks.push_back(rref) ;
            seq++ ;
        }

        LOGDEBUG("we have dump field_count[%d] row_count[%d] sequence[%d] total length[%d]" , field_count , row_count_ , seq , tlen) ;

        return tlen ;	
    }

    void db_result::restore(const std::vector<block_ref>& blocks)
    {
        reset();
        char *buffer = blocks[0].get_block()->buffer();
        head_ *head = reinterpret_cast<head_ *>(buffer);
        buffer += sizeof(head_);
        // set header_
        for(int idx = 0; idx < head->field_count; idx++){
            ss::field_def def;
            def.name = buffer;
            buffer += (def.name.size() + 1);
            def.table_name = buffer;
            buffer += (def.table_name.size() + 1);
            def.type = *reinterpret_cast<uint8 *>(buffer);
            buffer += sizeof(uint8);
            def.decimals = *reinterpret_cast<unsigned int *>(buffer);
            buffer += sizeof(unsigned int);
            def.max_length = *reinterpret_cast<unsigned long *>(buffer);
            buffer += sizeof(unsigned long);
            def.length = *reinterpret_cast<unsigned long *>(buffer);
            buffer += sizeof(unsigned long);
            header_.add(def);
        }

        //set row
        for(std::vector<block_ref>::const_iterator iter = (++blocks.begin()); iter != blocks.end(); ++iter){
            buffer = iter->get_block()->buffer();
            int tlen = sizeof(ss::field) * head->field_count;
            ss::block * block = new ss::block(tlen);
            memcpy(block->buffer(), buffer, tlen);
            buffer += tlen;

            ss::field_holder *fields = (ss::field_holder *)block->buffer();
            for(int idx = 0 ; idx < head->field_count ; idx++)
            {
                if(fields[idx].type == ss::SSTYPE_STRING){
                    fields[idx].data.str = (char *)::malloc(fields[idx].length);
                    memcpy(fields[idx].data.str, buffer , fields[idx].length) ;
                    buffer += fields[idx].length ;
                }
                else if(fields[idx].type == ss::SSTYPE_RAW){
                    fields[idx].data.raw = ::malloc(fields[idx].length);
                    memcpy(fields[idx].data.raw, buffer , fields[idx].length) ;
                    buffer += fields[idx].length ;
                }
            }
            block->length(tlen);
            ss::db_row *db_row = new ss::db_row(block);
            db_row->attach(block);
            add_row(db_row);
        }
    }

    int db_result::dumpForPatch(std::vector<ss::block_ref>& blocks) const
    {
        int hlen = 0 , blen = 0 ;

        if((hlen = encodeHead(blocks)) <= 0)
            return -1 ;

        if((blen = encodeBody(blocks)) < 0)
            return -2 ;

        return (hlen+blen) ;    
    }

    int db_result::calcHeadLength() const
    {

        int buflen = 0 ;
        int field_count = header_.field_count() ;
        for(int idx = 0 ; idx < field_count ; ++idx)
        {
            const field_def& def = header_.fields(idx) ;
            int nlen = def.name.length() + 1 ;

            buflen += nlen ;
            ++buflen ;
        }

        buflen += 14 ;

        return buflen ;
    }

    int db_result::encodeHead(std::vector<block_ref>& blocks) const
    {
        int buflen = 0 ;
        if((buflen = calcHeadLength()) <= 0)
            return 0 ;

        ss::block * block = new ss::block(buflen) ;

        char * buffer = block->buffer() ;

        int field_count = header_.field_count() ;
        int row_count = (int)this->row_count() ;

        //拷贝头部
        memcpy(buffer , MAGIC() , 4) ;
        memcpy(buffer + 4 , &buflen , 4) ;
        memcpy(buffer + 8 , &row_count , 4) ;
        memcpy(buffer + 12 , &field_count , 2) ;

        buflen = 14 ;
        for(int idx = 0 ; idx < field_count ; idx++)
        {
            const field_def& def = header_.fields(idx) ;
            const char * name = def.name.c_str() ;
            int nlen = def.name.length() ;
            ss::uint8 type = def.type ;

            if(name != NULL)
            {
                memcpy(buffer + buflen , name , nlen) ;
            }
            buflen += nlen ;
            buffer[buflen++] = '\0' ;

            memcpy(buffer + buflen , &type , 1) ;
            buflen++ ;		
        }

        block->length(buflen) ;

        ss::block_ref ref(block) ;
        blocks.push_back(ref) ;

        return buflen ;
    }

    int db_result::encodeBody(std::vector<block_ref>& blocks) const
    {
        int field_count = header_.field_count() ;
		
        if(field_count <= 0 || row_count() ==0 )
        {
            return 0 ;
        }

        //拷贝记录
        const db_row *row = NULL ;
        int seq = 0 ;
        int total = 0 ;

        while((row = this->next(row)) != NULL)
        {
            char * buffer = row->buffer() ;
            const ss::field_holder * fields = (const ss::field_holder *)buffer ;

            //计算长度
            int buflen = 8 ;        //SEQUENCE[4] + LENGTH[4]

            for(int fidx = 0 ; fidx < field_count ; ++fidx)
            {
                //TYPE[1] + NULLFLAG[1] + VALUE

                buflen += 1 ;       //TYPE 
                buflen += 1 ;       //NULL FLAG

                if(field_assigned(fields[fidx]))
                {
                    buflen += fields[fidx].length ;
                }
            }
            total += buflen ;

            //拷贝数据
            ss::block * block = new ss::block(buflen) ;
            buffer = block->buffer() ;

            memcpy(buffer , &seq , 4) ;
            memcpy(buffer + 4 , &buflen , 4) ;
            buflen = 8 ;

            for(int fidx = 0 ; fidx < field_count ; ++fidx)
            {
                const field_def& def = header_.fields(fidx) ;
                memcpy(buffer + buflen , &def.type , 1) ;
                ++buflen ;

                if(!field_assigned(fields[fidx]))
                {
                    //没有对应关系，或者类型不相等，则认为是空
                    buffer[buflen++] = 1 ;
                    continue ;
                }
                else
                {
                    buffer[buflen++] = 0 ;
                }

                if(def.type == ss::SSTYPE_STRING)
                    memcpy(buffer + buflen , fields[fidx].data.str , fields[fidx].length) ;
                else if(def.type == ss::SSTYPE_RAW)
                    memcpy(buffer + buflen , fields[fidx].data.raw , fields[fidx].length) ;
                else
                    memcpy(buffer + buflen , &fields[fidx].data.val , fields[fidx].length) ;

                buflen += fields[fidx].length ;
            }

            block->length(buflen) ;
            ss::block_ref ref(block) ;
            blocks.push_back(ref) ;
            ++seq ;
        }

        return total ;
    }

}

