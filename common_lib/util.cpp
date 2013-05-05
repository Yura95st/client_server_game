#include "util.h"
#include <sstream>


util::util(void)
{
}


util::~util(void)
{
}

string util::itos(int i){
	stringstream ss;
	ss << i;
	return ss.str();
}