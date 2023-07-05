#define CPAR_IMPLEMENTATION 1
#include "cpar.h"

#include <stdio.h>

struct test_data {
  const char *txt;
  uint32_t clr;
  enum cpar_status exp_status;
};

static const struct test_data tests[] = {
    [0] = {"#000", 0x000000ff, CPAR_STATUS_OK},
    [1] = {"#fff", 0xffffffff, CPAR_STATUS_OK},
    [2] = {"#f0f", 0xff00ffff, CPAR_STATUS_OK},
    [3] = {"#0f0", 0x00ff00ff, CPAR_STATUS_OK},
    [4] = {"#000000", 0x000000ff, CPAR_STATUS_OK},
    [5] = {"#FFFFFF", 0xffffffff, CPAR_STATUS_OK},
    [6] = {"#00Ff00", 0x00ff00ff, CPAR_STATUS_OK},
    [7] = {"rgb(0,0,0)", 0x000000ff, CPAR_STATUS_OK},
    [8] = {"rgb(255,255,255)", 0xffffffff, CPAR_STATUS_OK},
    [9] = {"rgb(255,0,255)", 0xff00ffff, CPAR_STATUS_OK},
    [10] = {"rgb(255,0,0)", 0xff0000ff, CPAR_STATUS_OK},
    [11] = {"rgb(255,255,0)", 0xffff00ff, CPAR_STATUS_OK},
    [12] = {"rgb(0,255,0)", 0x00ff00ff, CPAR_STATUS_OK},
    [13] = {"rgb(0,255,255)", 0x00ffffff, CPAR_STATUS_OK},
    [14] = {"rgb(0,0,255)", 0x0000ffff, CPAR_STATUS_OK},
    [15] = {"rgb(100%,100%,100%)", 0xffffffff, CPAR_STATUS_OK},
    [16] = {"rgb(100%, 0, 0)", 0xff0000ff, CPAR_STATUS_OK},
    [17] = {"rgb(0, 50%,100 %)", 0x007fffff, CPAR_STATUS_OK},
};

static const size_t n_tests = sizeof(tests) / sizeof(tests[0]);

int main()
{
  enum cpar_status status = CPAR_STATUS_OK;

  for (size_t i = 0; i < n_tests; i++) {
    uint32_t val = 0x00000000;

    if ((status = cpar_color_parse(tests[i].txt, &val)) !=
        tests[i].exp_status) {
      fprintf(stderr,
              "error: cparse_color_parse: expected '%s', got '%s'\n",
              cpar_strerror(tests[i].exp_status),
              cpar_strerror(status));
    }

    int pass = val == tests[i].clr && status == tests[i].exp_status;
    printf("%s: test %lu (in={%u,%u,%u,%u}, out={%u,%u,%u,%u})\n",
           pass ? "PASS" : "FAIL",
           i,
           CPAR_COLOR_RED(tests[i].clr),
           CPAR_COLOR_GREEN(tests[i].clr),
           CPAR_COLOR_BLUE(tests[i].clr),
           CPAR_COLOR_ALPHA(tests[i].clr),
           CPAR_COLOR_RED(val),
           CPAR_COLOR_GREEN(val),
           CPAR_COLOR_BLUE(val),
           CPAR_COLOR_ALPHA(val));
  }

  return 0;
}
