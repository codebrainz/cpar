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
  /** Tried and failed to find a color name match. */
  CPAR_STATUS_NO_COLOR_NAME,
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
 * Lookup the name of a colour, if any.
 *
 * @param value The colour value.
 * @returns The name of the colour, or @c NULL if no name was found for the
 *          given colour value.
 */
const char *cpar_lookup_color_name(uint32_t value);

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
 *   * red
 *
 * The @a result parameter can be @c NULL if only want to see whether the
 * colour string can be parsed successfully but don't need the actual value.
 *
 * If no other syntax is recognized, the string is looked up in a table of
 * colour names to see if it's a pre-defined colour.
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
#include <ostream>
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

    constexpr color(uint32_t value) noexcept : value{value} {}

    constexpr color(uint8_t r = 0,
                    uint8_t g = 0,
                    uint8_t b = 0,
                    uint8_t a = 255) noexcept
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

    constexpr operator uint32_t() const noexcept { return value; }

    constexpr bool operator==(color const &other) const noexcept
    {
      return value == other.value;
    }

    constexpr bool operator!=(color const &other) const noexcept
    {
      return !operator==(other);
    }

    constexpr uint8_t red() const noexcept { return CPAR_COLOR_RED(value); }
    constexpr uint8_t green() const noexcept { return CPAR_COLOR_GREEN(value); }
    constexpr uint8_t blue() const noexcept { return CPAR_COLOR_BLUE(value); }
    constexpr uint8_t alpha() const noexcept { return CPAR_COLOR_ALPHA(value); }
  };

  inline std::string to_string(color const &c)
  {
    char buf[10] = {0};
    std::snprintf(buf,
                  10,
                  "#%02x%02x%02x%02x",
                  c.red(),
                  c.green(),
                  c.blue(),
                  c.alpha());
    return buf;
  }

  inline std::ostream &operator<<(std::ostream &out, color const &c)
  {
    out << to_string(c);
    return out;
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
      [CPAR_STATUS_SYNTAX_ERROR] = CPAR_T("syntax error"),
      [CPAR_STATUS_NO_COLOR_NAME] = CPAR_T("no matching color name"),
  };

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

