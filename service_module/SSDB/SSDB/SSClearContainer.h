//-------------------------------------------------------------------------------------
//--Copyright	:	Sumscope
//--FileName	:	SSClearContainer.h
//--Author		:	cheng.xu@sumscope.com
//--Date		:	2015.02.11
//--Desc		:	清理容器模板函数
//--History		:	Created by cheng.xu, 2015.02.11
//--				
//-------------------------------------------------------------------------------------


#ifndef __SS_CLEAR_CONTAINER_H
#define __SS_CLEAR_CONTAINER_H

#include <map>
#include <list>
#include <vector>
#include <set>

namespace ss
{
	template<typename T>
	void ClearContainer(std::vector<T*>& items)
	{
		for(typename std::vector<T*>::iterator it = items.begin(); it != items.end(); it++)
		{
			if (NULL != (*it))
			{
				delete *it;
				*it = NULL;
			}
		}
		items.clear();
	}

	template<typename T>
	void ClearContainer(std::list<T*>& items)
	{
		for(typename std::list<T*>::iterator it = items.begin(); it != items.end(); it++)
		{
			if (NULL != (*it))
			{
				delete *it;
				*it = NULL;
			}
		}
		items.clear();
	}

	template<typename T>
	void ClearContainer(std::set<T*>& items)
	{
		for(typename std::set<T*>::iterator it = items.begin(); it != items.end(); it++)
		{
			if (NULL != (*it))
			{
				delete *it;
				*it = NULL;
			}
		}
		items.clear();
	}

	template<typename T1,typename T2>
	void ClearContainer(std::map<T1,T2*>& items)
	{
		for(typename std::map<T1,T2*>::iterator it = items.begin(); it != items.end(); it++)
		{
			if (NULL != it->second)
			{
				delete it->second;
				it->second = NULL;
			}
		}
		items.clear();
	}
}




#endif /** __SS_CLEAR_CONTAINER_H */
