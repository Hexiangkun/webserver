#include "util.h"

namespace hxk
{

long GetThreadID()
{
    return ::syscall(SYS_gettid);
}

uint64_t GetFiberID()
{
    return 0;
}

void BackTrace(std::vector<std::string>& out, int size, int skip)
{
    void** void_ptr_list = (void**)malloc(sizeof(void*) *size);
    int call_stack_count = ::backtrace(void_ptr_list, size);
    char** string_list = ::backtrace_symbols(void_ptr_list,call_stack_count);
    if(string_list == NULL){
        std::cerr << "Backtrace() exception!" << std::endl;
        return;
    }

    for(int i = skip; string_list && i < call_stack_count; i++){
        std::stringstream ss;
        char* str = string_list[i];
        char* bracket_pos = nullptr;
        char* plus_pos = nullptr;
        //找到左括号位置
        for(bracket_pos = str; *bracket_pos != '(' && *bracket_pos; bracket_pos++){

        }
        assert(*bracket_pos == '(');
        *bracket_pos = '\0';    //把左括号之前的字符串塞进字符串流中
        ss << string_list[i] << '(' ;
        *bracket_pos = '(';

        //找到+号位置
        for(plus_pos = bracket_pos; *plus_pos != '+' && *plus_pos; plus_pos++){

        }

        char* type = nullptr;
        if(*bracket_pos + 1 != *plus_pos){
            *plus_pos = '\0';
            int status;
            type = abi::__cxa_demangle(bracket_pos + 1, nullptr, nullptr, &status);
            assert(status == 0 || status == -2);
            ss << (status == 0) ? type : bracket_pos +1;    //status=-2，代表字符串解析错误
            *plus_pos = '+';
        }

        ss << plus_pos; //把剩下的字符串塞进去
        out.push_back(ss.str());
        free(type);
    }
    free(string_list);
    free(void_ptr_list);
}

std::string BackTraceToString(int size , int skip)
{
    std::vector<std::string> call_stack;
    BackTrace(call_stack, size, skip);
    std::stringstream ss;
    for(const auto& item : call_stack){
        ss << item << std::endl;
    }
    return ss.str();
}

uint64_t GetCurrentMS()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t GetCurrentUS()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul * 1000ul +tv.tv_usec;
}

bool MakeDir(const char* dir) {
    if (mkdir(dir, 0755) != 0) {
        if (EEXIST != errno) {
            perror("MakeDir failed:");
            return false;
        }
    }

    return true;
}

bool MakeDir(const std::string& filepath)
{
    if(!std::filesystem::exists(filepath)){
        auto res = std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());   //创建对应的目录
        if(!res) {
            std::cout << "创建目录失败" << std::endl;
        }
    }
    else {
        std::cout << "存在文件目录" << std::endl;
    }
}

std::vector<std::string> SplitString(const std::string& str, char seperator)
{
    std::vector<std::string> result;

    std::string::size_type start = 0;
    std::string::size_type sep = str.find(seperator);

    while(sep != std::string::npos)
    {
        if(start < sep) {
            result.emplace_back(str.substr(start, sep-start));
        }
        start = sep + 1;
        sep = str.find(seperator);
    }

    if(start != str.size())
    {
        result.emplace_back(str.substr(start));
    }
    return result;
}

}