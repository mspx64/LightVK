#include "Logger.h"
#include <chrono>
#include <unordered_map>

// Rate-limited sink - prevents console flooding
class RateLimitedSink : public spdlog::sinks::base_sink<std::mutex> {
public:
    explicit RateLimitedSink(std::shared_ptr<spdlog::sinks::sink> wrapped_sink,
                             std::chrono::milliseconds            rate_limit_interval = std::chrono::milliseconds(100))
        : wrapped_sink_(wrapped_sink),
          rate_limit_interval_(rate_limit_interval) {}

    void set_rate_limit(std::chrono::milliseconds interval) {
        std::lock_guard<std::mutex> lock(base_sink<std::mutex>::mutex_);
        rate_limit_interval_ = interval;
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        auto        now     = std::chrono::steady_clock::now();
        std::string msg_key = std::string(msg.payload.data(), msg.payload.size());

        auto it = last_log_time_.find(msg_key);

        // Always allow errors and critical
        if (msg.level >= spdlog::level::err) {
            wrapped_sink_->log(msg);
            last_log_time_[msg_key] = now;
            return;
        }

        // Rate limit other messages
        if (it == last_log_time_.end() || (now - it->second) >= rate_limit_interval_) {
            auto suppress_it = suppressed_count_.find(msg_key);
            if (suppress_it != suppressed_count_.end() && suppress_it->second > 0) {
                spdlog::details::log_msg modified_msg     = msg;
                std::string              modified_payload = fmt::format("{} (+{} suppressed)", msg_key, suppress_it->second);
                modified_msg.payload                      = modified_payload;
                wrapped_sink_->log(modified_msg);
                suppressed_count_[msg_key] = 0;
            } else {
                wrapped_sink_->log(msg);
            }

            last_log_time_[msg_key] = now;
        } else {
            suppressed_count_[msg_key]++;
        }
    }

    void flush_() override { wrapped_sink_->flush(); }

    void set_pattern_(const std::string& pattern) override { wrapped_sink_->set_pattern(pattern); }

    void set_formatter_(std::unique_ptr<spdlog::formatter> sink_formatter) override {
        wrapped_sink_->set_formatter(std::move(sink_formatter));
    }

private:
    std::shared_ptr<spdlog::sinks::sink>                                   wrapped_sink_;
    std::chrono::milliseconds                                              rate_limit_interval_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> last_log_time_;
    std::unordered_map<std::string, uint32_t>                              suppressed_count_;
};

// Custom sink with level-specific patterns
class LevelPatternSink : public spdlog::sinks::base_sink<std::mutex> {
public:
    explicit LevelPatternSink(std::shared_ptr<spdlog::sinks::sink> wrapped_sink)
        : wrapped_sink_(wrapped_sink) {
        patterns_[spdlog::level::trace]    = "[%T] [TRACE] %v";
        patterns_[spdlog::level::debug]    = "[%T] [DEBUG] %v";
        patterns_[spdlog::level::info]     = "%^[%T] %v%$";
        patterns_[spdlog::level::warn]     = "%^[%T] [WARN] [%!:%#] %v%$";
        patterns_[spdlog::level::err]      = "%^[%T] [ERROR] [%!:%#] %v%$";
        patterns_[spdlog::level::critical] = "%^[%T] [CRITICAL] [%!:%#] %v%$";
    }

    void set_pattern_for_level(spdlog::level::level_enum level, const std::string& pattern) {
        std::lock_guard<std::mutex> lock(base_sink<std::mutex>::mutex_);
        patterns_[level] = pattern;
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        auto it = patterns_.find(msg.level);
        if (it != patterns_.end()) {
            wrapped_sink_->set_pattern(it->second);
        }
        wrapped_sink_->log(msg);
    }

    void flush_() override { wrapped_sink_->flush(); }

    void set_pattern_(const std::string& pattern) override { wrapped_sink_->set_pattern(pattern); }

