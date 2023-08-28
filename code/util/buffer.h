#ifndef BUFFER_H
#define BUFFER_H

#include <cstring>
#include <memory>
#include <list>

namespace hxk
{

class Buffer
{
public:
    Buffer():m_readPos(0), m_writePos(0), m_capacity(0)
    {

    }
    Buffer(const void* data, size_t size):m_readPos(0), m_writePos(0), m_capacity(0)
    {
        pushData(data, size);
    }
    Buffer(Buffer&&);
    Buffer& operator=(Buffer&&);
    Buffer(const Buffer&) = delete;
    void operator=(const Buffer&) = delete;
    ~Buffer() = default;

    std::size_t pushData(const void* data, std::size_t size);
    std::size_t pushDataAt(const void* data, std::size_t size, std::size_t offset = 0);

    void produce(std::size_t bytes);
    void consume(std::size_t bytes);

    std::size_t peekDataAt(void* pBuf, std::size_t size, std::size_t offset = 0);
    std::size_t popData(void* pBuf, std::size_t size);

    char* readAddr();
    char* writeAddr();

    bool isEmpty();

    std::size_t readableSize() const;
    std::size_t writableSize() const;
    std::size_t capacity() const;

    void shrink();
    void clear();
    void swap(Buffer& buf);

    void assureSpace(std::size_t size);

public:
    static const std::size_t kMaxBufferSize;
    static const std::size_t kHighWaterMark;
    static const std::size_t kDefaultSize;

private:
    Buffer& moveFrom(Buffer&&);

private:
    std::size_t m_readPos;
    std::size_t m_writePos;
    std::size_t m_capacity;
    std::unique_ptr<char[]> m_buffer;
};

struct BufferVector
{
    static constexpr int kMinSize = 1024;

    typedef std::list<Buffer> BufferContainer;
    typedef BufferContainer::const_iterator const_iterator;
    typedef BufferContainer::iterator iterator;
    typedef BufferContainer::value_type value_type;
    typedef BufferContainer::reference reference;
    typedef BufferContainer::const_reference const_reference;

    BufferContainer m_buffers;
    size_t m_totalBytes;

    BufferVector():m_totalBytes(0)
    {

    }

    BufferVector(Buffer&& other):m_totalBytes(0)
    {
        push(std::move(other));
    }

    void push(Buffer&& other)
    {
        m_totalBytes += other.readableSize();
        if(shouldMerge()) {
            auto& last = m_buffers.back();
            last.pushData(other.readAddr(), other.readableSize());
        }
        else {
            m_buffers.emplace_back(std::move(other));
        }
    }

    void push(const void* data, std::size_t size)
    {
        m_totalBytes += size;
        if(shouldMerge()) {
            auto& last = m_buffers.back();
            last.pushData(data, size);
        }
        else{
            m_buffers.emplace_back(Buffer(data, size));
        }
    }

    void pop()
    {
        m_totalBytes -= m_buffers.front().readableSize();
        m_buffers.pop_front();
    }

    bool empty()
    {
        return m_buffers.empty();
    }

    std::size_t totalBytes()
    {
        return m_totalBytes;
    }

    void clear()
    {
        m_buffers.clear();
        m_totalBytes = 0;
    }

    iterator begin()
    {
        return m_buffers.begin();
    }

    iterator end()
    {
        return m_buffers.end();
    }

    const_iterator begin() const
    {
        return m_buffers.begin();
    }

    const_iterator end() const
    {
        return m_buffers.end();
    }

    const_iterator cbegin() const
    {
        return m_buffers.cbegin();
    }

    const_iterator cend() const
    {
        return m_buffers.cend();
    }

private:
    bool shouldMerge() const 
    {
        if(m_buffers.empty()) {
            return false;
        }
        else{
            const auto& last = m_buffers.back();
            return last.readableSize() < kMinSize;
        }
    }
};


struct Slice
{
    const void* m_data;
    size_t m_len;

    explicit Slice(const void* d = nullptr, size_t l = 0) : m_data(d), m_len(l)
    {

    }
};

struct SliceVector
{
    typedef std::list<Slice> Slices;
    typedef Slices::const_iterator const_iterator;
    typedef Slices::iterator iterator;
    typedef Slices::value_type value_type;
    typedef Slices::reference reference;
    typedef Slices::const_reference const_reference;

    iterator begin() 
    {
        return m_slices.begin();
    }

    iterator end()
    {
        return m_slices.end();
    }

    const_iterator begin() const
    {
        return m_slices.begin();
    }

    const_iterator end() const
    {
        return m_slices.end();
    }

    const_iterator cbegin() const
    {
        return m_slices.cbegin();
    }

    const_iterator cend() const
    {
        return m_slices.cend();
    }

    bool empty()
    {
        return m_slices.empty();
    }

    void pushBack(const void* data, size_t size)
    {
        m_slices.push_back(Slice(data, size));
    }

private:
    Slices m_slices;
};


}



#endif