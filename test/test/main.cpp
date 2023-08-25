#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <sys/stat.h>
#include <algorithm>

namespace fs = std::filesystem;

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

int test_creatDirectories() {
    std::string filePath = "/home/hxk/C++Project/webserver/test/test/your/log.log";
    auto last_pos = filePath.find_last_of("/");
    if(last_pos == std::string::npos) {
        throw std::logic_error("log file name is invalid!");
    }
    std::string full_dic = filePath.substr(0, last_pos+1);
    std::string full_name = filePath.substr(last_pos+1);
    full_name.insert(0, "ni");
    std::cout << full_dic << " " << full_name << std::endl;

    MakeDir(full_dic);
    // if(fs::exists(filePath)){
    //     std::cout << "存在文件目录" << std::endl;
    // }
    // else
    // {
    //     auto res = fs::create_directories(fs::path(filePath).parent_path());
    //     if(!res){
    //         std::cout << "创建目录失败" << std::endl;
    //     }

    // }
    // 创建缺失的目录
    
    //创建文件并写入内容
    std::ofstream file(full_dic+full_name);
    if (file.is_open()) {
        file << "Hello, World!";
        file.close();
        std::cout << "文件已成功创建并写入内容。\n";
    } else {
        std::cout << "无法打开文件。\n";
    }
    return 0;
}


void test_vec_reserve_resize()
{
    std::vector<char> strs;
    strs.reserve(50);
    
    std::cout << strs.size() << " " << strs.capacity() << std::endl;
    strs.emplace_back('a');
    std::cout << strs.size() << " " << strs.capacity() << std::endl;
    strs.clear();
    std::cout << strs.size() << " " << strs.capacity() << std::endl;
}


int test_color() {
    std::ofstream file("output.txt");
    if (file.is_open()) {
        file << "\033[1;31;40mThis is some red text.\033[1;34;40m" << std::endl;
        std::cout << "\033[1;31;40mThis is some red text.\033[1;34;40m" << std::endl;
        file.close();
    } else {
        std::cout << "Failed to open file." << std::endl;
    }
    return 0;
}

template <size_t... N>
void print(std::index_sequence<N...>) {
    std::vector<int> res;
    (void)std::initializer_list<int>{
        ((res.push_back(N), std::cout << N << " "), 0)...};
  //std::for_each(res.begin(), res.end(), [](int x) {std::cout << x << " ";});
}


int test_index_seq() {
  auto t = std::make_index_sequence<10>();
  print(t);
  return 0;
}


void test_remove()
{
    std::vector<int> vec = {1,2,3,4,5,2};
    vec.erase(std::remove(vec.begin(), vec.end(), 2), vec.end());

    for(const auto& v : vec) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
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


void test_sperator()
{
    std::string filePath = "/home/hxk/C++Project/webserver/test/test/your/file.txt";
}

int main()
{
    // test_vec_reserve_resize();
    // test_color();
    // test_index_seq();
    // test_remove();
    // MakeDir("/log/log");
    test_creatDirectories();
}