/**
 * @file cpar.h
 * @author Matt (m@cdbz.ca)
 * @brief A very simple function to parse CSS-like colours in C.
 * @version 0.1
 * @date 2023-07-05
 *
 * Include this header and use the @a cpar_color_parse() function to parse
 * colour strings.
 *
 * In exactly one source file, define the @a CPAR_IMPLEMENTATION macro before
 * including this header to include the implementation code.
 *
 * If you want to translate the error strings, define the @a CPAR_T()
 * function-like macro to call whatever translation function/macro is needed.
 * For example with gettext, you might define `#define CPAR_T(x) _(x)`.
 */

#ifndef CPAR_H
#define CPAR_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Status/error codes returned by @a cpar_color_parse().
 */
enum cpar_status {
  /** No error occurred. */
  CPAR_STATUS_OK,
  /** A supplied parameter did not meet preconditions, for example it was @c
   * NULL. */
  CPAR_STATUS_INVALID_PARAMETER,
  /** The supplied string was too large to fit into the working buffer. */
  CPAR_STATUS_TOO_BIG,
  /** One of the numeric components in the colour string failed to parse, for
   * example by the `strtol()` function. */
  CPAR_STATUS_INVALID_NUMBER,
  /** One of the numeric components in the colour string was outside of the
   * allowable range, for example larger than 255 for RGB components. */
  CPAR_STATUS_NUMBER_RANGE,
  /** Any other problem with the syntax of the supplied colour string, for
   * example `rgb(1,2)` is missing one of the colour components. */
  CPAR_STATUS_SYNTAX_ERROR,
};

/**
 * Returns a string describing the status code.
 *
 * The returned string is statically allocated and shouldn't be freed or
 * modified.
 *
 * @param status The status code to get description of.
 * @returns A string description of the status code or @c NULL if the status
 *          code is not valid.
 */
const char *cpar_strerror(enum cpar_status status);

/**
 * Parses a subset of CSS colours into a 32-bit integer.
 *
 * At present, the following colour syntaxes are supported and are all
 * equivalent (fully-opaque red):
 *
 *   * `#f00`
 *   * `#ff0000`
 *   * `#ff0000ff`
 *   * `rgb(255,0,0)`
 *   * `rgba(255,0,0,1)`
 *
 * The @a result parameter can be @c NULL if only want to see whether the
 * colour string can be parsed successfully but don't need the actual value.
 *
 * @param color_str The string to parse.
 * @param result Pointer to integer to store the parsed result in.
 *
 * @returns `true` on success, `false` on error. Check `errno` for the reason.
 */
enum cpar_status cpar_color_parse(const char *color_str, uint32_t *result);

/**
 * Extracts the red component from an RGBA 32-bit integer.
 *
 * @param c The 32-bit integer color.
 * @returns The 8-bit red component.
 */
#define CPAR_COLOR_RED(c) ((uint8_t)(((uint32_t)(c) >> 24) & 0xFF))

/**
 * Extracts the green component from an RGBA 32-bit integer.
 *
 * @param c The 32-bit integer color.
 * @returns The 8-bit green component.
 */
#define CPAR_COLOR_GREEN(c) ((uint8_t)(((uint32_t)(c) >> 16) & 0xFF))

/**
 * Extracts the blue component from an RGBA 32-bit integer.
 *
 * @param c The 32-bit integer color.
 * @returns The 8-bit blue component.
 */
#define CPAR_COLOR_BLUE(c) ((uint8_t)(((uint32_t)(c) >> 8) & 0xFF))

/**
 * Extracts the alpha component from an RGBA 32-bit integer.
 *
 * @param c The 32-bit integer color.
 * @returns The 8-bit alpha component.
 */
#define CPAR_COLOR_ALPHA(c) ((uint8_t)((uint32_t)(c)&0xFF))

/**
 * Packs the 8-bit RGBA components into 32-bit integer.
 *
 * @param r The red component.
 * @param g The green component.
 * @param b The blue component.
 * @param a The alpha component.
 *
 * @returns The 4 8-bit components packed into a single 32-bit integer.
 */
#define CPAR_COLOR_MAKE(r, g, b, a)          \
  (((uint32_t)(r) << 24) & 0xFF000000) |     \
      (((uint32_t)(g) << 16) & 0x00FF0000) | \
      (((uint32_t)(b) << 8) & 0x0000FF00) | ((uint32_t)(a)&0x000000FF)

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

#include <cstdio>
#include <stdexcept>
#include <string>

namespace cpar
{

  struct color {

    class error : public std::runtime_error
    {
    public:
      error(cpar_status code, const char *what)
          : std::runtime_error{what},
            m_code{code}
      {
      }

      cpar_status code() const noexcept { return m_code; }

    private:
      cpar_status m_code;
    };

    uint32_t value;

    color(uint32_t value) : value{value} {}

    color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : value{CPAR_COLOR_MAKE(r, g, b, a)}
    {
    }

