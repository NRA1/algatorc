#ifndef ALGATORC_BYTEBUF_HPP
#define ALGATORC_BYTEBUF_HPP
#include <streambuf>
#include <cstring>

class InputByteBuf : public std::streambuf
{
public:
    InputByteBuf(const char* arr, int n) : arr_(arr), n_(n)
    {
        setg(const_cast<char*>(&arr_[0]), const_cast<char*>(&arr_[0]), const_cast<char*>(&arr_[n]));
    }

protected:
    std::streamsize showmanyc() override
    {
        if (gptr() == egptr()) return -1;
        return (egptr() - gptr()) / sizeof(char);
    }

    std::streamsize xsgetn(char_type* s, std::streamsize n) override
    {
        const std::streamsize total = showmanyc();
        if (total == -1) return 0;
        const std::streamsize num = std::min(n, total);
        memcpy(s, arr_, num);
        setg(eback(), gptr() + num, egptr());
        return num;
    }

private:
    const char* arr_;
    const int n_;
};


#endif //ALGATORC_BYTEBUF_HPP