static const struct cpar_color_name_info {
  const char *name;
  uint32_t value;
} cpar_color_name_table[] = {
    {"black", CPAR_COLOR_MAKE(0, 0, 0, 255)},
    {"silver", CPAR_COLOR_MAKE(192, 192, 192, 255)},
    {"gray", CPAR_COLOR_MAKE(128, 128, 128, 255)},
    {"white", CPAR_COLOR_MAKE(255, 255, 255, 255)},
    {"maroon", CPAR_COLOR_MAKE(128, 0, 0, 255)},
    {"red", CPAR_COLOR_MAKE(255, 0, 0, 255)},
    {"purple", CPAR_COLOR_MAKE(128, 0, 128, 255)},
    {"fuchsia", CPAR_COLOR_MAKE(255, 0, 255, 255)},
    {"green", CPAR_COLOR_MAKE(0, 128, 0, 255)},
    {"lime", CPAR_COLOR_MAKE(0, 255, 0, 255)},
    {"olive", CPAR_COLOR_MAKE(128, 128, 0, 255)},
    {"yellow", CPAR_COLOR_MAKE(255, 255, 0, 255)},
    {"navy", CPAR_COLOR_MAKE(0, 0, 128, 255)},
    {"blue", CPAR_COLOR_MAKE(0, 0, 255, 255)},
    {"teal", CPAR_COLOR_MAKE(0, 128, 128, 255)},
    {"aqua", CPAR_COLOR_MAKE(0, 255, 255, 255)},
    {"aliceblue", CPAR_COLOR_MAKE(240, 248, 255, 255)},
    {"antiquewhite", CPAR_COLOR_MAKE(250, 235, 215, 255)},
    {"aqua", CPAR_COLOR_MAKE(0, 255, 255, 255)},
    {"aquamarine", CPAR_COLOR_MAKE(127, 255, 212, 255)},
    {"azure", CPAR_COLOR_MAKE(240, 255, 255, 255)},
    {"beige", CPAR_COLOR_MAKE(245, 245, 220, 255)},
    {"bisque", CPAR_COLOR_MAKE(255, 228, 196, 255)},
    {"black", CPAR_COLOR_MAKE(0, 0, 0, 255)},
    {"blanchedalmond", CPAR_COLOR_MAKE(255, 235, 205, 255)},
    {"blue", CPAR_COLOR_MAKE(0, 0, 255, 255)},
    {"blueviolet", CPAR_COLOR_MAKE(138, 43, 226, 255)},
    {"brown", CPAR_COLOR_MAKE(165, 42, 42, 255)},
    {"burlywood", CPAR_COLOR_MAKE(222, 184, 135, 255)},
    {"cadetblue", CPAR_COLOR_MAKE(95, 158, 160, 255)},
    {"chartreuse", CPAR_COLOR_MAKE(127, 255, 0, 255)},
    {"chocolate", CPAR_COLOR_MAKE(210, 105, 30, 255)},
    {"coral", CPAR_COLOR_MAKE(255, 127, 80, 255)},
    {"cornflowerblue", CPAR_COLOR_MAKE(100, 149, 237, 255)},
    {"cornsilk", CPAR_COLOR_MAKE(255, 248, 220, 255)},
    {"crimson", CPAR_COLOR_MAKE(220, 20, 60, 255)},
    {"cyan", CPAR_COLOR_MAKE(0, 255, 255, 255)},
    {"darkblue", CPAR_COLOR_MAKE(0, 0, 139, 255)},
    {"darkcyan", CPAR_COLOR_MAKE(0, 139, 139, 255)},
    {"darkgoldenrod", CPAR_COLOR_MAKE(184, 134, 11, 255)},
    {"darkgray", CPAR_COLOR_MAKE(169, 169, 169, 255)},
    {"darkgreen", CPAR_COLOR_MAKE(0, 100, 0, 255)},
    {"darkgrey", CPAR_COLOR_MAKE(169, 169, 169, 255)},
    {"darkkhaki", CPAR_COLOR_MAKE(189, 183, 107, 255)},
    {"darkmagenta", CPAR_COLOR_MAKE(139, 0, 139, 255)},
    {"darkolivegreen", CPAR_COLOR_MAKE(85, 107, 47, 255)},
    {"darkorange", CPAR_COLOR_MAKE(255, 140, 0, 255)},
    {"darkorchid", CPAR_COLOR_MAKE(153, 50, 204, 255)},
    {"darkred", CPAR_COLOR_MAKE(139, 0, 0, 255)},
    {"darksalmon", CPAR_COLOR_MAKE(233, 150, 122, 255)},
    {"darkseagreen", CPAR_COLOR_MAKE(143, 188, 143, 255)},
    {"darkslateblue", CPAR_COLOR_MAKE(72, 61, 139, 255)},
    {"darkslategray", CPAR_COLOR_MAKE(47, 79, 79, 255)},
    {"darkslategrey", CPAR_COLOR_MAKE(47, 79, 79, 255)},
    {"darkturquoise", CPAR_COLOR_MAKE(0, 206, 209, 255)},
    {"darkviolet", CPAR_COLOR_MAKE(148, 0, 211, 255)},
    {"deeppink", CPAR_COLOR_MAKE(255, 20, 147, 255)},
    {"deepskyblue", CPAR_COLOR_MAKE(0, 191, 255, 255)},
    {"dimgray", CPAR_COLOR_MAKE(105, 105, 105, 255)},
    {"dimgrey", CPAR_COLOR_MAKE(105, 105, 105, 255)},
    {"dodgerblue", CPAR_COLOR_MAKE(30, 144, 255, 255)},
    {"firebrick", CPAR_COLOR_MAKE(178, 34, 34, 255)},
    {"floralwhite", CPAR_COLOR_MAKE(255, 250, 240, 255)},
    {"forestgreen", CPAR_COLOR_MAKE(34, 139, 34, 255)},
    {"fuchsia", CPAR_COLOR_MAKE(255, 0, 255, 255)},
    {"gainsboro", CPAR_COLOR_MAKE(220, 220, 220, 255)},
    {"ghostwhite", CPAR_COLOR_MAKE(248, 248, 255, 255)},
    {"gold", CPAR_COLOR_MAKE(255, 215, 0, 255)},
    {"goldenrod", CPAR_COLOR_MAKE(218, 165, 32, 255)},
    {"gray", CPAR_COLOR_MAKE(128, 128, 128, 255)},
    {"green", CPAR_COLOR_MAKE(0, 128, 0, 255)},
    {"greenyellow", CPAR_COLOR_MAKE(173, 255, 47, 255)},
    {"grey", CPAR_COLOR_MAKE(128, 128, 128, 255)},
    {"honeydew", CPAR_COLOR_MAKE(240, 255, 240, 255)},
    {"hotpink", CPAR_COLOR_MAKE(255, 105, 180, 255)},
    {"indianred", CPAR_COLOR_MAKE(205, 92, 92, 255)},
    {"indigo", CPAR_COLOR_MAKE(75, 0, 130, 255)},
    {"ivory", CPAR_COLOR_MAKE(255, 255, 240, 255)},
    {"khaki", CPAR_COLOR_MAKE(240, 230, 140, 255)},
    {"lavender", CPAR_COLOR_MAKE(230, 230, 250, 255)},
    {"lavenderblush", CPAR_COLOR_MAKE(255, 240, 245, 255)},
    {"lawngreen", CPAR_COLOR_MAKE(124, 252, 0, 255)},
    {"lemonchiffon", CPAR_COLOR_MAKE(255, 250, 205, 255)},
    {"lightblue", CPAR_COLOR_MAKE(173, 216, 230, 255)},
    {"lightcoral", CPAR_COLOR_MAKE(240, 128, 128, 255)},
    {"lightcyan", CPAR_COLOR_MAKE(224, 255, 255, 255)},
    {"lightgoldenrodyellow", CPAR_COLOR_MAKE(250, 250, 210, 255)},
    {"lightgray", CPAR_COLOR_MAKE(211, 211, 211, 255)},
    {"lightgreen", CPAR_COLOR_MAKE(144, 238, 144, 255)},
    {"lightgrey", CPAR_COLOR_MAKE(211, 211, 211, 255)},
    {"lightpink", CPAR_COLOR_MAKE(255, 182, 193, 255)},
    {"lightsalmon", CPAR_COLOR_MAKE(255, 160, 122, 255)},
    {"lightseagreen", CPAR_COLOR_MAKE(32, 178, 170, 255)},
    {"lightskyblue", CPAR_COLOR_MAKE(135, 206, 250, 255)},
    {"lightslategray", CPAR_COLOR_MAKE(119, 136, 153, 255)},
    {"lightslategrey", CPAR_COLOR_MAKE(119, 136, 153, 255)},
    {"lightsteelblue", CPAR_COLOR_MAKE(176, 196, 222, 255)},
    {"lightyellow", CPAR_COLOR_MAKE(255, 255, 224, 255)},
    {"lime", CPAR_COLOR_MAKE(0, 255, 0, 255)},
    {"limegreen", CPAR_COLOR_MAKE(50, 205, 50, 255)},
    {"linen", CPAR_COLOR_MAKE(250, 240, 230, 255)},
    {"magenta", CPAR_COLOR_MAKE(255, 0, 255, 255)},
    {"maroon", CPAR_COLOR_MAKE(128, 0, 0, 255)},
    {"mediumaquamarine", CPAR_COLOR_MAKE(102, 205, 170, 255)},
    {"mediumblue", CPAR_COLOR_MAKE(0, 0, 205, 255)},
    {"mediumorchid", CPAR_COLOR_MAKE(186, 85, 211, 255)},
    {"mediumpurple", CPAR_COLOR_MAKE(147, 112, 219, 255)},
    {"mediumseagreen", CPAR_COLOR_MAKE(60, 179, 113, 255)},
    {"mediumslateblue", CPAR_COLOR_MAKE(123, 104, 238, 255)},
    {"mediumspringgreen", CPAR_COLOR_MAKE(0, 250, 154, 255)},
    {"mediumturquoise", CPAR_COLOR_MAKE(72, 209, 204, 255)},
    {"mediumvioletred", CPAR_COLOR_MAKE(199, 21, 133, 255)},
    {"midnightblue", CPAR_COLOR_MAKE(25, 25, 112, 255)},
    {"mintcream", CPAR_COLOR_MAKE(245, 255, 250, 255)},
    {"mistyrose", CPAR_COLOR_MAKE(255, 228, 225, 255)},
    {"moccasin", CPAR_COLOR_MAKE(255, 228, 181, 255)},
    {"navajowhite", CPAR_COLOR_MAKE(255, 222, 173, 255)},
    {"navy", CPAR_COLOR_MAKE(0, 0, 128, 255)},
    {"oldlace", CPAR_COLOR_MAKE(253, 245, 230, 255)},
    {"olive", CPAR_COLOR_MAKE(128, 128, 0, 255)},
    {"olivedrab", CPAR_COLOR_MAKE(107, 142, 35, 255)},
    {"orange", CPAR_COLOR_MAKE(255, 165, 0, 255)},
    {"orangered", CPAR_COLOR_MAKE(255, 69, 0, 255)},
    {"orchid", CPAR_COLOR_MAKE(218, 112, 214, 255)},
    {"palegoldenrod", CPAR_COLOR_MAKE(238, 232, 170, 255)},
    {"palegreen", CPAR_COLOR_MAKE(152, 251, 152, 255)},
    {"paleturquoise", CPAR_COLOR_MAKE(175, 238, 238, 255)},
    {"palevioletred", CPAR_COLOR_MAKE(219, 112, 147, 255)},
    {"papayawhip", CPAR_COLOR_MAKE(255, 239, 213, 255)},
    {"peachpuff", CPAR_COLOR_MAKE(255, 218, 185, 255)},
    {"peru", CPAR_COLOR_MAKE(205, 133, 63, 255)},
    {"pink", CPAR_COLOR_MAKE(255, 192, 203, 255)},
    {"plum", CPAR_COLOR_MAKE(221, 160, 221, 255)},
    {"powderblue", CPAR_COLOR_MAKE(176, 224, 230, 255)},
    {"purple", CPAR_COLOR_MAKE(128, 0, 128, 255)},
    {"red", CPAR_COLOR_MAKE(255, 0, 0, 255)},
    {"rosybrown", CPAR_COLOR_MAKE(188, 143, 143, 255)},
    {"royalblue", CPAR_COLOR_MAKE(65, 105, 225, 255)},
    {"saddlebrown", CPAR_COLOR_MAKE(139, 69, 19, 255)},
    {"salmon", CPAR_COLOR_MAKE(250, 128, 114, 255)},
    {"sandybrown", CPAR_COLOR_MAKE(244, 164, 96, 255)},
    {"seagreen", CPAR_COLOR_MAKE(46, 139, 87, 255)},
    {"seashell", CPAR_COLOR_MAKE(255, 245, 238, 255)},
    {"sienna", CPAR_COLOR_MAKE(160, 82, 45, 255)},
    {"silver", CPAR_COLOR_MAKE(192, 192, 192, 255)},
    {"skyblue", CPAR_COLOR_MAKE(135, 206, 235, 255)},
    {"slateblue", CPAR_COLOR_MAKE(106, 90, 205, 255)},
    {"slategray", CPAR_COLOR_MAKE(112, 128, 144, 255)},
    {"slategrey", CPAR_COLOR_MAKE(112, 128, 144, 255)},
    {"snow", CPAR_COLOR_MAKE(255, 250, 250, 255)},
    {"springgreen", CPAR_COLOR_MAKE(0, 255, 127, 255)},
    {"steelblue", CPAR_COLOR_MAKE(70, 130, 180, 255)},
    {"tan", CPAR_COLOR_MAKE(210, 180, 140, 255)},
    {"teal", CPAR_COLOR_MAKE(0, 128, 128, 255)},
    {"thistle", CPAR_COLOR_MAKE(216, 191, 216, 255)},
    {"tomato", CPAR_COLOR_MAKE(255, 99, 71, 255)},
    {"turquoise", CPAR_COLOR_MAKE(64, 224, 208, 255)},
    {"violet", CPAR_COLOR_MAKE(238, 130, 238, 255)},
    {"wheat", CPAR_COLOR_MAKE(245, 222, 179, 255)},
    {"white", CPAR_COLOR_MAKE(255, 255, 255, 255)},
    {"whitesmoke", CPAR_COLOR_MAKE(245, 245, 245, 255)},
    {"yellow", CPAR_COLOR_MAKE(255, 255, 0, 255)},
    {"yellowgreen", CPAR_COLOR_MAKE(154, 205, 50, 255)},
};

