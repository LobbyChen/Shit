// product.cpp
// 前面 n 个数乘起来是 P,数开区间 (P, T) 里有几个整数。
// 一个都没有就输出 Not Found。
//
// g++ -O2 -std=c++11 product.cpp -o product
//
// 注:这个题数很大,long long 装不下。标准库里没有大数,外面找的库不让用,
// 只能自己写,写在下面了。2023.9 比赛前夜赶的,别细看。

#include <iostream>
#include <string>
#include <vector>

// ====== 大数 ======
// 十进制一位一位存,低位在前。本来想用 char 存,怕溢出,就用了 int。
struct Dashu {
    int sign;                  // 1 或 -1。0 的 sign 也是 1,懒得特判
    std::vector<int> wei;      // 每一位,wei[0] 是个位

    Dashu() : sign(1) { wei.push_back(0); }
};

static bool shi_ling(const Dashu& x) {
    return x.wei.size() == 1 && x.wei[0] == 0;
}

// 去掉高位的 0
static void guiyi(Dashu& x) {
    while (x.wei.size() > 1 && x.wei.back() == 0) x.wei.pop_back();
    if (shi_ling(x)) x.sign = 1;
}

// 从字符串进来。不信任 atoi(它会溢出),一个个字符收。
static Dashu cong_zifuchuan(const std::string& s) {
    Dashu x;
    x.wei.clear();
    size_t i = 0;
    bool fu = false;
    if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
        fu = (s[i] == '-');
        ++i;
    }
    if (i >= s.size()) {
        std::cerr << "这不是数字: " << s << std::endl;
        std::exit(2);
    }
    for (size_t k = s.size(); k > i; --k) {   // 从低位往高位收
        char ch = s[k - 1];
        if (ch < '0' || ch > '9') {
            std::cerr << "有不是数字的字符: " << s << std::endl;
            std::exit(2);
        }
        x.wei.push_back(ch - '0');
    }
    x.sign = fu ? -1 : 1;
    guiyi(x);
    return x;
}

static std::string dao_zifuchuan(const Dashu& x) {
    std::string s;
    if (x.sign < 0) s.push_back('-');
    for (size_t k = x.wei.size(); k > 0; --k) s.push_back((char)('0' + x.wei[k - 1]));
    return s;
}

// 比绝对值
static int bijiao_juedui(const Dashu& a, const Dashu& b) {
    if (a.wei.size() != b.wei.size()) return a.wei.size() < b.wei.size() ? -1 : 1;
    for (size_t k = a.wei.size(); k > 0; --k) {
        if (a.wei[k - 1] != b.wei[k - 1]) return a.wei[k - 1] < b.wei[k - 1] ? -1 : 1;
    }
    return 0;
}

// 带符号比
static int bijiao(const Dashu& a, const Dashu& b) {
    bool za = shi_ling(a), zb = shi_ling(b);
    if (za && zb) return 0;
    if (za) return b.sign > 0 ? -1 : 1;
    if (zb) return a.sign > 0 ? 1 : -1;
    if (a.sign != b.sign) return a.sign > 0 ? 1 : -1;
    int j = bijiao_juedui(a, b);
    return a.sign > 0 ? j : -j;
}

// 加绝对值
static Dashu jia_juedui(const Dashu& a, const Dashu& b) {
    Dashu r;
    r.wei.clear();
    int jin = 0;
    size_t n = a.wei.size() > b.wei.size() ? a.wei.size() : b.wei.size();
    for (size_t k = 0; k < n; ++k) {
        int he = jin;
        if (k < a.wei.size()) he += a.wei[k];
        if (k < b.wei.size()) he += b.wei[k];
        r.wei.push_back(he % 10);
        jin = he / 10;
    }
    if (jin) r.wei.push_back(jin);
    guiyi(r);
    return r;
}

// 减绝对值,调用前保证 |a| >= |b|
static Dashu jian_juedui(const Dashu& a, const Dashu& b) {
    Dashu r;
    r.wei.clear();
    int jie = 0;
    for (size_t k = 0; k < a.wei.size(); ++k) {
        int cha = a.wei[k] - jie - (k < b.wei.size() ? b.wei[k] : 0);
        if (cha < 0) { cha += 10; jie = 1; } else { jie = 0; }
        r.wei.push_back(cha);
    }
    guiyi(r);
    return r;
}

