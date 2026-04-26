/**
 * @file FormatStringValidator.h
 * @brief Whitelist validation for user-supplied printf format strings.
 *
 * Defends snprintf/printf call sites that accept format strings from
 * untrusted sources (e.g. DataFetcher displayFormat from
 * POST /api/datafetcher) against CWE-134 format-string injection.
 *
 * Header-only so it works in both Arduino and native_test builds.
 *
 * Accepted grammar (single-argument formats only):
 *
 *     fmt        := ( text | "%%" | spec )*
 *     spec       := "%" flags? width? precision? conv
 *     flags      := [+-# 0]+
 *     width      := digit{1,2}
 *     precision  := "." digit{0,2}
 *     conv       := one of: d i u o x X f g e E s
 *
 * Rejected explicitly:
 *   - More than one non-%% specifier (avoids reading uninitialized varargs)
 *   - %n %p %c %a %A %h-family length modifiers and any other letter
 *   - %*  %.*  (variable width/precision — extra varargs slot)
 *   - %N$ positional arguments
 *   - 3+ digit width or precision (heap exhaustion / DoS)
 *   - Truncated specs (e.g. trailing "%")
 */
#pragma once

inline bool isSafeSingleArgFormat(const char* fmt)
{
    if (!fmt)
        return true;

    int args = 0;
    const char* p = fmt;

    while (*p)
    {
        if (*p != '%')
        {
            ++p;
            continue;
        }

        ++p; // consume '%'

        // Literal "%%"
        if (*p == '%')
        {
            ++p;
            continue;
        }

        // Flags: any of + - # space 0 (zero or more)
        while (*p == '+' || *p == '-' || *p == '#' || *p == ' ' || *p == '0')
            ++p;

        // Reject variable width
        if (*p == '*')
            return false;

        // Width: at most two digits
        int widthDigits = 0;
        while (*p >= '0' && *p <= '9')
        {
            if (++widthDigits > 2)
                return false;
            ++p;
        }

        // Reject positional argument syntax (e.g. "%1$d")
        if (*p == '$')
            return false;

        // Optional precision
        if (*p == '.')
        {
            ++p;
            if (*p == '*')
                return false;
            int precDigits = 0;
            while (*p >= '0' && *p <= '9')
            {
                if (++precDigits > 2)
                    return false;
                ++p;
            }
        }

        // Conversion character: whitelist
        switch (*p)
        {
            case 'd': case 'i': case 'u': case 'o':
            case 'x': case 'X':
            case 'f': case 'g': case 'e': case 'E':
            case 's':
                break;
            default:
                return false;
        }
        ++p;

        if (++args > 1)
            return false;
    }

    return true;
}