static const size_t cpar_n_color_name_table =
    sizeof(cpar_color_name_table) / sizeof(cpar_color_name_table[0]);

static int cpar_strcasecmp(const char *s1, const char *s2)
{
  assert(s1);
  assert(s2);
  while (*s1 && *s2) {
    int r = tolower(*s1) - tolower(*s2);
    if (r != 0)
      return r;
    s1++;
    s2++;
  }
  return tolower(*s1) - tolower(*s2);
}

static int cpar_color_name_info_compare_name_info(const void *p1,
                                                  const void *p2)
{
  return cpar_strcasecmp((const char *)p1,
                         ((const struct cpar_color_name_info *)p2)->name);
}

static enum cpar_status cpar_color_from_name(const char *color_str,
                                             uint32_t *result)
{
  if (!color_str)
    return CPAR_STATUS_INVALID_PARAMETER;
  const struct cpar_color_name_info *info =
      (const struct cpar_color_name_info *)bsearch(
          color_str,
          cpar_color_name_table,
          cpar_n_color_name_table,
          sizeof(struct cpar_color_name_info),
          cpar_color_name_info_compare_name_info);
  if (!info)
    return CPAR_STATUS_NO_COLOR_NAME;
  if (result)
    *result = info->value;
  return CPAR_STATUS_OK;
}