    void set_formatter_(std::unique_ptr<spdlog::formatter> sink_formatter) override {
        wrapped_sink_->set_formatter(std::move(sink_formatter));
    }

private:
    std::shared_ptr<spdlog::sinks::sink>                       wrapped_sink_;
    std::unordered_map<spdlog::level::level_enum, std::string> patterns_;
};

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;



std::shared_ptr<spdlog::logger>& Log::Core() {
    return s_CoreLogger;
}

void Log::Init() {
    spdlog::init_thread_pool(8192, 1);

    // Console sink with rate limiting and level-specific patterns
    auto baseConsoleSink        = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto rateLimitedConsoleSink = std::make_shared<RateLimitedSink>(baseConsoleSink, std::chrono::milliseconds(100));
    auto consoleSink            = std::make_shared<LevelPatternSink>(rateLimitedConsoleSink);
    consoleSink->set_level(spdlog::level::trace);

    // Customize console patterns
    consoleSink->set_pattern_for_level(spdlog::level::trace, "%^%v%$");
    consoleSink->set_pattern_for_level(spdlog::level::debug, "%^%v%$");
    consoleSink->set_pattern_for_level(spdlog::level::info, "%^%v%$");
    consoleSink->set_pattern_for_level(spdlog::level::warn, "%^[%n]%v%$");
    consoleSink->set_pattern_for_level(spdlog::level::err, "%^[%n]%v%$");
    consoleSink->set_pattern_for_level(spdlog::level::critical, "%^[%n]%v%$");

    // File sink with detailed patterns (no rate limiting for files)
    auto baseFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/LightVK.log", true);
    auto fileSink     = std::make_shared<LevelPatternSink>(baseFileSink);
    fileSink->set_level(spdlog::level::trace);

    // Detailed file patterns
    fileSink->set_pattern_for_level(spdlog::level::trace, "[%Y-%m-%d %T.%e] [T] [%n] %v");
    fileSink->set_pattern_for_level(spdlog::level::debug, "[%Y-%m-%d %T.%e] [D] [%n] [%!] %v");
    fileSink->set_pattern_for_level(spdlog::level::info, "[%Y-%m-%d %T.%e] [I] [%n] %v");
    fileSink->set_pattern_for_level(spdlog::level::warn, "[%Y-%m-%d %T.%e] [W] [thread %t] [%n] [%!:%#] %v");
    fileSink->set_pattern_for_level(spdlog::level::err, "[%Y-%m-%d %T.%e] [E] [thread %t] [%n] [%s:%#] [%!] %v");
    fileSink->set_pattern_for_level(spdlog::level::critical, "[%Y-%m-%d %T.%e] [C] [thread %t] [%n] [%s:%#] [%!] %v");

    s_CoreLogger = std::make_shared<spdlog::async_logger>(
        "LIGHTVK", spdlog::sinks_init_list{consoleSink, fileSink}, spdlog::thread_pool(), spdlog::async_overflow_policy::block);

    s_CoreLogger->set_level(spdlog::level::trace);
    s_CoreLogger->flush_on(spdlog::level::err);
    spdlog::register_logger(s_CoreLogger);
}

void Log::Shutdown() {
    if (s_CoreLogger)
        s_CoreLogger->flush();

    s_CoreLogger.reset();
    spdlog::shutdown();
}


void Log::LogStatus(const std::string& msg) {
    if (!s_CoreLogger)
        return;

    auto& sinks = s_CoreLogger->sinks();
    if (sinks.empty())
        return;

    static size_t last_len = 0;

    std::string padded = msg;

    // Clear leftovers from previous longer message
    if (last_len > msg.size())
        padded += std::string(last_len - msg.size(), ' ');

    last_len = padded.size();

    spdlog::details::log_msg logMsg(s_CoreLogger->name(), spdlog::level::info, "\r" + padded);

    sinks[0]->log(logMsg);
    sinks[0]->flush();
}
