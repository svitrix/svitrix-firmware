#include "SignatureVerifier.h"

namespace
{

bool isBase64Char(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '+' || c == '/';
}

// Compute the byte length of base64-encoded data ignoring whitespace.
// Returns the count of base64 alphabet characters and padding '=' chars.
size_t base64CharCount(const String& s, size_t& padCount)
{
    size_t count = 0;
    padCount = 0;
    for (size_t i = 0; i < s.length(); ++i)
    {
        char c = s.charAt(i);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            continue;
        if (c == '=')
        {
            ++padCount;
            ++count;
            continue;
        }
        if (!isBase64Char(c))
            return 0;  // invalid character — signal via zero
        ++count;
    }
    return count;
}

}  // anonymous namespace

bool isPlausibleRsa2048SignatureBase64(const String& sigBase64)
{
    // Empty / whitespace-only → not plausible.
    String trimmed = sigBase64;
    trimmed.trim();
    if (trimmed.length() == 0)
        return false;

    size_t padCount = 0;
    const size_t total = base64CharCount(sigBase64, padCount);

    // Any invalid character → 0; > 2 padding chars → malformed base64.
    if (total == 0 || padCount > 2)
        return false;
    // Base64 string length must be a multiple of 4.
    if ((total % 4) != 0)
        return false;

    // Decoded byte length: total/4 * 3 minus padding.
    const size_t decoded = (total / 4) * 3 - padCount;

    // Allow ±2 bytes of slack around the canonical 256.
    return (decoded + 2 >= kRsa2048SignatureBytes) &&
           (decoded <= kRsa2048SignatureBytes + 2);
}

String buildSignatureUrl(const String& firmwareUrl)
{
    if (firmwareUrl.length() == 0)
        return String("");
    return firmwareUrl + ".sig";
}
