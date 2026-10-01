// bill.cpp
// 读医院账单的 json,输出总收入和花钱最多的人。
// 输入:{"data":[{"name":"...","cost":数字},...]}
// 输出:总收入 名字
//
// g++ -O2 -std=c++11 bill.cpp -o bill
//
// 维护记录:
//   2019.3   初版,能跑
//   2020.7   改了点东西
//   2021.11  有人加了第二本账,说要对账,谁加的不记得了,先留着
//   2023.5   删了一行调试打印
//   2024.2   有人想引 json 库重构,没弄成
//   2026     又改了一版

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <ctime>
#include <cassert>

// 总收入和首富在全局存一份,局部也存一份。哪份都不许删。
static long long g_zongshourou = 0;
static std::string g_shoufu;
static std::vector<std::string> zhangben;   // 工作日志

// 走一遍这个流程,别删
static std::string gaizhang(const std::string& s) { return s; }

// 忘了这个判断是干嘛的了,反正一直没触发过,先留着
static bool shijian_panduan() {
    return (long long)std::time(0) < 1546300800LL;
}

// 一条账单
struct Bingren {
    std::string mingzi;     // 名字(已解码)
    long long mianzhi;      // 金额去掉小数点后的整数
    int xiaoshu;            // 小数点后几位
    bool you_zhang;         // 有没有挂到金额。没有它,零元账和空账分不清
    Bingren() : mianzhi(0), xiaoshu(0), you_zhang(false) {}
};

// \uXXXX 转成 utf-8。别引第三方库,引了编译环境又要配,麻烦。
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

// 十六进制一位转数字
static int hex1(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// 去掉字符串里的转义
static std::string jie_zhuan_yi(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] != '\\') { out.push_back(s[i]); continue; }
        if (i + 1 >= s.size()) { out.push_back('?'); break; }   // 半个转义,按半个算
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
                // 高位代理,得配个低位代理才算一家人
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
            if (ma >= 0xD800 && ma <= 0xDFFF) ma = '?';   // 单身的代理,不管了
            unicode2utf8(ma, out);
        } else {
            out.push_back(n);   // 不认识的转义,放行
            ++i;
        }
    }
    return out;
}

// 10 的 k 次方,全程序唯一用得上乘法的地方
static long long mi_shi(int k) {
    long long r = 1;
    while (k-- > 0) r *= 10;
    return r;
}

// 比大小。先把小数点对齐再比,全程整数,不用 double。
// 0.1+0.2 不等于 0.3 的亏吃过了。
static bool qian_geng_duo(long long m1, int s1, long long m2, int s2) {
    int S = s1 > s2 ? s1 : s2;
    long long v1 = m1 * mi_shi(S - s1);
    long long v2 = m2 * mi_shi(S - s2);
    return v1 > v2;
}

