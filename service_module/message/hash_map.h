
#ifndef __SDBUS_HASHMAP_H
#define __SDBUS_HASHMAP_H 1

#include "sdbus/compile.h"
#include "sdbus/type.h"

#include <string>
#include "sdbus/string.h"

namespace sdbus{

SDBUSAPI uint32_t _hash_key(int32_t key) ;
SDBUSAPI uint32_t _hash_key(int64_t key) ;

SDBUSAPI uint32_t _hash_key(uint32_t key) ;
SDBUSAPI uint32_t _hash_key(uint64_t key) ;

SDBUSAPI uint32_t _hash_key(const char * key) ;
inline uint32_t _hash_key(const std::string& key) {return _hash_key(key.c_str()) ;}
inline uint32_t _hash_key(const string& key) {return _hash_key(key.c_str()) ;}

inline bool _hash_compare(int32_t k1 , int32_t k2) {return k1 == k2 ;}
inline bool _hash_compare(int64_t k1 , int64_t k2) {return k1 == k2 ;}

inline bool _hash_compare(uint32_t k1 , uint32_t k2) {return k1 == k2 ;}
inline bool _hash_compare(uint64_t k1 , uint64_t k2) {return k1 == k2 ;}

SDBUSAPI bool _hash_compare(const char * k1 , const char * k2) ;
inline bool _hash_compare(const std::string& k1 , const std::string& k2) {return _hash_compare(k1.c_str() , k2.c_str()) ;}
inline bool _hash_compare(const string& k1 , const string& k2) {return _hash_compare(k1.c_str() , k2.c_str()) ;}


template<typename KT>
class hasher{
public:
    uint32_t operator()(const KT& key) const {return _hash_key(key) ;}
    bool operator()(const KT& k1 , const KT& k2) const {return _hash_compare(k1 , k2) ;}
} ;


template<typename KT , typename VT , typename HT = hasher<KT> >
class HashMap{
public:
    HashMap() : hasher_(HT()) 
    {
        entries_ = NULL ;
        htsize_ = 0 ;

        nodes_ = NULL ;
        count_ = 0 ;
        capacity_ = 0 ;
        last_ = 0 ;
    }

    HashMap(const HashMap& hm) : hasher_(HT())
    {
        entries_ = NULL ;
        htsize_ = 0 ;

        nodes_ = NULL ;
        count_ = 0 ;
        capacity_ = 0 ;
        last_ = 0 ;   

        Assign(hm) ;
    }

    HashMap& operator=(const HashMap& hm) 
    {
        Assign(hm) ;
        return (*this) ;
    }

    ~HashMap()
    {
        Final() ;
    }

    template<typename NKT , typename NVT>
    class Node{
    public:
        uint32_t HashCode ;
        int Flag : 2 ;  //0表示未被占用
        int Next : 30;  //-1表示结束，非负数为有效值

        NKT Key ;
        NVT Value ;
    } ;

    typedef KT KeyType ;
    typedef VT ValueType ;
    typedef Node<KT , VT> NodeType ;

    inline bool Init(size_t capacity = 0)
    {
        if(capacity == 0)
            return true ;

        size_t nsize = sizeof(Node<KT , VT>) * capacity ;
        Node<KT , VT> * nodes = (Node<KT , VT> *)::malloc(nsize) ;
        if(nodes == NULL)
            return false ;

        ::memset(nodes , 0 , nsize) ;
        nodes_ = nodes ;
        capacity_ = capacity ;
        return true ;        
    }

    inline int Set(const KT& key)
    {
        uint32_t hkey = hasher_(key) ;
        if(htsize_ == 0)
        {
            if(Rehash() == false)
                return -1 ;
        }

        int hidx = (int)(hkey % htsize_) ;
        int nidx = entries_[hidx] ;
        while(nidx >= 0)
        {
            Node<KT , VT>& node = nodes_[nidx] ;
            int next = node.Next ;

            if(node.HashCode == hkey)
            {
                if(hasher_(node.Key , key) == true)
                {
                    return nidx ;
                }
            }

            if(next < 0)
                break ;
            nidx = next ;        
        }

        int aidx = AllocNode() ;
        if(aidx < 0)
            return -1 ;

        Node<KT , VT>& anode = nodes_[aidx] ;
        anode.Key = key ;
        anode.HashCode = hkey ;
        anode.Flag = 1 ;
        anode.Next = -1 ;

        if(nidx < 0)
            entries_[hidx] = aidx ;
        else
        {
            Node<KT , VT>& tail = nodes_[nidx] ;
            tail.Next = aidx ;
        }

        if((count_ << 1) >= htsize_)
        {
            Rehash() ;
        }

        return aidx ;        
    }
    inline bool Set(const KT& key , const VT& value)
    {
        int index = Set(key) ;
        if(index < 0)
            return false ;

        Node<KT , VT> * node = GetNode(index) ;
        if(node == NULL)
            return false ;

        node->Value = value ;
        return true ;
    }

