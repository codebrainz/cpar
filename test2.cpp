#define CONFIG_CATCH_MAIN
#include "catch_amalgamated.hpp"

#define CPAR_IMPLEMENTATION
#include "cpar.h"

static cpar_status status;
static cpar::color clr;

#define ASSIGN(x)                           \
  do {                                      \
    try {                                   \
      clr = cpar::color{x};                 \
      status = CPAR_STATUS_OK;              \
    } catch (cpar::color::error const &e) { \
      status = e.code();                    \
    }                                       \
  } while (0)

//
// Short-form HTML hex
//

TEST_CASE("#000")
{
  ASSIGN("#000");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x000000ff);
}

TEST_CASE("#fff")
{
  ASSIGN("#fff");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xffffffff);
}

TEST_CASE("#f0f")
{
  ASSIGN("#f0f");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff00ffff);
}

TEST_CASE("#0f0")
{
  ASSIGN("#0f0");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x00ff00ff);
}

TEST_CASE("#0z0")
{
  ASSIGN("#0z0");
  CHECK(status == CPAR_STATUS_INVALID_NUMBER);
}

TEST_CASE("#f")
{
  ASSIGN("#f");
  CHECK(status == CPAR_STATUS_SYNTAX_ERROR);
}

//
// Medium-form HTML hex
//

TEST_CASE("#000000")
{
  ASSIGN("#000000");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x000000ff);
}

TEST_CASE("#ff0000")
{
  ASSIGN("#ff0000");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff0000ff);
}

TEST_CASE("#00ff00")
{
  ASSIGN("#00ff00");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x00ff00ff);
}

TEST_CASE("#0000ff")
{
  ASSIGN("#0000ff");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x0000ffff);
}

TEST_CASE("#ff00ff")
{
  ASSIGN("#ff00ff");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff00ffff);
}

TEST_CASE("#00zz00")
{
  ASSIGN("#00zz00");
  CHECK(status == CPAR_STATUS_INVALID_NUMBER);
}

//
// Long-form HTML hex
//

TEST_CASE("#00000000")
{
  ASSIGN("#00000000");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x00000000);
}

TEST_CASE("#000000ff")
{
  ASSIGN("#000000ff");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x000000ff);
}

TEST_CASE("#ffffffff")
{
  ASSIGN("#ffffffff");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xffffffff);
}

TEST_CASE("#ffffff00")
{
  ASSIGN("#ffffff00");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xffffff00);
}

TEST_CASE("#ff000000")
{
  ASSIGN("#ff000000");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff000000);
}

TEST_CASE("#00ff0000")
{
  ASSIGN("#00ff0000");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x00ff0000);
}

TEST_CASE("#0000ff00")
{
  ASSIGN("#0000ff00");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x0000ff00);
}

TEST_CASE("#ff00ff00")
{
  ASSIGN("#ff00ff00");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff00ff00);
}

TEST_CASE("#ff00ff7f")
{
  ASSIGN("#ff00ff7f");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff00ff7f);
}

// too long to fit in working buffer inside parse function
TEST_CASE("#fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff")
{
  ASSIGN("#fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  CHECK(status == CPAR_STATUS_TOO_BIG);
}

//
// RGB function
//

TEST_CASE("rgb(0,0,0)")
{
  ASSIGN("rgb(0,0,0)");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x000000ff);
}

TEST_CASE("rgb(255,0,0)")
{
  ASSIGN("rgb(255,0,0)");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff0000ff);
}

TEST_CASE("rgb(0,255,0)")
{
  ASSIGN("rgb(0,255,0)");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x00ff00ff);
}

TEST_CASE("rgb(0,0,255)")
{
  ASSIGN("rgb(0,0,255)");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x0000ffff);
}

TEST_CASE("rgb(255,0,255)")
{
  ASSIGN("rgb(255,0,255)");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff00ffff);
}

TEST_CASE("rgb(127,127,255")
{
  ASSIGN("rgb(127,127,127,255)");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x7f7f7fff);
}

TEST_CASE("rgb ( 1 , 2, 3 )")
{
  ASSIGN("rgb ( 1, 2, 3 )");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x010203ff);
}

TEST_CASE("rgb(50%, 100  %, 127)")
{
  ASSIGN("rgb(50%, 100  %, 127)");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x7fff7fff);
}

//
// RGBA function
//

// TODO: write more tests for rgba()

TEST_CASE("rgba(50 %, 255, 100%, 0.5)")
{
  ASSIGN("rgba(50 %, 255, 100%, 0.5)");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x7fffff7f);
}

//
// Colour names
//

TEST_CASE("red")
{
  ASSIGN("red");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xff0000ff);
}

TEST_CASE("lightseagreen")
{
  ASSIGN("lightseagreen");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0x20b2aaff);
}

TEST_CASE("MediumOrchid")
{
  ASSIGN("MediumOrchid");
  CHECK(status == CPAR_STATUS_OK);
  CHECK(clr.value == 0xba55d3ff);
}

TEST_CASE("NOT_A_REAL_COLOR")
{
  ASSIGN("NOT_A_REAL_COLOR");
  CHECK(status == CPAR_STATUS_NO_COLOR_NAME);
}

//
// Misc tests
//

TEST_CASE("")
{
  ASSIGN("");
  CHECK(status == CPAR_STATUS_INVALID_PARAMETER);
}
