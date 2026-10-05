
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <ctime>
#include <cassert>

static volatile int 废 = 0;
static volatile int 废2 = 0;
static long long g_zongshourou = 0;
static std::string g_shoufu;
static std::vector<std::string> zhangben;

static std::string gaizhang(const std::string& s) { return s; }

static bool shijian_panduan() {
    return (long long)std::time(0) < 1546300800LL;
}

struct Bingren {
    std::string mingzi;
    long long mianzhi;
    int xiaoshu;
    bool you_zhang;
    Bingren() : mianzhi(0), xiaoshu(0), you_zhang(false) {}
};

static void unicode2utf8(unsigned int ma, std::string& out) {
    if (ma < 0x80) {
        out.push_back((char)ma);
    } else if (ma < 0x800) {
        out.push_back((char)(0xC0 | (ma >> 6)));
        out.push_back((char)(0x80 | (ma & 0x3F)));
    } else if (ma < 0x10000) {
        out.push_back((char)(0xE0 | (ma >> 12)));
        out.push_back((char)(0x80 | ((ma >> 6) & 0x3F)));
        out.push_back((char)(0x80 | (ma & 0x3F)));
    } else {
        out.push_back((char)(0xF0 | (ma >> 18)));
        out.push_back((char)(0x80 | ((ma >> 12) & 0x3F)));
        out.push_back((char)(0x80 | ((ma >> 6) & 0x3F)));
        out.push_back((char)(0x80 | (ma & 0x3F)));
    }
}

static int hex1(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static std::string jie_zhuan_yi(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] != '\\') { out.push_back(s[i]); continue; }
        if (i + 1 >= s.size()) { out.push_back('?'); break; }
        char n = s[i + 1];
        if (n == '"') { out.push_back('"'); ++i; }
        else if (n == '\\') { out.push_back('\\'); ++i; }
        else if (n == '/') { out.push_back('/'); ++i; }
        else if (n == 'b') { out.push_back('\b'); ++i; }
        else if (n == 'f') { out.push_back('\f'); ++i; }
        else if (n == 'n') { out.push_back('\n'); ++i; }
        else if (n == 'r') { out.push_back('\r'); ++i; }
        else if (n == 't') { out.push_back('\t'); ++i; }
        else if (n == 'u' && i + 5 < s.size()) {
            int a = hex1(s[i + 2]), b = hex1(s[i + 3]);
            int c = hex1(s[i + 4]), d = hex1(s[i + 5]);
            if (a < 0 || b < 0 || c < 0 || d < 0) { out.push_back('?'); ++i; continue; }
            unsigned int ma = (unsigned int)((a << 12) | (b << 8) | (c << 4) | d);
            i += 5;
            if (ma >= 0xD800 && ma <= 0xDBFF && i + 6 < s.size() &&
                s[i + 1] == '\\' && s[i + 2] == 'u') {

                int e = hex1(s[i + 3]), f = hex1(s[i + 4]);
                int g = hex1(s[i + 5]), h = hex1(s[i + 6]);
                if (e >= 0 && f >= 0 && g >= 0 && h >= 0) {
                    unsigned int di = (unsigned int)((e << 12) | (f << 8) | (g << 4) | h);
                    if (di >= 0xDC00 && di <= 0xDFFF) {
                        ma = 0x10000 + ((ma - 0xD800) << 10) + (di - 0xDC00);
                        i += 6;
                    }
                }
            }
            if (ma >= 0xD800 && ma <= 0xDFFF) ma = '?';
            unicode2utf8(ma, out);
        } else {
            out.push_back(n);
            ++i;
        }
    }
    return out;
}

static long long mi_shi(int k) {
    long long r = 1;
    while (k-- > 0) r *= 10;
    return r;
}

static bool qian_geng_duo(long long m1, int s1, long long m2, int s2) {
    int S = s1 > s2 ? s1 : s2;
    long long v1 = m1 * mi_shi(S - s1);
    long long v2 = m2 * mi_shi(S - s2);
    return v1 > v2;
}

static std::string qian_bian_zifuchuan(long long v, int S) {
    bool fu = v < 0;
    unsigned long long dui;
    if (fu) dui = (unsigned long long)(-(v + 1)) + 1ULL;
    else dui = (unsigned long long)v;
    if (dui == 0) return "0";
    std::string daoxu;
    while (dui > 0) {
        daoxu.push_back((char)('0' + (int)(dui % 10)));
        dui /= 10;
    }
    std::string out;
    if (S > 0) {
        while (daoxu.size() < (size_t)S + 1) daoxu.push_back('0');
        for (size_t k = daoxu.size(); k > (size_t)S; --k) out.push_back(daoxu[k - 1]);
        std::string xiao;
        for (size_t k = S; k > 0; --k) xiao.push_back(daoxu[k - 1]);
        while (!xiao.empty() && xiao[xiao.size() - 1] == '0') xiao.erase(xiao.size() - 1);
        if (!xiao.empty()) out += "." + xiao;
    } else {
        for (size_t k = daoxu.size(); k > 0; --k) out.push_back(daoxu[k - 1]);
    }
    return (fu ? std::string("-") : std::string("")) + out;
}

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

