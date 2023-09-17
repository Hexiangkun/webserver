#include "Buffer.h"
#include <limits>
#include <assert.h>

namespace hxk
{
inline static std::size_t RoundUp2Power(std::size_t size) {
    if (size == 0)
        return 0;

    std::size_t roundUp = 1;
    while (roundUp < size)
        roundUp *= 2;

    return roundUp;
}

const std::size_t Buffer::kMaxBufferSize = std::numeric_limits<std::size_t>::max() / 2;
const std::size_t Buffer::kHighWaterMark = 1 * 1024;
const std::size_t Buffer::kDefaultSize = 256;

Buffer::Buffer(Buffer&& other)
{
    moveFrom(std::move(other));
}

Buffer& Buffer::operator=(Buffer&& other)
{
    return moveFrom(std::move(other));
}

Buffer& Buffer::moveFrom(Buffer&& other)
{
    if(this != &other){
        this->m_readPos = other.m_readPos;
        this->m_writePos = other.m_writePos;
        this->m_capacity = other.m_capacity;
        this->m_buffer = std::move(other.m_buffer);

        other.clear();
        other.shrink();
    }
    return *this;
}

std::size_t Buffer::pushData(const void* data, std::size_t size)
{
    std::size_t bytes = pushDataAt(data, size);
    produce(bytes);
    
    assert(bytes == size);
    return bytes;
}

std::size_t Buffer::pushData(const std::string& str)
{
    pushData(str.data(), str.size());
}

std::size_t Buffer::pushDataAt(const void* data, std::size_t size, std::size_t offset)
{
    if(!data || size == 0)
    {
        return 0;
    }
    if(readableSize() + size + offset >= kMaxBufferSize) { // ?
        return 0;
    }

    assureSpace(size + offset);
    assert(size + offset <= writableSize());

    ::memcpy(&m_buffer[m_writePos + offset], data, size);
    return size;
}

std::size_t Buffer::popData(void* pBuf, std::size_t size)
{
    std::size_t bytes = peekDataAt(pBuf, size);
    consume(bytes);
    return bytes;
}

std::size_t Buffer::peekDataAt(void* pBuf, std::size_t size, std::size_t offset)
{
    const std::size_t dataSize = readableSize();
    if(!pBuf || size == 0 || dataSize <= offset) {
        return 0;
    }

    if(size + offset > dataSize){
        size = dataSize - offset;
    }
    ::memcpy(pBuf, &m_buffer[m_readPos + offset], size);
    return size;
}

void Buffer::produce(std::size_t bytes)
{
    m_writePos += bytes;
}

void Buffer::consume(std::size_t bytes)
{
    assert(m_readPos + bytes <= m_writePos);
    
    m_readPos += bytes;

    if(isEmpty()){  //没有可读内容，清除所有数据，回到原点
        clear();
    }
}

void Buffer::assureSpace(std::size_t needSize)
{
    if(writableSize() >= needSize) {
        return;
    }

    const size_t dataSize = readableSize();
    const size_t old_capacity = m_capacity;

    while(writableSize() + m_readPos < needSize) {//未使用的空间小于即将要使用的空间
        if(m_capacity < kDefaultSize) { 
            m_capacity = kDefaultSize;
        }
        else if(m_capacity <= kMaxBufferSize) {
            const auto newCapacity = RoundUp2Power(m_capacity);
            if(m_capacity < newCapacity) {
                m_capacity = newCapacity;
            }
            else {
                m_capacity = 3*newCapacity / 2;
            }
        }
        else {
            assert(false);
        }
    }
    if(old_capacity < m_capacity) { //扩容
        std::unique_ptr<char[]> tmp(new char[m_capacity]);

        if(dataSize != 0){
            ::memcpy(&tmp[0], &m_buffer[m_readPos], dataSize);
        }
        m_buffer.swap(tmp);
    }
    else {  //未扩容
        assert(m_readPos > 0);
        ::memmove(&m_buffer[0], &m_buffer[m_readPos], dataSize);
    }

    m_readPos = 0;
    m_writePos = dataSize;
}

char* Buffer::readAddr()
{
    return &m_buffer[m_readPos];
}

char* Buffer::writeAddr()
{
    return &m_buffer[m_writePos];
}

bool Buffer::isEmpty()
{
    return readableSize() == 0;
}

std::size_t Buffer::readableSize() const
{
    return m_writePos - m_readPos;
}

std::size_t Buffer::writableSize() const
{
    return m_capacity - m_writePos;
}

std::size_t Buffer::capacity() const
{
    return m_capacity;
}

void Buffer::shrink()
{
    if(isEmpty()) {
        if(m_capacity > 8*1024) {
            clear();
            m_capacity = 0;
            m_buffer.reset();
        }

        return;
    }

    std::size_t old_capacity = m_capacity;
    std::size_t dataSize = readableSize();

    if(dataSize > old_capacity / 4) {
        return;
    }
    std::size_t new_Capacity = RoundUp2Power(dataSize);

    std::unique_ptr<char[]> tmp(new char[new_Capacity]);
    memcpy(&tmp[0], &m_buffer[m_readPos], dataSize);
    m_buffer.swap(tmp);
    m_capacity = new_Capacity;
    m_readPos = 0;
    m_writePos = dataSize;
}

void Buffer::clear()
{
    m_readPos = 0;
    m_writePos = 0;
}

void Buffer::swap(Buffer& buf)
{
    std::swap(m_readPos , buf.m_readPos);
    std::swap(m_writePos , buf.m_writePos);
    std::swap(m_capacity, buf.m_capacity);
    m_buffer.swap(buf.m_buffer);
}

std::string Buffer::to_string()
{
    return std::string(readAddr(), readableSize());
}

} // namespace hxk
