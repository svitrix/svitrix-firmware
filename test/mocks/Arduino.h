#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

typedef uint8_t byte;

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef F
#define F(x) (x)
#endif

#ifndef map
#define map(x, in_min, in_max, out_min, out_max) \
    (((long)(x) - (long)(in_min)) * ((long)(out_max) - (long)(out_min)) / ((long)(in_max) - (long)(in_min)) + (long)(out_min))
#endif

// Controllable millis() for testing time-dependent code
static uint32_t _mock_millis_value = 0;

inline uint32_t millis() { return _mock_millis_value; }
inline void setMockMillis(uint32_t ms) { _mock_millis_value = ms; }

// Arduino-compatible random()
inline long random(long max) { return max > 0 ? (rand() % max) : 0; }
inline long random(long min, long max) { return max > min ? (min + rand() % (max - min)) : min; }

// Minimal String class compatible with Arduino String API
class String {
    std::string _str;

public:
    String() {}
    String(const char *s) : _str(s ? s : "") {}
    String(const std::string &s) : _str(s) {}
    String(char c) : _str(1, c) {}
    String(int val) : _str(std::to_string(val)) {}
    String(unsigned int val) : _str(std::to_string(val)) {}
    String(long val) : _str(std::to_string(val)) {}
    String(float val, unsigned int decimalPlaces = 2) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.*f", decimalPlaces, val);
        _str = buf;
    }

    unsigned int length() const { return _str.length(); }
    const char *c_str() const { return _str.c_str(); }
    char charAt(unsigned int index) const {
        if (index < _str.length()) return _str[index];
        return 0;
    }
    char operator[](unsigned int index) const {
        return charAt(index);
    }

    int indexOf(char c, unsigned int fromIndex = 0) const {
        size_t pos = _str.find(c, fromIndex);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }
    int indexOf(const char *s, unsigned int fromIndex = 0) const {
        size_t pos = _str.find(s, fromIndex);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }
    int indexOf(const String &s, unsigned int fromIndex = 0) const {
        size_t pos = _str.find(s._str, fromIndex);
        return (pos == std::string::npos) ? -1 : (int)pos;
    }

    bool isEmpty() const { return _str.empty(); }

    String &operator+=(const String &rhs) {
        _str += rhs._str;
        return *this;
    }
    String &operator+=(const char *rhs) {
        _str += rhs;
        return *this;
    }
    String &operator+=(char c) {
        _str += c;
        return *this;
    }

    String operator+(const String &rhs) const { return String(_str + rhs._str); }
    String operator+(const char *rhs) const { return String(_str + rhs); }

    bool operator==(const String &rhs) const { return _str == rhs._str; }
    bool operator==(const char *rhs) const { return _str == rhs; }
    bool operator!=(const String &rhs) const { return _str != rhs._str; }
    bool operator<(const String &rhs) const { return _str < rhs._str; }

    void concat(char c) { _str += c; }
    void replace(const String &from, const String &to) {
        size_t pos = 0;
        while ((pos = _str.find(from._str, pos)) != std::string::npos) {
            _str.replace(pos, from._str.length(), to._str);
            pos += to._str.length();
        }
    }

    String substring(unsigned int beginIndex, unsigned int endIndex = 0xFFFFFFFF) const {
        if (endIndex == 0xFFFFFFFF) endIndex = _str.length();
        return String(_str.substr(beginIndex, endIndex - beginIndex));
    }

    int toInt() const { return atoi(_str.c_str()); }
    void trim() {
        size_t start = _str.find_first_not_of(" \t\r\n");
        size_t end = _str.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) {
            _str.clear();
        } else {
            _str = _str.substr(start, end - start + 1);
        }
    }

    bool startsWith(const char *prefix) const {
        return _str.rfind(prefix, 0) == 0;
    }
    bool equals(const String &s) const { return _str == s._str; }
    bool equalsIgnoreCase(const String &s) const {
        if (_str.length() != s._str.length()) return false;
        for (size_t i = 0; i < _str.length(); i++) {
            if (tolower(_str[i]) != tolower(s._str[i])) return false;
        }
        return true;
    }
    bool equalsIgnoreCase(const char *s) const {
        return equalsIgnoreCase(String(s));
    }

    friend String operator+(const char *lhs, const String &rhs) {
        return String(std::string(lhs) + rhs._str);
    }
};

// Minimal Serial stub
struct SerialMock {
    void print(const char *) {}
    void print(int) {}
    void print(unsigned long) {}
    void println(const char *) {}
    void println(const String &) {}
    void println(int) {}
    void println() {}
    void printf(const char *, ...) {}
    void begin(unsigned long) {}
};

static SerialMock Serial;

// toupper is in <cctype> but Arduino provides it globally
#include <cctype>

#endif // MOCK_ARDUINO_H
