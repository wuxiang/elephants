
#ifndef __SS_RLIST_H
#define __SS_RLIST_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace ss {


	class rlist {
	public:
		rlist * prev ;
		rlist * next ;

		rlist()
		{
			init() ;
		}

		void init()
		{
			prev = next = this ;
		}

		inline bool linked() const
		{
			return !empty() ;
		}

		inline bool empty() const
		{
			return (next == this) ;
		}

		inline void add(rlist * node)
		{
			rlist::__add(node , this , next) ;
		}

		inline void add_tail(rlist * node)
		{
			rlist::__add(node , prev , this) ;
		}

		inline void add_head(rlist * node)
		{
			rlist::__add(node , prev , this) ;
		}

		static inline void del(rlist * node)
		{
			rlist::__del(node->prev , node->next) ;
			node->next = NULL ;
			node->prev = NULL ;
		}

		static inline void del_init(rlist *node)
		{
			rlist::__del(node->prev , node->next)  ;
			node->init() ;		
		}

		inline void splice(const rlist *new_list)
		{
			if(!new_list->empty())
				rlist::__splice(new_list , this , next) ;
		}

		inline void splice_init(rlist * new_list)
		{
			if(!new_list->empty())
				rlist::__splice(new_list , this , next) ;

			new_list->init() ;
		}

		inline rlist &operator=(const rlist &rhs)
		{
			if(rhs.empty()){
				return *this;
			}
			prev = rhs.prev;
			next = rhs.next;
			return *this;
		}

		/*
			2011-12-31
			Add Node cur , which will  be prev + cur +next
		*/
		static inline void __add(rlist *cur , rlist *prev , rlist *next)
		{
			next->prev = cur ;
			cur->next = next ;
			cur->prev = prev ;
			prev->next = cur ;
		}

		static inline void __del(rlist *prev , rlist *next)
		{
			next->prev = prev ;
			prev->next = next ;
		}

		static inline void __splice(const rlist * new_list , rlist * prev , rlist *next)
		{
			rlist *first = new_list->next ;
			rlist *last = new_list->prev ;

			first->prev = prev ;
			prev->next = first ;

			last->next = next ;
			next->prev = last ;
		}
	} ;

}

#endif  /** __SS_RLIST_H */
