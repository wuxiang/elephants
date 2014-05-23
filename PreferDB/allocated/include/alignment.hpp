#ifndef BOOST_ALIGNMENT_H_
#define BOOST_ALIGNMENT_H_

namespace Elephants
{
    template <typename T> struct alignment_of;

    namespace detail
    {
        // align value
        template <typename T>
        struct alignment_of_hack
        {
            char c;
            T t;
            alignment_of_hack();
        };

        template <unsigned A, unsigned S>
        struct alignment_logic
        {
            static const std::size_t value = A < S ? A : S;
        };



        template< typename T >
        struct alignment_of_impl
        {
            static const std::size_t value =
                (alignment_logic<
                sizeof(alignment_of_hack<T>) - sizeof(T),
                sizeof(T)
                >::value);
        };

        template<class _Ty, _Ty _Val>
        struct integral_constant
        {	// convenient template for integral constant types
            static const _Ty value = _Val;

            typedef _Ty value_type;
            typedef integral_constant<_Ty, _Val> type;
        };

        template<typename T>
        struct alignment_of: public integral_constant<std::size_t, alignment_of_impl<T>::value>
        {
        public:
            /**/
            /**/
        };

        struct integral_c_tag 
        { 
            static const int value = 0; 
        };

        template<typename T, T val>
        struct integral_c
        {
            static const T value = val;
            typedef integral_c_tag tag;
            typedef integral_c type;
            typedef T value_type;
            operator T() const { return this->value; }

        };


        template< bool C >
        struct integral_c<bool, C>
        {
            static const bool value = C;
            typedef integral_c_tag tag;
            typedef integral_c type;
            typedef bool value_type;
            operator bool() const { return this->value; }
        };
    }
}

#endif