    inline int  Find(const KT& key) const 
    {
        if(count_ == 0 || htsize_ == 0)
            return -1 ;

        uint32_t hkey = hasher_(key) ;
        int hidx = hkey % htsize_ ;
        int nidx = entries_[hidx] ;
        while(nidx >= 0)
        {
            Node<KT , VT>& cur = nodes_[nidx] ;
            int next = cur.Next ;
            if(cur.HashCode == hkey)
            {
                if(hasher_(cur.Key , key) == true)
                {
                    return nidx ;
                }
            }

            nidx = next ;
        }

        return -1 ;        
    }

    inline const NodeType * FindNode(const KT& key) const 
    {
        return GetNode(Find(key)) ;
    }

    inline NodeType * FindNode(const KT& key) 
    {
        return GetNode(Find(key)) ;
    }
    
    inline const NodeType * GetNode(int index) const 
    {
        if(index < 0 || index >= (int)capacity_)
            return NULL ;

        const NodeType * node = nodes_ + index ;
        if(node->Flag == 0)
            return NULL ;
        else
            return node ;
    }

    inline NodeType * GetNode(int index)
    {
        if(index < 0 || index >= (int)capacity_)
            return NULL ;

        NodeType * node = nodes_ + index ;
        if(node->Flag == 0)
            return NULL ;
        else
            return node ;
    }

    inline bool DelNode(int index) 
    {
        if(index < 0 || index >= (int)capacity_)
            return false ;

        return Del(nodes_[index]) ;
    }

    inline bool Del(const Node<KT , VT>& node)
    {
        return Del(node.Key) ;
    }

    inline bool Del(const KT& key)
    {
        if(htsize_ == 0)
            return false ;

        uint32_t hkey = hasher_(key) ;    
        int hidx = (int)(hkey % htsize_) ;
        int nidx = entries_[hidx] ;
        int prev = -1 ;
        while(nidx >= 0)
        {
            Node<KT , VT>& cur = nodes_[nidx] ;
            int next = cur.Next ;
            if(cur.HashCode == hkey)
            {
                if(hasher_(cur.Key , key) == true)
                {
                    FreeNode(nidx) ;

                    if(prev < 0)
                    {
                        entries_[hidx] = next ;
                    }
                    else
                    {
                        Node<KT , VT>& pnode = nodes_[prev] ;
                        pnode.Next = next ;
                    }

                    if((count_ * 3) <= capacity_)
                    {
                        Shrink() ;
                    }    

                    return true ;
                }
            }    

            prev = nidx ;
            nidx = next ;
        }

        return false ;    
    }

    inline size_t Size() const {return count_ ;}
    inline size_t Capacity() const {return capacity_ ;}

    inline void Clear() 
    {
        if(nodes_ != NULL)
        {
            for(size_t idx = 0 ; idx < capacity_ ; ++idx)
            {
                if(nodes_[idx].Flag == 0)
                    continue;

                FreeNode(idx) ;
            }

            size_t nsize = sizeof(Node<KT , VT>) * capacity_ ;
            if(nsize > 0)
                ::memset(nodes_ , 0 , nsize) ;
        }
        count_ = 0 ;
        last_ = 0 ;

        if(entries_ != NULL)
        {
            ::memset(entries_ , -1 , sizeof(int) * htsize_) ;
        }    
    }

    inline void Final() 
    {
        Clear() ;

        if(nodes_ != NULL)
            ::free(nodes_) ;

        if(entries_ != NULL)
            ::free(entries_) ;

        entries_ = NULL ;
        htsize_ = 0 ;

        nodes_ = NULL ;
        count_ = 0 ;
        capacity_ = 0 ;
        last_ = 0 ;    
    }

    inline bool Extend(size_t capacity = 0)
    {
        if(capacity == 0)
        {
            //默认扩展策略，初始为32，后续为翻倍增长
            capacity = (capacity_ == 0)?32:(capacity_ << 1) ;
            if(count_ < capacity_)       //原来的节点数依然够用，无需扩展。
                return true ;
        }
        if(capacity_ >= capacity)
            return true ;

        size_t nsize = sizeof(Node<KT , VT>) * capacity ;
        if(nsize == 0)
            return true ;

        Node<KT , VT> * nodes = (Node<KT , VT> *)::malloc(nsize) ;
        if(nodes == NULL)
            return false ;
        ::memset(nodes , 0 , nsize) ;

        if(nodes_ != NULL)
        {
            size_t osize = sizeof(Node<KT , VT>) * capacity_ ;
            ::memcpy(nodes , nodes_ , osize) ;
            ::free(nodes_) ;
        }

        nodes_ = nodes ;
        capacity_ = capacity ;

        return true ;    
    }

