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
#include <ilcplex/ilocplex.h>
using namespace std;


#include <random>
typedef std::mt19937 base_generator_type;

#include "parameter.h"
#include "file.h"
#include "input.h"
#include "exact.h"


#ifdef DISABLE_COUT
#define cout 0 && std::cout
#endif