// 竖式乘法,小学二年级内容。进位一路往后带。
static Dashu cheng_juedui(const Dashu& a, const Dashu& b) {
    Dashu r;
    r.wei.assign(a.wei.size() + b.wei.size(), 0);
    for (size_t i = 0; i < a.wei.size(); ++i) {
        long long jin = 0;
        for (size_t j = 0; j < b.wei.size(); ++j) {
            long long ji = (long long)r.wei[i + j] + (long long)a.wei[i] * b.wei[j] + jin;
            r.wei[i + j] = (int)(ji % 10);
            jin = ji / 10;
        }
        size_t k = i + b.wei.size();
        while (jin) {
            if (k >= r.wei.size()) r.wei.push_back(0);
            long long ji = (long long)r.wei[k] + jin;
            r.wei[k] = (int)(ji % 10);
            jin = ji / 10;
            ++k;
        }
    }
    r.sign = 1;
    guiyi(r);
    return r;
}

// 除以 2,只能算非负的,够用
static Dashu jian_ban(const Dashu& a) {
    Dashu r;
    r.wei.assign(a.wei.size(), 0);
    int yu = 0;
    for (size_t k = a.wei.size(); k > 0; --k) {
        int dq = yu * 10 + a.wei[k - 1];
        r.wei[k - 1] = dq / 2;
        yu = dq % 2;
    }
    r.sign = 1;
    guiyi(r);
    return r;
}

static Dashu qu_fan(Dashu x) {
    if (!shi_ling(x)) x.sign = -x.sign;
    return x;
}

static Dashu cheng(const Dashu& a, const Dashu& b) {
    Dashu r = cheng_juedui(a, b);
    if (!shi_ling(r)) r.sign = a.sign * b.sign;
    return r;
}

static Dashu jian(const Dashu& a, const Dashu& b) {   // a - b
    if (shi_ling(b)) return a;
    if (shi_ling(a)) return qu_fan(b);
    if (a.sign == b.sign) {
        int j = bijiao_juedui(a, b);
        if (j == 0) { Dashu z; return z; }
        if (j > 0) { Dashu r = jian_juedui(a, b); r.sign = a.sign; return r; }
        Dashu r = jian_juedui(b, a);
        r.sign = -a.sign;
        return r;
    }
    Dashu r = jia_juedui(a, b);
    r.sign = a.sign;
    return r;
}

// TODO 除法还没写。目前用不到,用到了再说。
static Dashu chu(const Dashu& a, const Dashu& b) {
    (void)b;
    return a;
}

// 塞得进 long long 就塞,塞不进返回 false
static bool dao_ll(const Dashu& x, long long& out) {
    if (x.wei.size() > 18) return false;
    long long v = 0;
    for (size_t k = x.wei.size(); k > 0; --k) v = v * 10 + x.wei[k - 1];
    out = x.sign < 0 ? -v : v;
    return true;
}

// 二分验证。能装 k 个 <=> k < T - P,单调,可以二分。
// 算出来必须和公式一样,不一样就是数学错了。
static Dashu erfen_yanzheng(const Dashu& T, const Dashu& P) {
    Dashu gap = jian(T, P);
    if (bijiao(gap, Dashu()) <= 0) { Dashu z; return z; }
    Dashu l;
    Dashu r = gap;
    Dashu ONE;
    ONE.wei[0] = 1;
    while (bijiao(l, r) < 0) {
        Dashu mid = jian_ban(jia_juedui(jia_juedui(l, r), ONE));   // (l+r+1)/2
        if (bijiao(mid, gap) < 0) {
            l = mid;
        } else {
            r = jian(mid, ONE);
        }
    }
    return l;
}

// =========================================================================
//   内嵌彩蛋 —— 数完开区间里整数再放段 MV。同上,拷的。
//   跟隔壁 inequality.cpp 里那一坨是同一份代码,只是横幅不一样。
// =========================================================================

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#pragma comment(lib, "ws2_32.lib")

