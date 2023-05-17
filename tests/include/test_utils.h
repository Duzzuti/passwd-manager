#pragma once
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <iostream>
/*
this header provides some functions that are needed in the unittests 
but not anywhere else
*/
std::string gen_random_string(const unsigned int len);   //generates a random string with a given length


#endif //(TEST_UTILS_H)