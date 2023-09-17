#include "StringView.h"
#include <cstring>
#include <assert.h>

namespace hxk
{

constexpr std::size_t StringView::npos;

StringView::StringView()
{
    StringView(nullptr, 0);
}

StringView::StringView(const char* p)
{
    StringView(p, strlen(p));
}

StringView::StringView(const std::string& str)
{
    StringView(str.data(), str.size());
}

StringView::StringView(const char* p, size_t l):m_data(p), m_len(l)
{

}

const char& StringView::operator[](size_t index) const
{
    assert(index >= 0 && index < m_len);
    return m_data[index];
}

const char* StringView::data() const
{
    return m_data;
}

const char& StringView::front() const
{
    assert(m_len > 0);
    return m_data[0];
}

const char& StringView::back() const
{
    assert(m_len > 0);
    return m_data[m_len - 1];
}

const char* StringView::begin() const
{
    return m_data;
}

const char* StringView::end() const
{
    return m_data + m_len;
}

std::size_t StringView::size() const
{
    return m_len;
}

bool StringView::empty() const
{
    return m_len == 0;
}

void StringView::removePrefix(size_t n)
{
    m_data += n;
    m_len -= n;
}

void StringView::removeSuffix(size_t n)
{
    m_len -= n;
}

void StringView::swap(StringView& other)
{
    if(this != &other)
    {
        std::swap(this->m_data, other.m_data);
        std::swap(this->m_len, other.m_len);
    }
}

StringView StringView::substr(std::size_t pos = 0, std::size_t count) const
{
    return StringView(m_data + pos, std::min(count, m_len - count));
}

std::string StringView::toString() const
{
    return std::string(m_data, m_len);
}

bool operator==(const StringView& a, const StringView& b)
{
    return a.size() == b.size() && strncmp(a.data(), b.data(), a.size()) == 0;
}

bool operator!=(const StringView& a, const StringView& b)
{
    return !(a==b);
}

bool operator<(const StringView& a, const StringView& b)
{
    int res = strncmp(a.data(), b.data() , std::min(a.size(), b.size()));
    if(res != 0) {
        return res < 0;
    }
    return a.size() < b.size();
}

bool operator<=(const StringView& a, const StringView& b)
{
    return !(a > b);
}

bool operator>(const StringView& a, const StringView& b)
{
    return !(a < b || a == b);
}
bool operator>=(const StringView& a, const StringView& b)
{
    return !(a < b);
}
}