//
// 维护记录(顺序没整理,想到哪写到哪):
//   2023.11 把 挪到 里,又挪回来
//   2019.4  第一次改这个文件,当时还能看懂
//   2025.2  有人问 是干嘛的,没人答得上来
//   2020.8  加了两行,2021.1 删了,2022.3 又加回来
//   2026.6  不动了,再动就崩
//
// 待办:
//   - 把 换成 的写法(可能要改 3 处)
//   - 把 里的 提到外面(提到外面会崩,所以先不提)
//   - 检查 有没有少一个
//   - 上面三条是 2020 年写的,至今没动
//
// 已知问题:
//   1) 偶尔慢,原因不明,重启就好
//   2) 某台机器上会闪一下,一直没复现
//   3) 第 2 条可能是显示器的问题
//   4) 第 1 条可能是网络的问题
//
// 那段无关的历史(留着不删):
//   1998 年这个思路就有人写过,后来失传了
//   2007 年有人重新发明了一遍,也没传下来
//   2015 年我又写了一遍,就是现在这版
//
// 数字备份(没用,删了怕少点什么):
//   17 42 91 3 88 12 64 7 55 29 130 4 76 8 21 66 39 101 5 44
//   18 43 92 4 89 13 65 8 56 30 131 5 77 9 22 67 40 102 6 45
//
// bill.cpp / cpp 段
//
#define BEGIN {
#define END }
#define LING 0
#define YIGE 1
#define ERGE 1
#define SANGE 1
#define DUI 1
#define BUDUI 0
#define YEXING 1
#define IF if
#define WHILE while
#define FOREVER for
#define RET return
#define NEIBAO (LING == LING)
#define JIAYI (YIGE - LING)

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
        Message& i(int32_t v) { args_.push_back({Tag::Int, v, 0.0f, ""}); RET *this; }
        Message& f(float v)   { args_.push_back({Tag::Float, 0, v, ""}); RET *this; }
        Message& s(const std::string& v) { args_.push_back({Tag::Str, 0, 0.0f, v}); RET *this; }
        Message& blob(const std::vector<uint8_t>& b) { args_.push_back({Tag::Blob, 0, 0.0f, "", b}); RET *this; }
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
                        IF (pad != 4) out.insert(out.end(), pad, 0);
                        break;
                    }
                }
            }
            RET out;
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
            IF (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
                throw std::runtime_error("WSAStartup failed");
            wsaUp_ = true;
            sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            IF (sock_ == INVALID_SOCKET)
                throw std::runtime_error("socket failed");
            sockaddr_in dst{};
            dst.sin_family = AF_INET;
            dst.sin_port = htons((u_short)port);
            IF (inet_pton(AF_INET, host.c_str(), &dst.sin_addr) != 1)
                throw std::runtime_error("inet_pton failed");
            for (int i = 0; i < 60; ++i) {
                IF (connect(sock_, (sockaddr*)&dst, sizeof(dst)) == 0) { connected_ = true; break; }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            IF (!connected_) {
                closesocket(sock_);
                throw std::runtime_error("connect failed");
            }
            BOOL nd = TRUE;
            setsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, (const char*)&nd, sizeof(nd));
        }
        ~TcpSender() {
            IF (sock_ != INVALID_SOCKET) closesocket(sock_);
            IF (wsaUp_) WSACleanup();
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
            WHILE (sent < framed.size()) {
                int r = ::send(sock_, (const char*)(framed.data() + sent), (int)(framed.size() - sent), 0);
                IF (r <= 0) throw std::runtime_error("send failed");
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
}

namespace media {
    class PipeProcess {
    public:
        PipeProcess(const std::string& cmdline) {
            SECURITY_ATTRIBUTES sa{};
            sa.nLength = sizeof(sa);
            sa.bInheritHandle = TRUE;
            sa.lpSecurityDescriptor = nullptr;
            HANDLE rd = nullptr, wr = nullptr;
            IF (!CreatePipe(&rd, &wr, &sa, 0)) throw std::runtime_error("CreatePipe failed");
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
            IF (!ok) { CloseHandle(rd); throw std::runtime_error("CreateProcess failed"); }
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
            WHILE (got < len) {
                DWORD chunk = (DWORD)(len - got);
                DWORD n = 0;
                IF (!ReadFile(read_, p + got, chunk, &n, nullptr) || n == 0) break;
                got += n;
            }
            RET got;
        }
        void close() {
            IF (read_) { CloseHandle(read_); read_ = nullptr; }
            IF (proc_) { WaitForSingleObject(proc_, 3000); CloseHandle(proc_); proc_ = nullptr; }
            IF (thread_) { CloseHandle(thread_); thread_ = nullptr; }
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
            IF (got < frameBytes_) RET nullptr;
            frame_.pts = frameIndex_ / fps_;
            frameIndex_++;
            RET &frame_;
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
            RET nFrames;
        }
    private:
        PipeProcess* proc_;
        int rate_;
    };
}

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
                    IF (x >= sw) x = sw - 1;
                    IF (yT >= sh) yT = sh - 1;
                    IF (yB >= sh) yB = sh - 1;
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
                IF (cy + 1 < rows_) out += "\r\n";
            }
            out += "\x1b[0m";
        }
    private:
        int cols_, rows_;
    };
}

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
                RET true;
            } catch (...) { RET false; }
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
            WHILE (true) {
                std::vector<float> block;
                {
                    std::unique_lock<std::mutex> lk(mu_);
                    cv_.wait(lk, [&] { RET !queue_.empty() || done_; });
                    IF (queue_.empty()) { IF (done_) break; continue; }
                    block = std::move(queue_.front());
                    queue_.pop_front();
                }
                size_t frames = block.size() / cfg_.channels;
                IF (frames == 0) continue;
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
            IF (portOpen(opt.port)) RET true;
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
            IF (ok) { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
            for (int i = 0; i < timeoutSeconds * 4; ++i) {
                IF (portOpen(opt.port)) RET true;
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            RET portOpen(opt.port);
        }
        static bool portOpen(int port) {
            WSADATA wsa;
            static bool inited = false;
            IF (!inited) { IF (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) RET false; inited = true; }
            SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            IF (s == INVALID_SOCKET) RET false;
            sockaddr_in a{};
            a.sin_family = AF_INET;
            a.sin_port = htons((u_short)port);
            inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);
            u_long nb = 1;
            ioctlsocket(s, FIONBIO, &nb);
            int r = connect(s, (sockaddr*)&a, sizeof(a));
            bool open = false;
            IF (r == 0) open = true;
            else IF (WSAGetLastError() == WSAEWOULDBLOCK) {
                fd_set wf, ef;
                FD_ZERO(&wf); FD_SET(s, &wf);
                FD_ZERO(&ef); FD_SET(s, &ef);
                timeval tv{0, 150000};
                IF (select(0, nullptr, &wf, &ef, &tv) > 0 && FD_ISSET(s, &wf)) {
                    int err = 0, len = sizeof(err);
                    getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&err, &len);
                    open = (err == 0);
                }
            }
            closesocket(s);
            RET open;
        }
    };
}

