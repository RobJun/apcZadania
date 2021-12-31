
#include "bignum.h"
#include <iostream>
#include <cassert>

#define eval(str) ((str) ? ("True") : ("False"))

//int main(int argc, char* argv[])
int main()
{

	BigNum a{ ""};
	BigNum b{ "-1" };

	
	std::cout << a - b << std::endl;

#if SUPPORT_IFSTREAM == 1

#endif
}