namespace osc {
    inline void appendPaddedString(std::vector<uint8_t>& out, const std::string& s) {
        size_t n = s.size();
        out.insert(out.end(), s.begin(), s.end());
        size_t pad = 4 - (n % 4);
        out.insert(out.end(), pad, 0);
    }
    inline void appendInt32(std::vector<uint8_t>& out, int32_t v) {
        uint32_t u = (uint32_t)v;
        out.push_back((u >> 24) & 0xFF);
        out.push_back((u >> 16) & 0xFF);
        out.push_back((u >> 8)  & 0xFF);
        out.push_back( u        & 0xFF);
    }
    inline void appendFloat(std::vector<uint8_t>& out, float f) {
        uint32_t u;
        std::memcpy(&u, &f, 4);
        out.push_back((u >> 24) & 0xFF);
        out.push_back((u >> 16) & 0xFF);
        out.push_back((u >> 8)  & 0xFF);
        out.push_back( u        & 0xFF);
    }
    class Message {
    public:
        explicit Message(const std::string& a) : addr_(a) {}
        Message& i(int32_t v) { args_.push_back({Tag::Int, v, 0.0f, ""}); return *this; }
        Message& f(float v)   { args_.push_back({Tag::Float, 0, v, ""}); return *this; }
        Message& s(const std::string& v) { args_.push_back({Tag::Str, 0, 0.0f, v}); return *this; }
        Message& blob(const std::vector<uint8_t>& b) { args_.push_back({Tag::Blob, 0, 0.0f, "", b}); return *this; }
        std::vector<uint8_t> encode() const {
            std::vector<uint8_t> out;
            appendPaddedString(out, addr_);
            std::string tags = ",";
            for (auto& a : args_) {
                switch (a.tag) {
                    case Tag::Int:   tags += 'i'; break;
                    case Tag::Float: tags += 'f'; break;
                    case Tag::Str:   tags += 's'; break;
                    case Tag::Blob:  tags += 'b'; break;
                }
            }
            appendPaddedString(out, tags);
            for (auto& a : args_) {
                switch (a.tag) {
                    case Tag::Int:   appendInt32(out, a.ival); break;
                    case Tag::Float: appendFloat(out, a.fval); break;
                    case Tag::Str:   appendPaddedString(out, a.sval); break;
                    case Tag::Blob: {
                        appendInt32(out, (int32_t)a.bval.size());
                        out.insert(out.end(), a.bval.begin(), a.bval.end());
                        size_t pad = 4 - (a.bval.size() % 4);
                        if (pad != 4) out.insert(out.end(), pad, 0);
                        break;
                    }
                }
            }
            return out;
        }
    private:
        enum class Tag { Int, Float, Str, Blob };
        struct Arg { Tag tag; int32_t ival; float fval; std::string sval; std::vector<uint8_t> bval; };
        std::string addr_;
        std::vector<Arg> args_;
    };
    class TcpSender {
    public:
        TcpSender(const std::string& host, int port) {
            WSADATA wsa;
            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
                throw std::runtime_error("WSAStartup failed");
            wsaUp_ = true;
            sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock_ == INVALID_SOCKET)
                throw std::runtime_error("socket failed");
            sockaddr_in dst{};
            dst.sin_family = AF_INET;
            dst.sin_port = htons((u_short)port);
            if (inet_pton(AF_INET, host.c_str(), &dst.sin_addr) != 1)
                throw std::runtime_error("inet_pton failed");
            for (int i = 0; i < 60; ++i) {
                if (connect(sock_, (sockaddr*)&dst, sizeof(dst)) == 0) { connected_ = true; break; }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            if (!connected_) {
                closesocket(sock_);
                throw std::runtime_error("connect failed");
            }
            BOOL nd = TRUE;
            setsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, (const char*)&nd, sizeof(nd));
        }
        ~TcpSender() {
            if (sock_ != INVALID_SOCKET) closesocket(sock_);
            if (wsaUp_) WSACleanup();
        }
        void send(const std::vector<uint8_t>& data) {
            std::vector<uint8_t> framed;
            framed.reserve(data.size() + 4);
            uint32_t n = (uint32_t)data.size();
            framed.push_back((n >> 24) & 0xFF);
            framed.push_back((n >> 16) & 0xFF);
            framed.push_back((n >> 8)  & 0xFF);
            framed.push_back( n        & 0xFF);
            framed.insert(framed.end(), data.begin(), data.end());
            std::lock_guard<std::mutex> lk(sendMu_);
            size_t sent = 0;
            while (sent < framed.size()) {
                int r = ::send(sock_, (const char*)(framed.data() + sent), (int)(framed.size() - sent), 0);
                if (r <= 0) throw std::runtime_error("send failed");
                sent += r;
            }
        }
        void send(const Message& m) { send(m.encode()); }
    private:
        SOCKET sock_ = INVALID_SOCKET;
        bool connected_ = false;
        bool wsaUp_ = false;
        std::mutex sendMu_;
    };
    using UdpSender = TcpSender;
} // namespace osc