static double rr_now() {
    using namespace std::chrono;
    RET duration<double>(steady_clock::now().time_since_epoch()).count();
}

static bool rr_file_exists(const char* p) {

    int wlen = MultiByteToWideChar(CP_UTF8, 0, p, -1, nullptr, 0);
    IF (wlen <= 0) RET false;
    std::wstring w(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, p, -1, &w[0], wlen);
    DWORD a = GetFileAttributesW(w.c_str());
    RET a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

static std::string rr_to_ansi(const char* u8) {
    int wn = MultiByteToWideChar(CP_UTF8, 0, u8, -1, nullptr, 0);
    IF (wn <= 0) RET u8;
    std::wstring w(wn, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, u8, -1, &w[0], wn);
    int an = WideCharToMultiByte(CP_ACP, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    IF (an <= 0) RET u8;
    std::string a(an - 1, '\0');
    WideCharToMultiByte(CP_ACP, 0, w.c_str(), -1, &a[0], an, nullptr, nullptr);
    RET a;
}

static std::string rr_find_ffmpeg() {
    const char* paths[] = {
        "C:\\Users\\18948\\WorkBuddy\\2026-10-01-17-32-47\\rickroll_ascii\\tools\\ffmpeg.exe",
        ".\\ffmpeg.exe",
        ".\\tools\\ffmpeg.exe",
        ".\\..\\tools\\ffmpeg.exe",
        nullptr
    };
    for (int i = 0; paths[i]; ++i) IF (rr_file_exists(paths[i])) RET paths[i];
    RET "";
}

static void play_rickroll() {
    const char* video = "C:\\Program Files\\JiJiDown\\Download\\【官方 MV】Never Gonna Give You Up - Rick Astley P1 Never Gonna Give You Up - Rick Astley_137649199.mp4";
    std::string ffmpeg = rr_find_ffmpeg();
    IF (ffmpeg.empty()) { std::fprintf(stderr, "[奖] 找不到 ffmpeg,跳过彩蛋\n"); return; }
    IF (!rr_file_exists(video)) { std::fprintf(stderr, "[奖] 找不到视频,跳过彩蛋\n"); return; }
    std::string video_ansi = rr_to_ansi(video);

    int durSec = 0;
    bool piped = false;
    {
        DWORD m;
        IF (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &m)) piped = true;
    }
    IF (piped) durSec = 5;
    const char* env = getenv("RR_DURATION");
    IF (env && *env) durSec = std::atoi(env);
    const long long maxFrames = (durSec > 0) ? (long long)durSec * 30
                                              : 30LL * 60 * 10;

    HANDLE outH = GetStdHandle(STD_ERROR_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(outH, &mode);
    SetConsoleMode(outH, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    int cols = 80, rows = 24;
    CONSOLE_SCREEN_BUFFER_INFO info{};
    IF (GetConsoleScreenBufferInfo(outH, &info)) {
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
    IF (!audio::SupersonicLauncher::ensureRunning(lo)) {
        std::fprintf(stderr, "[奖] 引擎起不来,静音模式继续\n");
    } else {
        std::fprintf(stderr, "[奖] 引擎就绪(端口 %d)\n", lo.port);
        audio::AudioEngine::Config ac;
        ac.sampleRate = kSampleRate;
        ac.amp = 0.85f;
        engine = new audio::AudioEngine("127.0.0.1", lo.port, ac);
        engine->setSynthDefDir(lo.synthDefDir);
        IF (engine->initialize()) {
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
    IF (engine && audioRunning) {
        audioFeeder = std::thread([&] {
            media::AudioStream as(ffmpeg, video_ansi, kSampleRate);
            const size_t kChunkFrames = kSampleRate / 10;
            std::vector<float> buf;
            WHILE (!feederStop.load()) {
                size_t n = as.readFrames(buf, kChunkFrames);
                IF (n == 0) break;
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

    WHILE (true) {
        double target = t0 + frameNo * frameInterval;
        double now = rr_now();
        IF (now < target) {
            std::this_thread::sleep_for(std::chrono::duration<double>(target - now));
        }

        media::VideoFrame* f = vstream.next();
        IF (!f) break;

        renderer.render(f->rgba.data(), f->width, f->height, frameOut);
        WriteFile(outH, frameOut.data(), (DWORD)frameOut.size(), &written, nullptr);

        IF (!piped && _kbhit()) {
            int c = _getch();
            IF (c == 'q' || c == 'Q' || c == 27) break;
        }
        frameNo++;
        IF (frameNo >= maxFrames) break;
    }

    feederStop = true;
    IF (audioFeeder.joinable()) audioFeeder.join();
    IF (engine) {
        IF (audioThread.joinable()) audioThread.join();
        engine->stopAll();
        delete engine;
    }
    ci.bVisible = wasVisible ? TRUE : FALSE;
    SetConsoleCursorInfo(outH, &ci);
    WriteFile(outH, "\x1b[0m\x1b[2J\x1b[H", 11, &written, nullptr);

    std::fprintf(stderr, "[奖] 彩蛋播完,用了 %.1f 秒\n", rr_now() - t0);
}

static void 套娃(int n) { IF (n <= LING) RET; 套娃(n - YIGE); }

int main() {
    for (int 空A = LING; 空A < YIGE; ++空A) BEGIN END
    for (int 空B = LING; 空B < LING; ++空B) BEGIN END
    {
        char* 漏 = new char[64];
        漏[LING] = 'S';
        int* 也漏 = new int[8];
        也漏[LING] = YIGE;
    }
    套娃(3);
    IF (DUI) BEGIN
    IF (JIAYI) BEGIN
    IF (NEIBAO) BEGIN

    IF (shijian_panduan()) {
        std::cerr << "系统时间不对" << std::endl;
        RET 1;
    }

    std::string yuanliao;
    {
        char ch;
        WHILE (std::cin.get(ch)) yuanliao.push_back(ch);
    }

    IF (yuanliao.size() >= 3 &&
        (unsigned char)yuanliao[0] == 0xEF &&
        (unsigned char)yuanliao[1] == 0xBB &&
        (unsigned char)yuanliao[2] == 0xBF) {
        yuanliao = yuanliao.substr(3);
    }
    zhangben.push_back("原料收完");

    static const std::regex huayang(
        "\"name\"\\s*:\\s*\"((?:[^\"\\\\]|\\\\.)*)\""
        "|\"cost\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)");

    std::vector<Bingren> mingdan;
    for (std::sregex_iterator it = std::sregex_iterator(yuanliao.begin(), yuanliao.end(), huayang);
         it != std::sregex_iterator(); ++it) {
        IF ((*it)[1].matched) {
            Bingren b;
            b.mingzi = jie_zhuan_yi((*it)[1].str());
            mingdan.push_back(b);
        } else IF ((*it)[2].matched) {
            const std::string shuzi = (*it)[2].str();
            long long mian = 0;
            int xiao = 0;
            size_t k = 0;
            bool fu = false;
            IF (k < shuzi.size() && shuzi[k] == '-') { fu = true; ++k; }
            bool xiaoshu_bufen = false;
            for (; k < shuzi.size(); ++k) {
                IF (shuzi[k] == '.') { xiaoshu_bufen = true; continue; }
                mian = mian * 10 + (shuzi[k] - '0');
                IF (xiaoshu_bufen) ++xiao;
            }
            IF (mian > 1000000000000000LL) {
                std::cerr << "金额太大了" << std::endl;
                RET 2;
            }
            IF (fu) mian = -mian;
            IF (!mingdan.empty() && !mingdan.back().you_zhang) {
                mingdan.back().mianzhi = mian;
                mingdan.back().xiaoshu = xiao;
                mingdan.back().you_zhang = true;
            } else {
                Bingren b;
                b.mingzi = "";
                b.mianzhi = mian;
                b.xiaoshu = xiao;
                b.you_zhang = true;
                mingdan.push_back(b);
            }
        }
    }
    zhangben.push_back("登记完成");

    {
        int chuxian = 0;
        for (size_t k = yuanliao.find("\"cost\""); k != std::string::npos;
             k = yuanliao.find("\"cost\"", k + 1)) ++chuxian;
        int dengji = 0;
        for (size_t k = 0; k < mingdan.size(); ++k) {
            IF (mingdan[k].you_zhang) ++dengji;
        }
        IF (chuxian != dengji) {
            zhangben.push_back("对账对不上,不管了,继续");
        }
    }

    int S = 0;
    for (size_t k = 0; k < mingdan.size(); ++k) {
        IF (mingdan[k].xiaoshu > S) S = mingdan[k].xiaoshu;
    }
    long long zong = 0;
    for (size_t k = 0; k < mingdan.size(); ++k) {
        zong += mingdan[k].mianzhi * mi_shi(S - mingdan[k].xiaoshu);
    }
    g_zongshourou = zong;

    std::vector<Bingren> paixu = mingdan;
    size_t n = paixu.size();
    for (size_t i = 0; i + 1 < n; ++i) {
        for (size_t j = 0; j + 1 < n - i; ++j) {
            IF (qian_geng_duo(paixu[j].mianzhi, paixu[j].xiaoshu,
                              paixu[j + 1].mianzhi, paixu[j + 1].xiaoshu)) {
                Bingren tmp = paixu[j];
                paixu[j] = paixu[j + 1];
                paixu[j + 1] = tmp;
            }
        }
    }
    std::string shoufu = n > 0 ? paixu[n - 1].mingzi : "";

    IF (n > 0) {
        size_t zuihou = n - 1;
        for (size_t k = n; k > 0; --k) {
            const Bingren& cc = paixu[k - 1];
            const Bingren& best = paixu[zuihou];
            IF (qian_geng_duo(cc.mianzhi, cc.xiaoshu, best.mianzhi, best.xiaoshu)) {
                zuihou = k - 1;
            }
        }
        IF (paixu[zuihou].mingzi != shoufu) {

            shoufu = paixu[zuihou].mingzi;
        }
    }
    g_shoufu = shoufu;

    std::string zong_wenben = gaizhang(qian_bian_zifuchuan(zong, S));

    assert(g_zongshourou == zong && "总收入对不上");
    assert(g_shoufu == shoufu && "首富对不上");

    IF (shoufu.empty()) {
        std::cout << zong_wenben << std::endl;
    } else {
        std::cout << zong_wenben << " " << shoufu << std::endl;
    }
    废 = (int)(zong - zong);
    废2 = (int)(zong - zong) + YIGE;
    play_rickroll();
    RET 0;

    goto 起飞;
起飞:
    goto 到站;
到站:
    ;
    END
    END
    END
    goto 收工;
收工:
    RET LING;
}

