#pragma once

#include <vector>
#include <string>

#include <unistd.h>
#include <sys/uio.h>

namespace tinyserver
{

class Buffer
{
private:
    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;

    char* begin() { return &*buffer_.begin(); }
    const char* begin() const { return &*buffer_.begin(); }

    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(writeIndex_ + len);
        }
        else
        {
            size_t readable = readableBytes();
            std::copy(begin() + readIndex_,
                        begin() + writeIndex_,
                        begin() + kCheapPrepend);
            readIndex_ = kCheapPrepend;
            writeIndex_ = readIndex_ + readable;
        }
    }

public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize),
          readIndex_(kCheapPrepend),
          writeIndex_(kCheapPrepend)
    {}

    size_t readableBytes() const { return writeIndex_ - readIndex_; }
    size_t writableBytes() const { return buffer_.size() - writeIndex_; }
    size_t prependableBytes() const { return readIndex_; }

    const char* peek() const { return begin() + readIndex_; }
    void retrieve(size_t len)
    {
        if (len < readableBytes())
            readIndex_ += len;
        else
            retrieveAll();
    }
    void retrieveAll()
    {
        readIndex_ = kCheapPrepend;
        writeIndex_ = kCheapPrepend;
    }
    std::string retrieveAsString(size_t len)
    {
        std::string str(peek(), len);
        retrieve(len);
        return str;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
            makeSpace(len);
    }

    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, begin() + writeIndex_);
        writeIndex_ += len;
    }

    char* beginWrite() { return begin() + writeIndex_; }
    const char* beginWrite() const { return begin() + writeIndex_; }

    ssize_t readFd(int fd, int* savedErrno);
    ssize_t writeFd(int fd, int* savedErrno);

};
}
