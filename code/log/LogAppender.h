#ifndef LOG_APPENDER_H
#define LOG_APPENDER_H

#include "LogLevel.h"
#include "LogFormatter.h"
#include "LogBuffer.h"
#include "lock.h"
#include "util.h"
#include "timeUtil.h"
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>

namespace hxk
{

class LogAppender
{
public:
    using _ptr = std::shared_ptr<LogAppender>;

    explicit LogAppender(LogLevel::LEVEL level = LogLevel::DEBUG);
    virtual ~LogAppender();     //基类能够析构

    virtual void log(LogLevel::LEVEL level, LogEvent::_ptr ev) = 0;

    LogFormatter::_ptr getFormatter();
    void setFormatter(LogFormatter::_ptr formatter);

protected:
    LogLevel::LEVEL m_level;
    LogFormatter::_ptr m_formatter;
    Mutex m_mutex;
};

LogAppender::LogAppender(LogLevel::LEVEL level):m_level(level)
{
}

LogAppender::~LogAppender()
{
}

LogFormatter::_ptr LogAppender::getFormatter()
{
    ScopedLock lock(&m_mutex);
    return m_formatter;
}

void LogAppender::setFormatter(LogFormatter::_ptr formatter)
{
    ScopedLock lock(&m_mutex);
    m_formatter = std::move(formatter);
}


class StdoutLogAppender : public LogAppender
{
public:
    using _ptr = std::shared_ptr<StdoutLogAppender>;

    explicit StdoutLogAppender(LogLevel::LEVEL level = LogLevel::DEBUG);
    void log(LogLevel::LEVEL level, LogEvent::_ptr ev) override;
};

StdoutLogAppender::StdoutLogAppender(LogLevel::LEVEL level):LogAppender(level)
{

}

void StdoutLogAppender::log(LogLevel::LEVEL level, LogEvent::_ptr ev)
{
    if(level < m_level) {
        return;
    }
    ScopedLock lock(&m_mutex);
    std::cout << m_formatter->format(ev);
    std::cout.flush();
}



class FileLogAppender : public LogAppender
{
public:
    using _ptr = std::shared_ptr<FileLogAppender>;

    explicit FileLogAppender(const std::string& filename, LogLevel::LEVEL level = LogLevel::DEBUG);
    ~FileLogAppender();
    void log(LogLevel::LEVEL level, LogEvent::_ptr ev);
    bool reopen();

private:
    std::string m_filename;
    std::ofstream m_file_stream;
};

FileLogAppender::FileLogAppender(const std::string& filename, LogLevel::LEVEL level)
                :LogAppender(level),m_filename(filename)
{
    if(!reopen()) {
        // todo
    }
}

FileLogAppender::~FileLogAppender()
{
}

bool FileLogAppender::reopen()
{
    ScopedLock lock(&m_mutex);

    if(!m_file_stream) {
        m_file_stream.close();
    }
    // if(!std::filesystem::exists(m_filename)){
    //     std::filesystem::create_directories(std::filesystem::path(m_filename).parent_path());   //创建对应的目录
    // }
    auto last_pos = m_filename.find_last_of("/");
    if(last_pos == std::string::npos) {
        throw std::logic_error("log file name is invalid!");
    }
    std::string full_dic = m_filename.substr(0, last_pos+1);
    MakeDir(full_dic);
    std::string full_name = m_filename.substr(last_pos+1);
    TimeUtil now;
    full_name.insert(0,now.formatTime()+"@");

    m_file_stream.open(full_dic + full_name, std::ios_base::out | std::ios_base::app);
    return !!m_file_stream;
}

void FileLogAppender::log(LogLevel::LEVEL level, LogEvent::_ptr ev)
{
    if(level < m_level) {
        return;
    }
    ScopedLock lock(&m_mutex);

    m_file_stream << m_formatter->format(ev);
    m_file_stream.flush();
}

class AsyncFileLogAppender : public LogAppender
{
public:
    using _ptr = std::shared_ptr<AsyncFileLogAppender>;
    using LogBuffer_Uptr = LogBuffer::_uptr;

    explicit AsyncFileLogAppender(const std::string& filename, LogLevel::LEVEL level = LogLevel::DEBUG, int flush_interval = 500);
    ~AsyncFileLogAppender();
    void log(LogLevel::LEVEL level, LogEvent::_ptr ev);
    bool reopen();
    void stop();
private:
    void writeThread();
private:
    std::string m_filename;
    std::ofstream m_file_stream;

