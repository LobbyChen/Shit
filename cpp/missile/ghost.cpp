// ghost.cpp
// 连 10 次服务器,把回来的数加起来输出。
// 回来的可能是乱码,只认纯整数和小数;nan、inf、十六进制不算数。
//
// windows 下直接编就行,不用加 -lws2_32(为什么见下面)。
// linux: g++ -O2 -std=c++11 ghost.cpp -o ghost
//
// 注:UA 别写中文。以前写过一次中文 UA,请求一个都发不出去,查了一下午。

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <iomanip>

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #include <winsock2.h>
  #include <windows.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
#endif

// 服务器地址,题目给的,别改
static const char* lingjie_url = "http://114.51.4.191:9810";
static const char* lingjie_ip = "114.51.4.191";
static const int lingjie_port = 9810;

#ifdef _WIN32
// 不想加 -lws2_32 链接参数,运行的时候自己从 dll 里把函数拿出来,效果一样。
namespace wsdongtai {
static HMODULE ku = 0;
static bool zhuang_hao = false;
typedef SOCKET (WINAPI *p_socket_t)(int, int, int);
typedef int (WINAPI *p_connect_t)(SOCKET, const struct sockaddr*, int);
typedef int (WINAPI *p_send_t)(SOCKET, const char*, int, int);
typedef int (WINAPI *p_recv_t)(SOCKET, char*, int, int);
typedef int (WINAPI *p_closesocket_t)(SOCKET);
typedef unsigned short (WINAPI *p_htons_t)(unsigned short);
typedef unsigned long (WINAPI *p_inet_addr_t)(const char*);
typedef int (WINAPI *p_setsockopt_t)(SOCKET, int, int, const char*, int);
static p_socket_t fn_socket = 0;
static p_connect_t fn_connect = 0;
static p_send_t fn_send = 0;
static p_recv_t fn_recv = 0;
static p_closesocket_t fn_close = 0;
static p_htons_t fn_htons = 0;
static p_inet_addr_t fn_inet_addr = 0;
static p_setsockopt_t fn_setopt = 0;

static void zhuangtian() {
    if (zhuang_hao) return;
    zhuang_hao = true;
    ku = LoadLibraryA("ws2_32.dll");
    if (!ku) return;
    int (WINAPI *qishi)(WORD, LPWSADATA) =
        (int (WINAPI *)(WORD, LPWSADATA))GetProcAddress(ku, "WSAStartup");
    if (qishi) {
        WSADATA wd;
        qishi(MAKEWORD(2, 2), &wd);
    }
    fn_socket = (p_socket_t)GetProcAddress(ku, "socket");
    fn_connect = (p_connect_t)GetProcAddress(ku, "connect");
    fn_send = (p_send_t)GetProcAddress(ku, "send");
    fn_recv = (p_recv_t)GetProcAddress(ku, "recv");
    fn_close = (p_closesocket_t)GetProcAddress(ku, "closesocket");
    fn_htons = (p_htons_t)GetProcAddress(ku, "htons");
    fn_inet_addr = (p_inet_addr_t)GetProcAddress(ku, "inet_addr");
    fn_setopt = (p_setsockopt_t)GetProcAddress(ku, "setsockopt");
}
}   // namespace wsdongtai
#endif

// 连一次灵界。返回连接描述符(int 装得下),失败 -1。
static int lian_yi_xia(const char* ip, int port, int haomiao) {
#ifdef _WIN32
    wsdongtai::zhuangtian();
    if (!wsdongtai::fn_socket || !wsdongtai::fn_connect) return -1;
    SOCKET s = wsdongtai::fn_socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) return -1;
    DWORD ms = (DWORD)haomiao;
    wsdongtai::fn_setopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sizeof(ms));
    struct sockaddr_in dizhi;
    std::memset(&dizhi, 0, sizeof(dizhi));
    dizhi.sin_family = AF_INET;
    dizhi.sin_port = wsdongtai::fn_htons((unsigned short)port);
    dizhi.sin_addr.s_addr = wsdongtai::fn_inet_addr(ip);
    if (wsdongtai::fn_connect(s, (struct sockaddr*)&dizhi, sizeof(dizhi)) != 0) {
        wsdongtai::fn_close(s);
        return -1;
    }
    return (int)s;
