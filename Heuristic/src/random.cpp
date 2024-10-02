#include "lib.h"
int boost_random_mechanism::random_number_generator(int  lower_bound, int upper_bound, base_generator_type &generator)
{
  int result=0;


  typedef boost::uniform_int<> distribution_type;
  typedef boost::variate_generator<base_generator_type&, distribution_type> gen_type;
 
 
  gen_type die_gen(generator, distribution_type(lower_bound, upper_bound));
  boost::generator_iterator<gen_type> die(&die_gen);
  result=*die;
  return result;
}