    color(std::string const &str) : value{0x000000ff}
    {
      if (cpar_status status = cpar_color_parse(str.c_str(), &value);
          status != CPAR_STATUS_OK) {
        throw error{status, cpar_strerror(status)};
      }
    }

    operator uint32_t() const noexcept { return value; }

    uint8_t red() const noexcept { return CPAR_COLOR_RED(value); }
    uint8_t green() const noexcept { return CPAR_COLOR_GREEN(value); }
    uint8_t blue() const noexcept { return CPAR_COLOR_BLUE(value); }
    uint8_t alpha() const noexcept { return CPAR_COLOR_ALPHA(value); }

    std::string to_string() const noexcept
    {
      char buf[10] = {0};
      std::snprintf(buf,
                    10,
                    "#%02x%02x%02x%02x",
                    red(),
                    green(),
                    blue(),
                    alpha());
      return buf;
    }
  };

  inline std::string to_string(color const &c)
  {
    return c.to_string();
  }

} // namespace cpar

#endif

#endif // CPAR_H

#ifdef CPAR_IMPLEMENTATION
/**
 * When defined, includes the implementation code.
 *
 * Define this before including the `cpar.h` header in one and only one source
 * file. If you get undefined symbol linking errors, you probably forgot to
 * define this in one source file. If you get duplicate symbol errors, you
 * probably defined it before more than one include of the header.
 */
#define CPAR_IMPLEMENTATION 1

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define CPAR_COLOR_PARSE_BUFFER_LEN 64
#define CPAR_COLOR_PARSE_BUFFER_MAX (CPAR_COLOR_PARSE_BUFFER_LEN - 1)

#ifndef CPAR_T
/**
 * A function-like macro that can allow translating the status messages.
 *
 * @param text The text to wrap in whatever macro/function you want.
 * @returns The result of the customized version or else the original text
 *          if not customized.
 */
#define CPAR_T(text) text
#endif

const char *cpar_strerror(enum cpar_status status)
{
  static const char *error_strings[] = {
      [CPAR_STATUS_OK] = CPAR_T("success"),
      [CPAR_STATUS_INVALID_PARAMETER] = CPAR_T("invalid parameter"),
      [CPAR_STATUS_TOO_BIG] = CPAR_T("color string too big"),
      [CPAR_STATUS_INVALID_NUMBER] =
          CPAR_T("numeric component failed to parse"),
      [CPAR_STATUS_NUMBER_RANGE] = CPAR_T("numeric component out-of-range"),
      [CPAR_STATUS_SYNTAX_ERROR] = CPAR_T("syntax error")};

  if ((size_t)status > (sizeof(error_strings) / sizeof(error_strings[0]))) {
    return NULL;
  }
  return error_strings[status];
}

static enum cpar_status cpar_hex_byte_to_int(const char *hex_str,
                                             uint8_t *result)
{
  char *ep = NULL;
  int eno = errno;
  long value;
  if (!hex_str)
    return CPAR_STATUS_INVALID_PARAMETER;
  errno = 0;
  value = strtol(hex_str, &ep, 16);
  if (errno != 0) {
    return CPAR_STATUS_INVALID_NUMBER;
  } else if (value < 0 || value > 255) {
    return CPAR_STATUS_NUMBER_RANGE;
  } else if (*ep != '\0') {
    return CPAR_STATUS_INVALID_NUMBER;
  }
  errno = eno;
  if (result)
    *result = (uint8_t)value;
  return CPAR_STATUS_OK;
}

static enum cpar_status cpar_parse_component_rgb(char *str, uint8_t *out)
{
  int percent = 0;
  size_t str_len = strlen(str);
  char *ep = NULL;
  int e = errno;
  long val = 0;

  if (str[str_len - 1] == '%') {
    percent = 1;
    str[--str_len] = '\0';
  }

  errno = 0;
  val = strtol(str, &ep, 10);

  if (errno != 0 || *ep != '\0') {
    return CPAR_STATUS_INVALID_NUMBER;
  } else if (val < 0 || val > 255) {
    return CPAR_STATUS_NUMBER_RANGE;
  }

  if (out) {
    if (percent) {
      *out = (uint8_t)(((float)val / 100.0) * 255.0);
    } else {
      *out = (uint8_t)val;
    }
  }

  return CPAR_STATUS_OK;
}

static enum cpar_status cpar_parse_component_a(char *str, uint8_t *out)
{
  int e = errno;
  char *ep = NULL;
  float val = 0.0f;

  errno = 0;
  val = strtof(str, &ep);

  if (errno != 0 || *ep != '\0') {
    return CPAR_STATUS_INVALID_NUMBER;
  } else if (val < 0.0f || val > 1.0f) {
    return CPAR_STATUS_NUMBER_RANGE;
  }

  errno = e;

  if (out)
    *out = val / 255.0f;

  return CPAR_STATUS_OK;
}