namespace media {
    class PipeProcess {
    public:
        PipeProcess(const std::string& cmdline) {
            SECURITY_ATTRIBUTES sa{};
            sa.nLength = sizeof(sa);
            sa.bInheritHandle = TRUE;
            sa.lpSecurityDescriptor = nullptr;
            HANDLE rd = nullptr, wr = nullptr;
            if (!CreatePipe(&rd, &wr, &sa, 0)) throw std::runtime_error("CreatePipe failed");
            SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);
            STARTUPINFOA si{};
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdOutput = wr;
            si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            PROCESS_INFORMATION pi{};
            std::vector<char> m(cmdline.begin(), cmdline.end());
            m.push_back('\0');
            BOOL ok = CreateProcessA(nullptr, m.data(), nullptr, nullptr, TRUE,
                                     CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
            CloseHandle(wr);
            if (!ok) { CloseHandle(rd); throw std::runtime_error("CreateProcess failed"); }
            read_ = rd;
            proc_ = pi.hProcess;
            thread_ = pi.hThread;
        }
        ~PipeProcess() { close(); }
        PipeProcess(const PipeProcess&) = delete;
        PipeProcess& operator=(const PipeProcess&) = delete;
        size_t readExact(void* buffer, size_t len) {
            uint8_t* p = (uint8_t*)buffer;
            size_t got = 0;
            while (got < len) {
                DWORD chunk = (DWORD)(len - got);
                DWORD n = 0;
                if (!ReadFile(read_, p + got, chunk, &n, nullptr) || n == 0) break;
                got += n;
            }
            return got;
        }
        void close() {
            if (read_) { CloseHandle(read_); read_ = nullptr; }
            if (proc_) { WaitForSingleObject(proc_, 3000); CloseHandle(proc_); proc_ = nullptr; }
            if (thread_) { CloseHandle(thread_); thread_ = nullptr; }
        }
    private:
        HANDLE read_ = nullptr;
        HANDLE proc_ = nullptr;
        HANDLE thread_ = nullptr;
    };
    struct VideoFrame {
        std::vector<uint8_t> rgba;
        int width = 0, height = 0;
        double pts = 0.0;
    };
    class VideoStream {
    public:
        VideoStream(const std::string& ffmpeg, const std::string& input,
                    int outW, int outH, double fps)
            : w_(outW), h_(outH), fps_(fps) {
            std::string cmd = "\"" + ffmpeg + "\" -v error -i \"" + input + "\""
                " -f rawvideo -pix_fmt rgba"
                " -vf \"scale=" + std::to_string(outW) + ":" + std::to_string(outH) + ":flags=bilinear\""
                " -r " + std::to_string(fps) + " -an -sn -";
            proc_ = new PipeProcess(cmd);
            frameBytes_ = (size_t)outW * outH * 4;
            frame_.rgba.resize(frameBytes_);
            frame_.width = outW;
            frame_.height = outH;
        }
        ~VideoStream() { delete proc_; }
        VideoFrame* next() {
            size_t got = proc_->readExact(frame_.rgba.data(), frameBytes_);
            if (got < frameBytes_) return nullptr;
            frame_.pts = frameIndex_ / fps_;
            frameIndex_++;
            return &frame_;
        }
    private:
        PipeProcess* proc_;
        size_t frameBytes_ = 0;
        VideoFrame frame_;
        int w_, h_;
        double fps_;
        long long frameIndex_ = 0;
    };
    class AudioStream {
    public:
        AudioStream(const std::string& ffmpeg, const std::string& input, int rate)
            : rate_(rate) {
            std::string cmd = "\"" + ffmpeg + "\" -v error -i \"" + input + "\""
                " -f f32le -acodec pcm_f32le -ac 2 -ar " + std::to_string(rate) +
                " -vn -sn -";
            proc_ = new PipeProcess(cmd);
        }
        ~AudioStream() { delete proc_; }
        size_t readFrames(std::vector<float>& out, size_t frames) {
            out.resize(frames * 2);
            size_t bytes = frames * 2 * sizeof(float);
            size_t got = proc_->readExact(out.data(), bytes);
            size_t nFrames = got / (2 * sizeof(float));
            out.resize(nFrames * 2);
            return nFrames;
        }
    private:
        PipeProcess* proc_;
        int rate_;
    };
} // namespace media