    inline bool Shrink(size_t capacity = 0)
    {
        //1、先判断需要收缩的最后长度
        if(capacity == 0)
        {
            //默认收缩规则，如果有效节点没有低于1/4，不收缩。否则收缩一半
            if((count_ << 2) >= capacity_)
                return true ;

            capacity = (capacity_ >> 1) ;
        }
        if(capacity >= capacity_ )
            return true ;
        if(capacity < count_)
            return false ;

        //分配内存
        size_t nsize = sizeof(Node<KT , VT>) * capacity ;
        if(nsize == 0)
            return true ;

        Node<KT , VT> * nodes = (Node<KT , VT> *)::malloc(nsize) ;
        if(nodes == NULL)
            return false ;

        ::memset(nodes , 0 , nsize) ;
        int nidx = 0 ;
        for(size_t oidx = 0 ; oidx < capacity_ ; ++oidx)
        {
            Node<KT , VT> * cur = nodes_ + oidx ;
            if(cur->Flag == 0)
                continue ;

            Node<KT , VT> * node = nodes + nidx ;
            ::memcpy(node , cur , sizeof(Node<KT , VT>)) ;
            ++nidx ;
        }

        if(nodes_ != NULL)
        {
            ::free(nodes_) ;
        }

        nodes_ = nodes ;
        capacity_ = capacity ;

        return Rehash(htsize_) ;    
    }

    inline bool Rehash(size_t capacity = 0)
    {
        if(capacity == 0)
        {
            //执行默认规则，
            if((count_ << 1) < htsize_)
                return true ;

            capacity = (htsize_ == 0) ? 32 : (htsize_ << 1) ;
        }

        if(capacity != htsize_)
        {
            size_t htsize = sizeof(int) * capacity ;
            int * ents = (int *)::malloc(htsize) ;
            if(ents == NULL)
                return false ;        

            if(entries_ != NULL)
                ::free(entries_) ;
            entries_ = ents ;
            htsize_ = capacity ;
        }

        size_t htsize = sizeof(int) * htsize_ ;
        ::memset(entries_ , -1 , htsize) ;

        for(size_t idx = 0 ; idx < capacity_ ; ++idx)
        {
            Node<KT , VT>& cur = nodes_[idx] ;
            if(cur.Flag == 0)
                continue ;

            cur.Next = -1 ;
            uint32_t hkey = cur.HashCode ;
            int hidx = (int)(hkey % htsize_) ;
            int nidx = entries_[hidx] ;
            while(nidx >= 0)
            {
                Node<KT , VT>& node = nodes_[nidx] ;
                int next = node.Next ;
                if(next < 0)
                    break ;
                nidx = next ;
            }

            if(nidx < 0)
                entries_[hidx] = idx ;
            else
            {
                Node<KT , VT>& tail = nodes_[nidx] ;
                tail.Next = idx ;
            }
        }

        return true ;    
    }

private:

    int * entries_ ;
    size_t htsize_ ;

    Node<KT , VT> * nodes_ ;
    size_t count_ ;
    size_t capacity_ ;
    size_t last_ ;

    HT hasher_ ;

    int AllocNode()
    {
        if(count_ >= capacity_)
        {
            if(Extend() == false)
                return -1 ;
        }

        int index = (int)last_ ;
        for(size_t times = 0 ;times < capacity_ ; ++times)
        {
            if(index >= (int)capacity_)
                index = 0 ;

            Node<KT , VT> * node = nodes_ + index ;
            if(node->Flag == 0)
            {                
                ::new (node) Node<KT , VT> ;
                node->Flag = 1 ;
                ++count_ ;
                last_ = (size_t)index + 1 ;
                return index ;
            }

            ++index ;
        }

        return -1 ;    
    }

    void FreeNode(int index)
    {
        if(index < 0 || index > (int)capacity_)
            return ;

        Node<KT , VT> * node = nodes_ + index ;
        if(node->Flag == 0)
            return ;

        node->~Node<KT , VT>() ;

        nodes_[index].Flag = 0 ;
        --count_ ;
    }

    void Assign(const HashMap& hm) 
    {
        Final() ;
        if(Extend(hm.capacity_) == false)
            return ;

        for(size_t nidx = 0 ; nidx < capacity_ ; ++nidx)
        {
            NodeType& onode = hm.nodes_[nidx] ;
            if(onode.Flag == 0)
                continue ;

            NodeType& node = nodes_[nidx] ;
            node = onode ;
        }

        size_t htsize = hm.htsize_ ;
        Rehash(htsize) ;
    }
} ;

}

#endif  /** __SDBUS_HASHMAP_H */
