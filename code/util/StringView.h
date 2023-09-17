#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <string>
#include <ostream>
#include <functional>
namespace hxk
{

class StringView
{
public:
    StringView();
    StringView(const char*);
    explicit StringView(const std::string&);
    StringView(const char*, size_t);
    StringView(const StringView&) = default;

    const char& operator[](size_t index) const;

    const char* data() const;
    const char& front() const;
    const char& back() const;

    const char* begin() const;
    const char* end() const;

    std::size_t size() const;
    bool empty() const;

    void removePrefix(size_t n);
    void removeSuffix(size_t n);

    void swap(StringView&);
    StringView substr(std::size_t pos = 0, std::size_t count = npos) const;

    std::string toString() const;

    static constexpr std::size_t npos = std::size_t(-1);
private:
    const char* m_data;
    size_t m_len;
    /* data */  
};

bool operator==(const StringView& a, const StringView& b);
bool operator!=(const StringView& a, const StringView& b);
bool operator<(const StringView& a, const StringView& b);
bool operator<=(const StringView& a, const StringView& b);
bool operator>(const StringView& a, const StringView& b);
bool operator>=(const StringView& a, const StringView& b);

inline std::ostream& operator<< (std::ostream& os, const StringView& sv) {
    return os << sv.data();
}

}

namespace std
{
template<>
struct hash<hxk::StringView>
{
    typedef hxk::StringView argument_type;

    std::size_t operator()(const argument_type& sv) const noexcept
    {
        std::size_t result = 0;
        for(auto ch : sv) {
            result = (result*131) + ch;
        }
        return result;
    }
};
}

#endif