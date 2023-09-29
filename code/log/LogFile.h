#ifndef LOG_FILE_H
#define LOG_FILE_H

#include "code/util/Noncopyable.h"
#include "code/util/Util.h"
#include "code/util/TimeStamp.h"
#include <string>
#include <mutex>
#include <memory>

namespace hxk
{
class FileWriter : public  Noncopyable
{
public:
    explicit FileWriter(std::string file_name): m_written_bytes(0)
    {
        m_file = fopen(file_name.c_str(), "a+");

        if(!m_file) {
            return;
        }

        ::setbuffer(m_file, m_buffer, sizeof(m_buffer));    //文件缓冲区
    }

    ~FileWriter()
    {
        if(m_file){
            fclose(m_file);
        }
    }

    off_t get_writtrn_bytes() const
    {
        return m_written_bytes;
    }

    void append(const char* line, const size_t len)
    {
        size_t n = ::fwrite(line, 1, len, m_file);
        size_t remain = len - n;
        while (remain)
        {
            size_t x = ::fwrite(line + n , 1, remain, m_file);
            if(x == 0) {
                int err = ferror(m_file);
                if(err) {
                    fprintf(stderr, "FileWriter::append() failed!");
                }
            }
            n += x;
            remain -= x; 
        }
        m_written_bytes += len;
    }

    void flush()
    {
        fflush(m_file);
    }

private:
    FILE* m_file;
    char m_buffer[128 * 1024];
    off_t m_written_bytes;
};


class LogFile : public Noncopyable
{
public:
    LogFile(off_t roll_size, std::string basename) : m_roll_size(roll_size),m_file_index(0), m_basename(basename)
    {
        setBaseName();
        rollFile();
    }

    ~LogFile() = default;

    void append(const char* line, const size_t len)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_file_writer->append(line, len);

        if(m_file_writer->get_writtrn_bytes() > m_roll_size) {
            rollFile();
        }
    } 

    void flush()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_file_writer->flush();
    }

    void rollFile()
    {
        std::string filename = getLogFileName();
        m_file_writer.reset(new FileWriter(filename));
    }

private:
    void setBaseName()
    {
        auto last_pos = m_basename.find_last_of("/");
        if(last_pos == std::string::npos) {
            throw std::logic_error("log file name is invalid!");
        }
        std::string file_dir = m_basename.substr(0, last_pos+1);
        MakeDir(file_dir);
        m_filename = m_basename.substr(last_pos+1);
        m_basename = file_dir;
    }
    std::string getLogFileName()
    {
        TimeStamp now;
        std::string filename = m_filename;
        filename.insert(m_filename.find("."), "_" + std::to_string(m_file_index) +"@"+now.formatTime());  
        ++m_file_index;
        return m_basename + filename; 
    }

private:
    std::string m_basename;
    std::string m_filename;
    off_t m_roll_size;
    int m_file_index;
    std::mutex m_mutex;
    std::unique_ptr<FileWriter> m_file_writer;
};
}

#endif