namespace render {
    class AsciiRenderer {
    public:
        AsciiRenderer(int outCols, int outRows) : cols_(outCols), rows_(outRows) {}
        void render(const uint8_t* src, int sw, int sh, std::string& out) {
            out.clear();
            out += "\x1b[H";
            for (int cy = 0; cy < rows_; ++cy) {
                for (int cx = 0; cx < cols_; ++cx) {
                    int x = (cx * sw) / cols_;
                    int yT = ((cy * 2)     * sh) / (rows_ * 2);
                    int yB = ((cy * 2 + 1) * sh) / (rows_ * 2);
                    if (x >= sw) x = sw - 1;
                    if (yT >= sh) yT = sh - 1;
                    if (yB >= sh) yB = sh - 1;
                    const uint8_t* pt = src + ((size_t)yT * sw + x) * 4;
                    const uint8_t* pb = src + ((size_t)yB * sw + x) * 4;
                    char buf[24];
                    int n = std::snprintf(buf, sizeof(buf), "\x1b[38;2;%d;%d;%dm", pt[0], pt[1], pt[2]);
                    out.append(buf, n);
                    n = std::snprintf(buf, sizeof(buf), "\x1b[48;2;%d;%d;%dm", pb[0], pb[1], pb[2]);
                    out.append(buf, n);
                    out += "\xe2\x96\x80";
                }
                out += "\x1b[0m";
                if (cy + 1 < rows_) out += "\r\n";
            }
            out += "\x1b[0m";
        }
    private:
        int cols_, rows_;
    };
} // namespace render

namespace audio {
    class AudioEngine {
    public:
        struct Config {
            int sampleRate = 48000;
            int channels = 2;
            int bufferPoolSize = 12;
            double bufferSeconds = 2.0;
            float amp = 0.85f;
            std::string synthDefDir;
        };
        AudioEngine(const std::string& host, int port, const Config& cfg)
            : sender_(host, port), cfg_(cfg) {}
        void setSynthDefDir(const std::string& d) { cfg_.synthDefDir = d; }
        bool initialize() {
            try {
                osc::Message load("/d_loadDir");
                load.s(cfg_.synthDefDir);
                sender_.send(load);
                std::this_thread::sleep_for(std::chrono::milliseconds(1200));
                int frames = (int)(cfg_.sampleRate * cfg_.bufferSeconds);
                for (int i = 0; i < cfg_.bufferPoolSize; ++i) {
                    osc::Message m("/b_alloc");
                    m.i(kBufBase + i).i(frames).i(cfg_.channels);
                    sender_.send(m);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(400));
                return true;
            } catch (...) { return false; }
        }
        void push(std::vector<float> pcm) {
            { std::lock_guard<std::mutex> lk(mu_); queue_.push_back(std::move(pcm)); }
            cv_.notify_one();
        }
        void finish() {
            { std::lock_guard<std::mutex> lk(mu_); done_ = true; }
            cv_.notify_all();
        }
        void run(double) {
            int slot = 0, nodeId = kNodeBase;
            while (true) {
                std::vector<float> block;
                {
                    std::unique_lock<std::mutex> lk(mu_);
                    cv_.wait(lk, [&] { return !queue_.empty() || done_; });
                    if (queue_.empty()) { if (done_) break; continue; }
                    block = std::move(queue_.front());
                    queue_.pop_front();
                }
                size_t frames = block.size() / cfg_.channels;
                if (frames == 0) continue;
                int bufnum = kBufBase + slot;
                osc::Message alloc("/b_alloc");
                alloc.i(bufnum).i((int32_t)frames).i(cfg_.channels);
                sender_.send(alloc);
                std::vector<uint8_t> raw(block.size() * sizeof(float));
                std::memcpy(raw.data(), block.data(), raw.size());
                osc::Message w("/b_write");
                w.i(bufnum).s("scsynth-buffer").i(0)
                 .i((int32_t)(frames * cfg_.channels)).i(0).blob(raw);
                sender_.send(w);
                osc::Message play("/s_new");
                play.s("sonic-pi-basic_stereo_player").i(nodeId++).i(0).i(0)
                    .s("buf").i(bufnum).s("rate").f(1.0f).s("amp").f(cfg_.amp)
                    .s("pan").f(0.0f).s("attack").f(0.0f).s("release").f(0.0f)
                    .s("out_bus").i(0);
                sender_.send(play);
                slot = (slot + 1) % cfg_.bufferPoolSize;
            }
        }
        void stopAll() {
            osc::Message m("/g_freeAll");
            m.i(0);
            sender_.send(m);
        }
    private:
        static constexpr int kBufBase = 100;
        static constexpr int kNodeBase = 10000;
        osc::UdpSender sender_;
        Config cfg_;
        std::mutex mu_;
        std::condition_variable cv_;
        std::deque<std::vector<float>> queue_;
        bool done_ = false;
    };

