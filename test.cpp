#define CPAR_IMPLEMENTATION 1
#include "cpar.h"

#include <array>
#include <cstdio>
#include <iostream>
#include <string>

struct test_data {
  uint32_t index;
  std::string txt;
  uint32_t clr;
  cpar_status exp_status;
};

static const test_data tests[] = {
    {0, "#000", 0x000000ff, CPAR_STATUS_OK},
    {1, "#fff", 0xffffffff, CPAR_STATUS_OK},
    {2, "#f0f", 0xff00ffff, CPAR_STATUS_OK},
    {3, "#0f0", 0x00ff00ff, CPAR_STATUS_OK},
    {4, "#000000", 0x000000ff, CPAR_STATUS_OK},
    {5, "#FFFFFF", 0xffffffff, CPAR_STATUS_OK},
    {6, "#00Ff00", 0x00ff00ff, CPAR_STATUS_OK},
    {7, "rgb(0,0,0)", 0x000000ff, CPAR_STATUS_OK},
    {8, "rgb(255,255,255)", 0xffffffff, CPAR_STATUS_OK},
    {9, "rgb(255,0,255)", 0xff00ffff, CPAR_STATUS_OK},
    {10, "rgb(255,0,0)", 0xff0000ff, CPAR_STATUS_OK},
    {11, "rgb(255,255,0)", 0xffff00ff, CPAR_STATUS_OK},
    {12, "rgb(0,255,0)", 0x00ff00ff, CPAR_STATUS_OK},
    {13, "rgb(0,255,255)", 0x00ffffff, CPAR_STATUS_OK},
    {14, "rgb(0,0,255)", 0x0000ffff, CPAR_STATUS_OK},
    {15, "rgb(100%,100%,100%)", 0xffffffff, CPAR_STATUS_OK},
    {16, "rgb(100%, 0, 0)", 0xff0000ff, CPAR_STATUS_OK},
    {17, "rgb(0, 50%,100 %)", 0x007fffff, CPAR_STATUS_OK},
    {18, "rgb(256,256,256,1)", 0, CPAR_STATUS_NUMBER_RANGE},
    {19, "rgba(1,2,3,1.2)", 0, CPAR_STATUS_NUMBER_RANGE},
    {20, "#f", 0, CPAR_STATUS_SYNTAX_ERROR},
    {21, "rgb(1, 2, hello)", 0, CPAR_STATUS_INVALID_NUMBER},
    {22, "#aaZZbb", 0, CPAR_STATUS_INVALID_NUMBER},
    {23,
     "#fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
     0,
     CPAR_STATUS_TOO_BIG},
    {24, "", 0, CPAR_STATUS_INVALID_PARAMETER},
};

static const size_t n_tests = sizeof(tests) / sizeof(tests[0]);

int main()
{
  cpar_status status = CPAR_STATUS_OK;

  for (size_t i = 0; i < n_tests; i++) {
    try {
      cpar::color clr = tests[i].txt;
      cpar::color exp = tests[i].clr;

      if (clr.value == tests[i].clr) {
        std::cout << "pass: test " << i << " passed, expected=" << exp
                  << ", actual=" << clr << std::endl;
      } else {
        std::cout << "fail: test " << i << " failed, expected=" << exp
                  << ", actual=" << clr << std::endl;
      }

    } catch (cpar::color::error const &e) {
      if (e.code() != tests[i].exp_status) {
        std::cerr << "fail: test " << i
                  << "failed, unexpected color parse failure " << e.code()
                  << " on test " << static_cast<int>(e.code()) << std::endl;
      } else {
        std::cout << "pass: test " << i
                  << " passed, expected color parse failure on test "
                  << static_cast<int>(e.code()) << std::endl;
      }
      continue;
    }
  }

  return 0;
}