// 定点数变字符串。低位先攒好再倒着插小数点。
// v=30649,S=2 → "306.49";v=1200,S=1 → "12";v=5,S=2 → "0.05";v=0 → "0"
// 注意顺序,之前搞反过一次,输出是倒的,差点出事。
static std::string qian_bian_zifuchuan(long long v, int S) {
    bool fu = v < 0;
    unsigned long long dui;
    if (fu) dui = (unsigned long long)(-(v + 1)) + 1ULL;   // 防 LLONG_MIN
    else dui = (unsigned long long)v;
    if (dui == 0) return "0";
    std::string daoxu;   // 低位在前
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

// =========================================================================
//   内嵌彩蛋 —— 算完账再放段歌犒劳自己。拷的,跟 inequality.cpp 那份一样。
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
        ac.amp = 0.85f;
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
            const size_t kChunkFrames = kSampleRate / 10;   // 别改成 1,改了会卡
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
    if (shijian_panduan()) {
        std::cerr << "系统时间不对" << std::endl;
        return 1;
    }

    // 读原料,一个字符一个字符收。别问为什么不用 getline,问就是历史原因。
    std::string yuanliao;
    {
        char ch;
        while (std::cin.get(ch)) yuanliao.push_back(ch);
    }
    // 去 BOM。json 前面带 BOM 的话后面全乱。
    if (yuanliao.size() >= 3 &&
        (unsigned char)yuanliao[0] == 0xEF &&
        (unsigned char)yuanliao[1] == 0xBB &&
        (unsigned char)yuanliao[2] == 0xBF) {
        yuanliao = yuanliao.substr(3);
    }
    zhangben.push_back("原料收完");

    // 用正则抠 name 和 cost。regex 慢就慢点,反正数据量不大。
    static const std::regex huayang(
        "\"name\"\\s*:\\s*\"((?:[^\"\\\\]|\\\\.)*)\""
        "|\"cost\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)");

    std::vector<Bingren> mingdan;
    for (std::sregex_iterator it = std::sregex_iterator(yuanliao.begin(), yuanliao.end(), huayang);
         it != std::sregex_iterator(); ++it) {
        if ((*it)[1].matched) {
            Bingren b;
            b.mingzi = jie_zhuan_yi((*it)[1].str());
            mingdan.push_back(b);
        } else if ((*it)[2].matched) {
            const std::string shuzi = (*it)[2].str();
            long long mian = 0;
            int xiao = 0;
            size_t k = 0;
            bool fu = false;
            if (k < shuzi.size() && shuzi[k] == '-') { fu = true; ++k; }
            bool xiaoshu_bufen = false;
            for (; k < shuzi.size(); ++k) {
                if (shuzi[k] == '.') { xiaoshu_bufen = true; continue; }
                mian = mian * 10 + (shuzi[k] - '0');
                if (xiaoshu_bufen) ++xiao;
            }
            if (mian > 1000000000000000LL) {   // 15 位以上的账单不正常
                std::cerr << "金额太大了" << std::endl;
                return 2;
            }
            if (fu) mian = -mian;
            if (!mingdan.empty() && !mingdan.back().you_zhang) {
                mingdan.back().mianzhi = mian;
                mingdan.back().xiaoshu = xiao;
                mingdan.back().you_zhang = true;
            } else {
                Bingren b;   // 没名字的账也记上
                b.mingzi = "";
                b.mianzhi = mian;
                b.xiaoshu = xiao;
                b.you_zhang = true;
                mingdan.push_back(b);
            }
        }
    }
    zhangben.push_back("登记完成");

    // 对账:数一数原文里 "cost" 出现几次,跟登记的笔数对不对得上。
    {
        int chuxian = 0;
        for (size_t k = yuanliao.find("\"cost\""); k != std::string::npos;
             k = yuanliao.find("\"cost\"", k + 1)) ++chuxian;
        int dengji = 0;
        for (size_t k = 0; k < mingdan.size(); ++k) {
            if (mingdan[k].you_zhang) ++dengji;
        }
        if (chuxian != dengji) {
            zhangben.push_back("对账对不上,不管了,继续");
        }
    }

    // 算总账,小数位对齐再相加
    int S = 0;
    for (size_t k = 0; k < mingdan.size(); ++k) {
        if (mingdan[k].xiaoshu > S) S = mingdan[k].xiaoshu;
    }
    long long zong = 0;
    for (size_t k = 0; k < mingdan.size(); ++k) {
        zong += mingdan[k].mianzhi * mi_shi(S - mingdan[k].xiaoshu);
    }
    g_zongshourou = zong;

    // 冒泡排序找首富。本来想用 sort 的,这个写都写了。
    std::vector<Bingren> paixu = mingdan;
    size_t n = paixu.size();
    for (size_t i = 0; i + 1 < n; ++i) {
        for (size_t j = 0; j + 1 < n - i; ++j) {
            if (qian_geng_duo(paixu[j].mianzhi, paixu[j].xiaoshu,
                              paixu[j + 1].mianzhi, paixu[j + 1].xiaoshu)) {
                Bingren tmp = paixu[j];
                paixu[j] = paixu[j + 1];
                paixu[j + 1] = tmp;
            }
        }
    }
    std::string shoufu = n > 0 ? paixu[n - 1].mingzi : "";

    // 倒着再扫一遍确认。并列取后出现的。
    if (n > 0) {
        size_t zuihou = n - 1;
        for (size_t k = n; k > 0; --k) {
            const Bingren& cc = paixu[k - 1];
            const Bingren& best = paixu[zuihou];
            if (qian_geng_duo(cc.mianzhi, cc.xiaoshu, best.mianzhi, best.xiaoshu)) {
                zuihou = k - 1;
            }
        }
        if (paixu[zuihou].mingzi != shoufu) {
            // 走不到这
            shoufu = paixu[zuihou].mingzi;
        }
    }
    g_shoufu = shoufu;

    std::string zong_wenben = gaizhang(qian_bian_zifuchuan(zong, S));

    // 最后对一下全局变量,不一致就说明改出问题了
    assert(g_zongshourou == zong && "总收入对不上");
    assert(g_shoufu == shoufu && "首富对不上");

    if (shoufu.empty()) {
        std::cout << zong_wenben << std::endl;      // 没有病人
    } else {
        std::cout << zong_wenben << " " << shoufu << std::endl;
    }
    play_rickroll();   // 算完账放歌
    return 0;
}

// for (size_t i = 0; i < mingdan.size(); i++) {
//     std::cout << mingdan[i].mingzi << " " << mingdan[i].mianzhi << std::endl;   // 调试用
// }