
#ifndef __SS_DB_RESULT_H
#define __SS_DB_RESULT_H 1

#include "SSType.h"
#include "SSField.h"
#include "SSTuple.h"
#include "SSRList.h"
#include "SSBlock.h"

#include <vector>

namespace ss {

	class db_row : public rlist{
	private:
		block * block_ ;
	public:
		db_row() ;
		db_row(block * bk) ;
		db_row(const db_row& row) ;
		~db_row() ;

		void attach(block * bk) ;
		void detach() ;
		db_row &operator=(const db_row &rhs);
		void clear();
		inline char * buffer() const {return (block_ == NULL) ? NULL : block_->buffer() ;}
		inline int length() const {return (block_ == NULL) ? 0 : block_->length() ;}
		inline const block * get_block() const {return block_ ;}
		inline block * get_block() {return block_ ;}
	} ;


	class db_result {
	public:
		typedef tuple::header header ;

		db_result() ;
		~db_result() ;

		void clear() ;
		void reset() ;

		inline uint32 field_count() const {return header_.field_count() ;}
		inline uint64 row_count() const {return row_count_ ;}

		inline header& get_header() {return header_ ;}
		inline const header& get_header() const {return header_ ;}
		void set_header(const header& h);
        inline const header& get_lower_case_header() const {return lower_case_header_;}

		void add_row(db_row * row) ;
        void del_row(db_row * row) ;
		const db_row * next(const db_row * cur = NULL) const;
		const db_row * prev(const db_row * cur) const;


		/*
			2012-06-25
			将头部和纪录按格式导出到内存
			头部格式 ： 
				MAGIC[4] + LENGTH[4] + RECORD_COUNT[4] + FIELD_COUNT[2] +
				NAME[STRING] + TYPE[1] + ...

				其中，LENGTH等于头部总长度，包含MAGCI/LENGTH...
			
			记录格式：
				SEQUENCE[4] + LENGTH[4] +
				TYPE[1] + NULLFLAG[1] + VALUE + ...

				其中，SEQUENCE从0开始，LENGTH等于整个记录的总长度，如果NULL FLAG非0，表示NULL，同时VALUE域不存在
		*/
		int dump(std::vector<block_ref>& blocks) const;
		void restore(const std::vector<block_ref>& blocks);

        int dumpForPatch(std::vector<ss::block_ref>& blocks) const;
        int calcHeadLength() const;
        int encodeHead(std::vector<block_ref>& blocks) const;
        int encodeBody(std::vector<block_ref>& blocks) const;

	protected:
		header header_ ;
		header lower_case_header_ ;
		uint64 row_count_ ;

		rlist rows_ ;	
	} ;

	template<typename T>
	class db_cursor {
	private:
		const db_result * result_ ;
		mutable const db_row *	row_ ;

		const T * value() const
		{
			if(result_ == NULL || row_ == NULL)
				return NULL ;


			char * buf = row_->buffer() ;

			if(buf == NULL)
				return NULL ;
			else
				return (const T*)buf ;		
		}
	public:
		db_cursor(const db_result * result)
		{
			row_ = NULL ;
			result_ = result ;
		}
		
		operator bool () const
		{
			return value() != NULL ;
		}

		const T * operator->() const
		{
			return value() ;
		}

		bool next() const
		{
			if(result_ == NULL)
				return false ;

			row_ = result_->next(row_) ;
			if(row_ == NULL)
				return false ;
			else
				return true ;
		}
	} ;

}

#endif  /** __SS_DB_RESULT_H */