static enum cpar_status cpar_parse_comma_components(char *str,
                                                    int n_comp,
                                                    uint8_t *rout,
                                                    uint8_t *gout,
                                                    uint8_t *bout,
                                                    uint8_t *aout)
{
  int i = 0;
  char *tok = strtok(str, ",");
  enum cpar_status status = CPAR_STATUS_OK;

  while (tok && i < n_comp) {
    if (i == 0) { // R
      if ((status = cpar_parse_component_rgb(tok, rout)) != CPAR_STATUS_OK)
        return status;
    } else if (i == 1) { // G
      if ((status = cpar_parse_component_rgb(tok, gout)) != CPAR_STATUS_OK)
        return status;
    } else if (i == 2) { // B
      if ((status = cpar_parse_component_rgb(tok, bout)) != CPAR_STATUS_OK)
        return status;
    } else if (i == 3) { // A
      if ((status = cpar_parse_component_a(tok, aout)) != CPAR_STATUS_OK)
        return status;
    }
    i++;
    tok = strtok(NULL, ",");
  }

  if (i != n_comp)
    return CPAR_STATUS_SYNTAX_ERROR;

  return CPAR_STATUS_OK;
}

enum cpar_status cpar_color_parse(const char *color_str, uint32_t *result)
{
  size_t color_str_len = 0;
  size_t buffer_len = 0;
  char buffer[CPAR_COLOR_PARSE_BUFFER_LEN] = {0};
  char *start = &buffer[0];
  uint8_t r = 0, g = 0, b = 0, a = 255;
  enum cpar_status status = CPAR_STATUS_OK;

  if (!color_str)
    return CPAR_STATUS_INVALID_PARAMETER;

  color_str_len = strlen(color_str);
  if (color_str_len >= CPAR_COLOR_PARSE_BUFFER_LEN)
    return CPAR_STATUS_TOO_BIG;

  // copy non-whitespace as lower-case to a local buffer we can modify
  for (size_t i = 0; i < color_str_len; i++) {
    if (!isspace(color_str[i]))
      buffer[buffer_len++] = tolower(color_str[i]);
  }

  // ensure zero-terminated
  buffer[buffer_len] = '\0';

  // parse html colors like #fff, #ffffff, #ffffffff
  if (*start == '#') {
    size_t hex_len = buffer_len - 1;
    char rr[3] = {0}, gg[3] = {0}, bb[3] = {0}, aa[3] = {0};

    start++; // skip #

    if (hex_len == 3) {
      rr[0] = *start, rr[1] = *start++;
      gg[0] = *start, gg[1] = *start++;
      bb[0] = *start, bb[1] = *start++;
      aa[0] = 'f', aa[1] = 'f';
    } else if (hex_len == 6) {
      rr[0] = *start++, rr[1] = *start++;
      gg[0] = *start++, gg[1] = *start++;
      bb[0] = *start++, bb[1] = *start++;
      aa[0] = 'f', aa[1] = 'f';
    } else if (hex_len == 8) {
      rr[0] = *start++, rr[1] = *start++;
      gg[0] = *start++, gg[1] = *start++;
      bb[0] = *start++, bb[1] = *start++;
      aa[0] = *start++, aa[1] = *start++;
    }

    if ((status = cpar_hex_byte_to_int(rr, &r)) != CPAR_STATUS_OK ||
        (status = cpar_hex_byte_to_int(gg, &g)) != CPAR_STATUS_OK ||
        (status = cpar_hex_byte_to_int(bb, &b)) != CPAR_STATUS_OK ||
        (status = cpar_hex_byte_to_int(aa, &a)) != CPAR_STATUS_OK) {
      return status;
    }

    if (result)
      *result = CPAR_COLOR_MAKE(r, g, b, a);

    return CPAR_STATUS_OK;
  }

  // parse rgb(1,2,3) colours
  else if (strstr(start, "rgb(") == start) {
    start += 4;
    buffer_len -= 4;
    if (start[--buffer_len] != ')')
      return CPAR_STATUS_SYNTAX_ERROR;
    start[buffer_len] = '\0';
    if ((status = cpar_parse_comma_components(start, 3, &r, &g, &b, &a)) !=
        CPAR_STATUS_OK) {
      return status;
    }

    if (result)
      *result = CPAR_COLOR_MAKE(r, g, b, a);

    return CPAR_STATUS_OK;
  }

  // parse rgba(1,2,50%,0.1) colours
  else if (strstr(start, "rgba(") == start) {
    start += 5;
    buffer_len -= 5;
    if (start[--buffer_len] != ')')
      return CPAR_STATUS_SYNTAX_ERROR;
    start[buffer_len] = '\0';
    if ((status = cpar_parse_comma_components(start, 4, &r, &g, &b, &a)) !=
        CPAR_STATUS_OK) {
      return status;
    }

    if (result)
      *result = CPAR_COLOR_MAKE(r, g, b, a);

    return CPAR_STATUS_OK;
  }

  return CPAR_STATUS_SYNTAX_ERROR;
}

#endif // CPAR_IMPLEMENTATION