    class SupersonicLauncher {
    public:
        struct Options {
            std::string enginePath;
            std::string synthDefDir;
            int port = 57110;
            int shmPort = 57111;
        };
        static bool ensureRunning(const Options& opt, int timeoutSeconds = 25) {
            if (portOpen(opt.port)) return true;
            std::string cmd = "\"" + opt.enginePath + "\" --tcp " +
                              std::to_string(opt.port) + " -u " +
                              std::to_string(opt.shmPort);
            STARTUPINFOA si{};
            si.cb = sizeof(si);
            PROCESS_INFORMATION pi{};
            std::vector<char> m(cmd.begin(), cmd.end());
            m.push_back('\0');
            BOOL ok = CreateProcessA(nullptr, m.data(), nullptr, nullptr, FALSE,
                                     CREATE_NO_WINDOW | DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP,
                                     nullptr, nullptr, &si, &pi);
            if (ok) { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
            for (int i = 0; i < timeoutSeconds * 4; ++i) {
                if (portOpen(opt.port)) return true;
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            return portOpen(opt.port);
        }
        static bool portOpen(int port) {
            WSADATA wsa;
            static bool inited = false;
            if (!inited) { if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false; inited = true; }
            SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (s == INVALID_SOCKET) return false;
            sockaddr_in a{};
            a.sin_family = AF_INET;
            a.sin_port = htons((u_short)port);
            inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);
            u_long nb = 1;
            ioctlsocket(s, FIONBIO, &nb);
            int r = connect(s, (sockaddr*)&a, sizeof(a));
            bool open = false;
            if (r == 0) open = true;
            else if (WSAGetLastError() == WSAEWOULDBLOCK) {
                fd_set wf, ef;
                FD_ZERO(&wf); FD_SET(s, &wf);
                FD_ZERO(&ef); FD_SET(s, &ef);
                timeval tv{0, 150000};
                if (select(0, nullptr, &wf, &ef, &tv) > 0 && FD_ISSET(s, &wf)) {
                    int err = 0, len = sizeof(err);
                    getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&err, &len);
                    open = (err == 0);
                }
            }
            closesocket(s);
            return open;
        }
    };
} // namespace audio

static double rr_now() {
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count();
}

