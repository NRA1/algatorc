#ifndef ALGATORC_OUTPUTBYTEBUF_HPP
#define ALGATORC_OUTPUTBYTEBUF_HPP
#include <iosfwd>
#include <streambuf>
#include <cmath>
#include <cstring>


class OutputByteBuf : public std::streambuf
{
public:
    OutputByteBuf()
    {
        expandBuffer(512);
    }

    int overflow(const int_type c) override
    {
        if (c == EOF) return EOF;
        if (pos_ == size_) expandBuffer(1);
        buffer_[pos_] = c;
        return c;
    }

    std::streamsize xsputn(const char* s, const std::streamsize n) override
    {
        if (pos_ + n > size_) expandBuffer(n);
        memcpy(&buffer_[pos_], s, n * sizeof(char));
        pos_ += n;
        return n;
    }

    char* takeBuffer(unsigned int& size)
    {
        size = pos_;
        char* b = buffer_;
        buffer_ = nullptr;
        return b;
    }

    ~OutputByteBuf() override
    {
        if (buffer_ != nullptr) free(buffer_);
        buffer_ = nullptr;
    }

private:
    void expandBuffer(const size_t by_min)
    {
        if (buffer_ == nullptr)
        {
            size_ = by_min;
            buffer_ = static_cast<char*>(malloc(size_ * sizeof(char)));
        }
        else
        {
            const size_t min_new_size = pos_ + by_min;
            if (min_new_size <= size_) return;
            size_ = std::pow(2, std::ceil(std::log2(min_new_size)));
            buffer_ = static_cast<char*>(realloc(buffer_, size_ * sizeof(char)));
        }
    }

    char* buffer_ = nullptr;
    size_t size_ = 0;
    size_t pos_ = 0;
};


#endif //ALGATORC_OUTPUTBYTEBUF_HPP