    LogBuffer_Uptr m_current_buf;
    LogBuffer_Uptr m_next_buf;
    std::vector<LogBuffer_Uptr> m_bufs;

    const int m_flush_interval;
    bool m_running;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

AsyncFileLogAppender::AsyncFileLogAppender(const std::string& filename, LogLevel::LEVEL level, int flush_interval)
                :m_filename(filename), LogAppender(level), m_flush_interval(flush_interval),
                m_running(true), m_thread(std::bind(&AsyncFileLogAppender::writeThread, this), "AsyncLogInThread"),
                m_current_buf(new LogBuffer), m_next_buf(new LogBuffer)
{
    m_bufs.reserve(8);
    if(!reopen()){
        //todo
    }
}
AsyncFileLogAppender::~AsyncFileLogAppender()
{
    if(m_running) {
        stop();
    }
}

bool AsyncFileLogAppender::reopen()
{
    std::unique_lock<std::mutex> guard(m_mutex);

    if(!m_file_stream) {
        m_file_stream.close();
    }
    // if(!std::filesystem::exists(m_filename)){
    //     std::filesystem::create_directories(std::filesystem::path(m_filename).parent_path());   //创建对应的目录
    // }
    auto last_pos = m_filename.find_last_of("/");
    if(last_pos == std::string::npos) {
        throw std::logic_error("log file name is invalid!");
    }
    std::string full_dic = m_filename.substr(0, last_pos+1);
    MakeDir(full_dic);
    std::string full_name = m_filename.substr(last_pos+1);
    TimeUtil now;
    full_name.insert(0,now.formatTime()+"@");

    m_file_stream.open(full_dic + full_name, std::ios_base::out | std::ios_base::app);
    return !!m_file_stream;
}

void AsyncFileLogAppender::stop()
{
    m_running = false;
    m_thread.join();
}

void AsyncFileLogAppender::log(LogLevel::LEVEL level, LogEvent::_ptr ev)
{
    if(level < m_level) {
        return;
    }
    std::unique_lock<std::mutex> guard(m_mutex);
    if(!m_current_buf->append(m_formatter->format(ev))) {
        m_bufs.emplace_back(std::move(m_current_buf));
        if(m_next_buf){
            m_current_buf = std::move(m_next_buf);
        }
        else{
            m_current_buf.reset(new LogBuffer);
        }
        if(!m_current_buf->append(m_formatter->format(ev))){
            throw std::logic_error("buffer append error");
        }
        m_cond.notify_one();
    }
}

void AsyncFileLogAppender::writeThread()
{
    std::vector<LogBuffer_Uptr> buffers_to_write;
    LogBuffer_Uptr new_buf1(new LogBuffer);
    LogBuffer_Uptr new_buf2(new LogBuffer);
    while (m_running)
    {
        {
            std::unique_lock<std::mutex> guard(m_mutex);
            m_cond.wait_for(guard, std::chrono::milliseconds(m_flush_interval));
            m_bufs.push_back(std::move(m_current_buf));
            m_current_buf = std::move(new_buf1);
            buffers_to_write.swap(m_bufs);
            if(!m_next_buf){
                m_next_buf = std::move(new_buf2);
            }
        }
        assert(!buffers_to_write.empty());
        if(buffers_to_write.size() > 16) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Dropped log messages %zd larger buffers\n", buffers_to_write.size() - 2);
            fputs(buf, stderr);
            // 如果日志太多，则丢掉一些数据
            buffers_to_write.erase(buffers_to_write.begin() + 2, buffers_to_write.end());
        }
        for(const auto& buf : buffers_to_write) {
            m_file_stream << buf->data();
        }
        if(buffers_to_write.size() > 2){
            buffers_to_write.resize(2);
        }
        if(!new_buf1){
            assert(!buffers_to_write.empty());
            new_buf1 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buf1->clear();
        }
        if(!new_buf2){
            assert(!buffers_to_write.empty());
            new_buf2 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buf2->clear();
        }
        buffers_to_write.clear();
        m_file_stream.flush();
    }
    m_file_stream.flush();
}

}

#endif