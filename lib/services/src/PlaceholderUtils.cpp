/**
 * @file PlaceholderUtils.cpp
 * @brief Implementation of template placeholder substitution.
 */
#include "PlaceholderUtils.h"

String replacePlaceholdersWith(const String &input,
                               const std::function<String(const String &)> &getValue)
{
    String text = input;
    int start = 0;
    while ((start = text.indexOf("{{", start)) != -1)
    {
        int end = text.indexOf("}}", start);
        if (end == -1)
        {
            break;
        }
        String key = text.substring(start + 2, end);
        String value = getValue(key);
        String token = "{{" + key + "}}";
        text.replace(token, value);
        start += value.length();
    }
    return text;
}
