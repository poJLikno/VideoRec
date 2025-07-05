#ifndef TEXT_UTILS_H_
#define TEXT_UTILS_H_

#include <string.h>
#include <windows.h>

/* Need free memory after use (delete[] ...) */
inline char *to_utf8(const wchar_t *w_str, int &bytes_were_written)
{
    int symbols_count = (int)wcslen(w_str) + 1;/* + null-symbol */
    int need_bytes_count = WideCharToMultiByte(CP_UTF8, 0, w_str, symbols_count, nullptr, 0, nullptr, nullptr);/* Include null-symbol */

    char *str = new char[need_bytes_count] { 0 };
    if (str == nullptr)
    {
        return nullptr;
    }

    bytes_were_written = WideCharToMultiByte(CP_UTF8, 0, w_str, symbols_count, str, need_bytes_count, nullptr, nullptr);

    return str;
}

/* Need free memory after use (delete[] ...) */
inline char *to_utf8(const wchar_t *w_str)
{
    int tmp = 0;
    return to_utf8(w_str, tmp);
}

/* Need free memory after use (delete[] ...) */
inline wchar_t *to_utf16(const char *str, int &symbols_were_written)
{
    int bytes_count = (int)strlen(str) + 1;/* + null-symbol */
    int w_need_symbols_count = MultiByteToWideChar(CP_UTF8, 0, str, bytes_count, nullptr, 0);/* Include null-symbol */

    wchar_t *w_str = new wchar_t[w_need_symbols_count] { 0 };
    if (w_str == nullptr)
    {
        return nullptr;
    }

    symbols_were_written = MultiByteToWideChar(CP_UTF8, 0, str, bytes_count, w_str, w_need_symbols_count);

    return w_str;
}

/* Need free memory after use (delete[] ...) */
inline wchar_t *to_utf16(const char *str)
{
    int tmp = 0;
    return to_utf16(str, tmp);
}

#endif /* TEXT_UTILS_H_ */