static int cpar_color_name_info_compare_value(const void *p1, const void *p2)
{
  return (long)p1 - ((const struct cpar_color_name_info *)p2)->value;
}

const char *cpar_lookup_color_name(uint32_t value)
{
  const struct cpar_color_name_info *info =
      (const struct cpar_color_name_info *)bsearch(
          (void *)((long)value),
          cpar_color_name_table,
          cpar_n_color_name_table,
          sizeof(struct cpar_color_name_info),
          cpar_color_name_info_compare_value);
  if (!info)
    return NULL;
  return info->name;
}

enum cpar_status cpar_color_parse(const char *color_str, uint32_t *result)
{
  size_t color_str_len = 0;
  size_t buffer_len = 0;
  char buffer[CPAR_COLOR_PARSE_BUFFER_LEN] = {0};
  char *start = &buffer[0];
  uint8_t r = 0, g = 0, b = 0, a = 255;
  enum cpar_status status = CPAR_STATUS_OK;

  if (!color_str || !*color_str)
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
    } else {
      return CPAR_STATUS_SYNTAX_ERROR;
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

  // parse as colour name as a last resort
  else {
    uint32_t value = 0;
    if ((status = cpar_color_from_name(buffer, &value)) != CPAR_STATUS_OK)
      return status;
    if (result)
      *result = value;
    return CPAR_STATUS_OK;
  }

  return CPAR_STATUS_SYNTAX_ERROR;
}

#endif // CPAR_IMPLEMENTATION
