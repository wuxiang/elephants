#ifndef COMMON_MATH_H_
#define COMMON_MATH_H_
#include "globalEnv.h"
#include "alignment.hpp"

namespace Elephants
{
typedef Elephants::uint64_t static_gcd_type;


template < static_gcd_type Value1, static_gcd_type Value2 >
struct static_gcd_helper_t
{
private:
    static const static_gcd_type new_value1 = Value2 ;
    static const  static_gcd_type new_value2 = Value1 % Value2 ;

#ifndef __BORLANDC__
#define BOOST_DETAIL_GCD_HELPER_VAL(Value) static_cast<static_gcd_type>(Value)
#else
    typedef static_gcd_helper_t  self_type;
#define BOOST_DETAIL_GCD_HELPER_VAL(Value)  (self_type:: Value )
#endif

    typedef static_gcd_helper_t< BOOST_DETAIL_GCD_HELPER_VAL(new_value1),
        BOOST_DETAIL_GCD_HELPER_VAL(new_value2) >  next_step_type;

#undef BOOST_DETAIL_GCD_HELPER_VAL

public:
    static const  static_gcd_type value = next_step_type::value ;
};

// Non-recursive case
template < static_gcd_type Value1 >
struct static_gcd_helper_t< Value1, 0UL >
{
    static const  static_gcd_type value = Value1 ;
};



template < static_gcd_type Value1, static_gcd_type Value2 >
struct static_lcm_helper_t
{
    typedef static_gcd_helper_t<Value1, Value2>  gcd_type;

    static const  static_gcd_type value = Value1 / gcd_type::value
        * Value2 ;
};

// Special case for zero-GCD values
template < >
struct static_lcm_helper_t< 0UL, 0UL >
{
    static const  static_gcd_type value = 0UL ;
};

template < static_gcd_type Value1, static_gcd_type Value2 >
struct static_gcd: public Elephants::detail::integral_c<static_gcd_type, (static_gcd_helper_t<Value1, Value2>::value) >
{
};  // boost::math::static_gcd


//  Compile-time least common multiple evaluator class declaration  ----------//

template < static_gcd_type Value1, static_gcd_type Value2 >
struct static_lcm : public Elephants::detail::integral_c<static_gcd_type, (static_lcm_helper_t<Value1, Value2>::value) >
{
};  // boost::math::static_lcm


}

#endif


