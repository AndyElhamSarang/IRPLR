#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <math.h>
#include <limits>
#include <time.h>
#include <string>
#include <set>
#include <limits>
#include <assert.h>

using namespace std;


#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/additive_combine.hpp> 
typedef boost::mt19937 base_generator_type;

#include "parameter.h"
#include "file.h"
#include "input.h"
#include "preprocessing.h"
#include "random.h"
#include "hgs.h"
#include "solution.h"
#include "solution_construction.h"
#include "solution_improvement.h"