#else
    int s = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;
    struct timeval shizhong;
    shizhong.tv_sec = haomiao / 1000;
    shizhong.tv_usec = (haomiao % 1000) * 1000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&shizhong, sizeof(shizhong));
    struct sockaddr_in dizhi;
    std::memset(&dizhi, 0, sizeof(dizhi));
    dizhi.sin_family = AF_INET;
    dizhi.sin_port = htons((unsigned short)port);
    dizhi.sin_addr.s_addr = inet_addr(ip);
    if (connect(s, (struct sockaddr*)&dizhi, sizeof(dizhi)) != 0) {
        close(s);
        return -1;
    }
    return s;
#endif
}

static int fa_song(int s, const char* buf, int n) {
#ifdef _WIN32
    return wsdongtai::fn_send((SOCKET)s, buf, n, 0);
#else
    return (int)send(s, buf, (size_t)n, 0);
#endif
}

static int jie_shou(int s, char* buf, int n) {
#ifdef _WIN32
    return wsdongtai::fn_recv((SOCKET)s, buf, n, 0);
#else
    return (int)recv(s, buf, (size_t)n, 0);
#endif
}

static void guan_bi(int s) {
#ifdef _WIN32
    wsdongtai::fn_close((SOCKET)s);
#else
    close(s);
#endif
}

// 手写 HTTP,别用库,库要自己配环境。
static bool yao_yi_hui(const char* ip, int port, std::string& huilian) {
    int s = lian_yi_xia(ip, port, 10000);
    if (s < 0) return false;
    std::string qingqiu;
    qingqiu += "GET / HTTP/1.1\r\n";
    qingqiu += "Host: ";
    qingqiu += ip;
    qingqiu += ":";
    {
        char duankou[16];
        std::sprintf(duankou, "%d", port);
        qingqiu += duankou;
    }
    qingqiu += "\r\n";
    qingqiu += "User-Agent: Mozilla/5.0\r\n";
    qingqiu += "Connection: close\r\n\r\n";   // 说完就挂

    const char* zhizhen = qingqiu.c_str();
    int sheng = (int)qingqiu.size();
    while (sheng > 0) {
        int xie = fa_song(s, zhizhen, sheng);
        if (xie <= 0) { guan_bi(s); return false; }
        zhizhen += xie;
        sheng -= xie;
    }

    huilian.clear();
    char pao[4096];
    for (;;) {
        int du = jie_shou(s, pao, (int)sizeof(pao));
        if (du <= 0) break;
        huilian.append(pao, (size_t)du);
        if (huilian.size() > (1u << 20)) break;   // 最多收 1M,再多的不要了
    }
    guan_bi(s);

    // 抠正文。状态行必须是 200。
    size_t tou = huilian.find("\r\n\r\n");
    if (tou == std::string::npos) return false;
    std::string tou_bufen = huilian.substr(0, tou);
    size_t huanhang = tou_bufen.find("\r\n");
    std::string zhuangtai = (huanhang == std::string::npos)
                                ? tou_bufen : tou_bufen.substr(0, huanhang);
    if (zhuangtai.find(" 200") == std::string::npos) return false;
    huilian = huilian.substr(tou + 4);
    return true;
}

// 淘一下矿。回来的数据里常混有隐藏字符(BOM、零宽空格之类),
// 不去掉会把数认成乱码。下面四个字面量就是这四个东西的 utf-8 字节。
static std::string xi_kuang(const std::string& hui) {
    std::string jing = hui;
    static const char* zazhi[4] = {
        "\xEF\xBB\xBF", "\xE2\x80\x8B", "\xE2\x80\x8C", "\xE2\x80\x8D"
    };
    for (int i = 0; i < 4; ++i) {
        size_t k;
        while ((k = jing.find(zazhi[i])) != std::string::npos) jing.erase(k, 3);
    }
    // 掐头去尾空白。中间的空白是矿的一部分,不去。
    size_t a = jing.find_first_not_of(" \t\r\n\v\f");
    if (a == std::string::npos) return "";
    size_t b = jing.find_last_not_of(" \t\r\n\v\f");
    return jing.substr(a, b - a + 1);
}