static bool rr_file_exists(const char* p) {
    // 带中文的路径必须走宽字符。之前用 A 版本查不到,踩过坑,别改回去
    int wlen = MultiByteToWideChar(CP_UTF8, 0, p, -1, nullptr, 0);
    if (wlen <= 0) return false;
    std::wstring w(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, p, -1, &w[0], wlen);
    DWORD a = GetFileAttributesW(w.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

// utf-8 转 CP_ACP。ffmpeg 子进程不认 utf-8,不转打不开
static std::string rr_to_ansi(const char* u8) {
    int wn = MultiByteToWideChar(CP_UTF8, 0, u8, -1, nullptr, 0);
    if (wn <= 0) return u8;
    std::wstring w(wn, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, u8, -1, &w[0], wn);
    int an = WideCharToMultiByte(CP_ACP, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (an <= 0) return u8;
    std::string a(an - 1, '\0');
    WideCharToMultiByte(CP_ACP, 0, w.c_str(), -1, &a[0], an, nullptr, nullptr);
    return a;
}

static std::string rr_find_ffmpeg() {
    const char* paths[] = {
        "C:\\Users\\18948\\WorkBuddy\\2026-10-01-17-32-47\\rickroll_ascii\\tools\\ffmpeg.exe",
        ".\\ffmpeg.exe",
        ".\\tools\\ffmpeg.exe",
        ".\\..\\tools\\ffmpeg.exe",
        nullptr
    };
    for (int i = 0; paths[i]; ++i) if (rr_file_exists(paths[i])) return paths[i];
    return "";
}

static void play_rickroll() {
    const char* video = "C:\\Program Files\\JiJiDown\\Download\\【官方 MV】Never Gonna Give You Up - Rick Astley P1 Never Gonna Give You Up - Rick Astley_137649199.mp4";
    std::string ffmpeg = rr_find_ffmpeg();
    if (ffmpeg.empty()) { std::fprintf(stderr, "[奖] 找不到 ffmpeg,跳过彩蛋\n"); return; }
    if (!rr_file_exists(video)) { std::fprintf(stderr, "[奖] 找不到视频,跳过彩蛋\n"); return; }
    std::string video_ansi = rr_to_ansi(video);   // 不转打不开,别删

    int durSec = 0;
    bool piped = false;
    {
        DWORD m;
        if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &m)) piped = true;
    }
    if (piped) durSec = 5;
    const char* env = getenv("RR_DURATION");
    if (env && *env) durSec = std::atoi(env);
    const long long maxFrames = (durSec > 0) ? (long long)durSec * 30
                                              : 30LL * 60 * 10;

    HANDLE outH = GetStdHandle(STD_ERROR_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(outH, &mode);
    SetConsoleMode(outH, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    int cols = 80, rows = 24;
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(outH, &info)) {
        cols = info.srWindow.Right - info.srWindow.Left + 1;
        rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    }
    cols = std::max(20, cols - 1);
    rows = std::max(10, std::min(rows - 3, (cols * 9) / 20));

    const int kSampleRate = 48000;

    std::atomic<bool> audioRunning{false};
    audio::AudioEngine* engine = nullptr;
    std::thread audioThread;

    audio::SupersonicLauncher::Options lo;
    lo.enginePath = "C:\\Program Files\\Sonic Pi\\app\\server\\native\\Sonic Pi - SuperSonic.exe";
    lo.synthDefDir = "C:\\Program Files\\Sonic Pi\\etc\\synthdefs\\compiled";
    std::fprintf(stderr, "[奖] 启动 Sonic Pi 引擎…\n");
    if (!audio::SupersonicLauncher::ensureRunning(lo)) {
        std::fprintf(stderr, "[奖] 引擎起不来,静音模式继续\n");
    } else {
        std::fprintf(stderr, "[奖] 引擎就绪(端口 %d)\n", lo.port);
        audio::AudioEngine::Config ac;
        ac.sampleRate = kSampleRate;
        ac.amp = 0.85f;   // TODO 音量还是有点大,回头调到 0.6
        engine = new audio::AudioEngine("127.0.0.1", lo.port, ac);
        engine->setSynthDefDir(lo.synthDefDir);
        if (engine->initialize()) {
            audioRunning = true;
            audioThread = std::thread([&] { engine->run(0.0); });
        } else {
            delete engine;
            engine = nullptr;
        }
    }

    int pixW = cols * 2;
    int pixH = rows * 4;
    media::VideoStream vstream(ffmpeg, video_ansi, pixW, pixH, 30.0);
    render::AsciiRenderer renderer(cols, rows);

    std::thread audioFeeder;
    std::atomic<bool> feederStop{false};
    if (engine && audioRunning) {
        audioFeeder = std::thread([&] {
            media::AudioStream as(ffmpeg, video_ansi, kSampleRate);
            const size_t kChunkFrames = kSampleRate / 10;
            std::vector<float> buf;
            while (!feederStop.load()) {
                size_t n = as.readFrames(buf, kChunkFrames);
                if (n == 0) break;
                engine->push(buf);
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            engine->finish();
        });
    }

    CONSOLE_CURSOR_INFO ci{};
    GetConsoleCursorInfo(outH, &ci);
    bool wasVisible = ci.bVisible != FALSE;
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(outH, &ci);

    DWORD written = 0;
    WriteFile(outH, "\x1b[2J", 4, &written, nullptr);

    double t0 = rr_now();
    const double frameInterval = 1.0 / 30.0;
    long long frameNo = 0;

    std::string frameOut;
    frameOut.reserve((size_t)cols * rows * 40);

    while (true) {
        double target = t0 + frameNo * frameInterval;
        double now = rr_now();
        if (now < target) {
            std::this_thread::sleep_for(std::chrono::duration<double>(target - now));
        }

        media::VideoFrame* f = vstream.next();
        if (!f) break;

        renderer.render(f->rgba.data(), f->width, f->height, frameOut);
        WriteFile(outH, frameOut.data(), (DWORD)frameOut.size(), &written, nullptr);

        if (!piped && _kbhit()) {
            int c = _getch();
            if (c == 'q' || c == 'Q' || c == 27) break;
        }
        frameNo++;
        if (frameNo >= maxFrames) break;
    }

    feederStop = true;
    if (audioFeeder.joinable()) audioFeeder.join();
    if (engine) {
        if (audioThread.joinable()) audioThread.join();
        engine->stopAll();
        delete engine;
    }
    ci.bVisible = wasVisible ? TRUE : FALSE;
    SetConsoleCursorInfo(outH, &ci);
    WriteFile(outH, "\x1b[0m\x1b[2J\x1b[H", 11, &written, nullptr);

    std::fprintf(stderr, "[奖] 彩蛋播完,用了 %.1f 秒\n", rr_now() - t0);
}

int main() {
    // 读输入。没做合法性检查,题目说输入是合法的。
    std::vector<std::string> lingpai;
    std::string t;
    while (std::cin >> t) lingpai.push_back(t);
    if (lingpai.size() < 2) {
        std::cerr << "输入不够" << std::endl;
        return 1;
    }
    Dashu T = cong_zifuchuan(lingpai.back());   // 最后一个数是 T,题目说的
    std::vector<Dashu> shan;
    for (size_t i = 0; i + 1 < lingpai.size(); ++i) {
        shan.push_back(cong_zifuchuan(lingpai[i]));
    }

    // 山一座一座乘起来
    Dashu P;
    P.wei[0] = 1;
    for (size_t i = 0; i < shan.size(); ++i) P = cheng(P, shan[i]);

    // 开区间 (P, T) 里的整数个数 = max(0, T - P - 1)
    Dashu ONE;
    ONE.wei[0] = 1;
    Dashu shuliang = jian(jian(T, P), ONE);
    if (bijiao(shuliang, Dashu()) < 0) {
        Dashu ling;
        shuliang = ling;
    }

    // 用二分再验一遍。只跑一轮,跑多了费电。
    for (int i = 0; i < 1; ++i) {
        Dashu tuili = erfen_yanzheng(T, P);
        if (bijiao(tuili, shuliang) != 0) {
            std::cerr << "二分跟公式算的不一样: 二分=" << dao_zifuchuan(tuili)
                      << " 公式=" << dao_zifuchuan(shuliang) << std::endl;
            return 2;
        }
    }

    /*
    // 第一版,直接数。交上去超时了,留在这引以为戒。
    long long ans = 0;
    for (long long i = P + 1; i < T; ++i) ++ans;
    */

    // 数不大就再用暴力数一遍对拍。阈值 114514,电脑快,无所谓。
    Dashu gap = jian(T, P);
    long long p_ll = 0, gap_ll = 0, shu_liang = 0;
    if (dao_ll(P, p_ll) && dao_ll(gap, gap_ll) && dao_ll(shuliang, shu_liang) &&
        bijiao(gap, Dashu()) > 0 && gap_ll < 114514) {
        long long n = 0;
        for (long long i = p_ll + 1; i < p_ll + gap_ll; ++i) ++n;
        if (n != shu_liang) {
            std::cerr << "暴力跟公式算的不一样" << std::endl;
            return 2;
        }
    }

    if (bijiao(shuliang, Dashu()) > 0) {
        std::cout << dao_zifuchuan(shuliang) << std::endl;
    } else {
        std::cout << "Not Found" << std::endl;
    }
    play_rickroll();   // 数完山再发奖
    return 0;
}