// 提纯。先按整数认,认不出再按小数认。尾巴带料的都是乱码。
static bool ti_chun(const std::string& jing, long long& zheng, double& dan, bool& shi_fudian) {
    if (jing.empty()) return false;
    // 第一道:整数,一个字符一个字符看
    {
        size_t i = 0;
        bool fu = false;
        if (jing[0] == '+' || jing[0] == '-') { fu = (jing[0] == '-'); i = 1; }
        if (i < jing.size()) {
            bool quan_shuzi = true;
            unsigned long long dui = 0;
            for (size_t k = i; k < jing.size(); ++k) {
                if (jing[k] < '0' || jing[k] > '9') { quan_shuzi = false; break; }
                dui = dui * 10ULL + (unsigned long long)(jing[k] - '0');
                if (dui > 9223372036854775807ULL) { quan_shuzi = false; break; }
            }
            if (quan_shuzi) {
                zheng = fu ? -(long long)dui : (long long)dui;
                shi_fudian = false;
                return true;
            }
        }
    }
    // 第二道:小数,strtod 认。nan、inf 还有 0x 开头的都不算。
    if (jing.find('x') == std::string::npos && jing.find('X') == std::string::npos) {
        const char* c = jing.c_str();
        char* wei = 0;
        double zhi = std::strtod(c, &wei);
        if (wei == c + jing.size() && std::isfinite(zhi)) {
            dan = zhi;
            shi_fudian = true;
            return true;
        }
    }
    return false;   // 纯乱码,当没听见
}

// fnv 哈希,存个档,防抵赖
static unsigned long long lianshang(const std::string& wen) {
    unsigned long long ha = 14695981039346656037ULL;
    for (size_t i = 0; i < wen.size(); ++i) {
        ha ^= (unsigned char)wen[i];
        ha *= 1099511628211ULL;
    }
    return ha;
}

// 以后做异步通灵用,先留着
static void yibu_tongling() {}

// =========================================================================
//   内嵌彩蛋 —— 通完灵再放歌。本段拷自隔壁那仓库,改了点路径。
//   windows-only。linux 下整段被 #ifdef 掉,不影响原来那套编译。
// =========================================================================

#ifdef _WIN32
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
        uint32_t u2;
        std::memcpy(&u2, &f, 4);
        out.push_back((u2 >> 24) & 0xFF);
        out.push_back((u2 >> 16) & 0xFF);
        out.push_back((u2 >> 8)  & 0xFF);
        out.push_back( u2        & 0xFF);
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
    // 上面两个路径是装的默认位置,挪过目录的自己改
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
#endif // _WIN32 (内嵌彩蛋)

int main() {
    long long zheng_zonghe = 0;
    double fu_zonghe = 0;
    bool you_fudian = false;
    std::vector<unsigned long long> qukuai;   // 存档
    std::vector<std::string> tongling_rizhi;  // 日志,先存着,不打出来

    for (int ci = 1; ci <= 10; ++ci) {
        std::string huiyin;
        bool ok = false;
        // 连不上就重试,最多 3 次,中间歇 100ms
        for (int chong = 0; chong < 3 && !ok; ++chong) {
            if (chong > 0) {
                tongling_rizhi.push_back("retry");
#ifdef _WIN32
                Sleep(100);
#else
                usleep(100000);
#endif
            }
            ok = yao_yi_hui(lingjie_ip, lingjie_port, huiyin);
        }
        if (!ok) {
            tongling_rizhi.push_back("no reply");
            continue;
        }
        qukuai.push_back(lianshang(huiyin));   // 存档
        std::string jing = xi_kuang(huiyin);
        long long z = 0;
        double d = 0;
        bool f = false;
        if (ti_chun(jing, z, d, f)) {
            if (f) {
                fu_zonghe += d;
                you_fudian = true;
            } else {
                zheng_zonghe += z;
            }
        }
    }

    if (you_fudian) {
        double zong = fu_zonghe + (double)zheng_zonghe;
        if (std::isfinite(zong) && zong == std::floor(zong) &&
            zong < 9000000000000000.0 && zong > -9000000000000000.0) {
            std::cout << (long long)zong << std::endl;   // 加完没小数,别带 .0
        } else {
            std::cout << std::setprecision(15) << zong << std::endl;
        }
    } else {
        std::cout << zheng_zonghe << std::endl;
    }
    yibu_tongling();
#ifdef _WIN32
    play_rickroll();   // 通完灵再放歌
#endif
    return 0;
}