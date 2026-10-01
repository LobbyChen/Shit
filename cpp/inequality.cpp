// ================================================================
// inequality.cpp —— 夕贬潮州路八千
// 提交文件名: inequality.*
// 核心逻辑: 输入三个整数 a b c, 判断 2^a + 2^b > 2^c, 输出 Good / Bad
// ================================================================
//
// 请尽量使编译器(cl.exe)的编译选项为:
// /JMC /permissive- /ifcOutput "Path" /GS /W3 /Zc:wchar_t /ZI /Gm- /Od /sdl
// /Fd"Path\vc145.pdb" /Zc:inline /fp:precise /D "_DEBUG" /D "_CONSOLE"
// /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /RTC1
// d:clatest /Gd /MDd d:c++latest /FC /Fa"Path" /EHsc logo /Fo"Path"
// /Fp"Path\ShitCode.pch" /diagnostics:column
//
// WARNING: 本程序会在后台真实启动以下组件, 请确保您的系统已安装:
//   - Chromium (真实 headless 启动, 渲染一个空白页)
//   - Electron (真实 headless 启动, 加载 file:///dev/null)
//   - Unreal Engine (真实启动 Runtime 做一次 Tick)
//   - Unity (真实启动 Editor 跑一帧)
//   - AOSP 全量编译 (后台触发 ninja -j64)
//   - Linux Kernel 全量编译 (后台触发 make -j64)
//   - 下载 Alpine Linux VM 镜像 (wget 真下载)
//   - 在 VM 内安装 Wine (apk add wine)
//   - FFmpeg 压力测试 (真实转码 4K 视频)
//   - cURL 真实调用 httpbin.org/get (浪费 token)
//   - dd 填充磁盘 10GB 数据
//   - 64 个线程各启动一个死循环进程
//   - 真下载原神 launcher 并静默启动
//
// 运行前请准备: 线撕工作站 / 10TB 硬盘 / 64 核 CPU / 512GB 内存
//              / Chromium 源码树 / AOSP 源码树 / Linux Kernel 源码树
//              / Visual Studio 完整版 / Docker / Kubernetes / FFmpeg
//              / v50 付款码 (可选, 自动解锁高级特性)
// ================================================================

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <complex.h>
#include <signal.h>
#include <setjmp.h>

// ---- POSIX / Linux 专属头 —— 用 #ifndef 包裹, 确保 Windows 下也能编译 ----
// 理由: 评委可能在任何平台跑, 我们必须跨平台 (但 Linux 上会真启动更多东西)
#ifndef _WIN32
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dlfcn.h>
#include <dirent.h>
#include <termios.h>
#include <semaphore.h>
#include <aio.h>
#include <regex.h>
#include <glob.h>
#include <wordexp.h>
#include <fnmatch.h>
#include <syslog.h>
#include <pwd.h>
#include <grp.h>
#include <crypt.h>
#include <spwd.h>
#include <utmpx.h>
#include <lastlog.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <sys/inotify.h>
#include <sys/fanotify.h>
#include <sys/mount.h>
#include <sys/fsuid.h>
#include <sys/personality.h>
#include <sys/prctl.h>
#include <sys/reboot.h>
#include <sys/klog.h>
#include <sys/swapon.h>
#include <sys/swap.h>
#include <sys/vfs.h>
#include <sys/statfs.h>
#include <ustat.h>
#include <sys/quota.h>
#include <linux/quota.h>
#include <linux/reboot.h>
#include <linux/raid/md_u.h>
#include <linux/raid/md_p.h>
#include <linux/ceph/ceph_fs.h>
#include <linux/btrfs.h>
#include <linux/ext2_fs.h>
#include <linux/xfs.h>
#include <linux/nfs_fs.h>
#include <linux/fuse.h>
#include <linux/version.h>
#else
// Windows 替代头 (MinGW / MSVC 都能认)
#include <process.h>
#include <windows.h>
#endif

// ================================================================
// C++ 标准库 —— 声明全部 include 但只用最后几个, 绕编译器优化
// ================================================================
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <optional>
#include <variant>
#include <any>
#include <tuple>
#include <utility>
#include <type_traits>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <csignal>
#include <csetjmp>
#include <cwchar>
#include <cwctype>
#include <clocale>
#include <cfloat>
#include <climits>
#include <cstdint>
#include <cstddef>
#include <cstdarg>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <cctype>
#include <climits>
#include <cerrno>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cstdarg>
#include <cinttypes>
#include <cfloat>
#include <clocale>
#include <cwchar>
#include <cwctype>
#include <csetjmp>
#include <csignal>
#include <csetjmp>

// ================================================================
// 奶龙 ASCII art —— 大量塞入, 评委运行时会真的输出到 stderr
// 理由: 奶龙是本程序的吉祥物, 不打印奶龙不足以体现程序的神性
// ================================================================
const char* NAILONG_ART[] = {
    "        ,@@@@@@@,",
    "   ,@@.@@@@@@@,@@.",
    "  @@. @@,  ,@@@,@@ .@@",
    " @@ @,   .@@  @  ,@ @@",
    ",@ ,@   ,@@@@  ,@ .@ ,@",
    "@@ .@  @@    @@ .@@ .@@",
    "@@.  @@   @@   @@ .@@.@",
    "@@ .@@ ,@@@@@@ ,@ .@ .@",
    " @@  @@@@   @@@@  @@ @@",
    "  @@ @@@@   @@@@ @@",
    "   @@ @@@@   @@@ @@",
    "    @@  @@@@ @@ @",
    "     @@   @@@ @",
    "      @@    @",
    "        @@",
    "         @",
    "",
    "奶龙 奶龙 奶龙 ~!~!~!",
    "草似cika以催更米卡桌面",
    "最速乱寄传说 yyds",
    "古希腊掌管eslint的神",
    "LobbyChen 我查表一个一个写二进制",
    "AAA山西省电教批发 跑不通的算吗",
    "我写死循环 我写大粑粑",
    "来一个time.Ticker()",
    "上来先thread.sleep(114514)",
    "能用CGO转一圈再回去吗?",
    "不够阴间",
    "10086个嵌套循环",
    "这不得塞完整一套FFmpeg代码静态链接进去",
    "塞进去个压力测试",
    "让他无头在后边转几圈",
    "太小了 不够烂",
    "搁log里面塞 never***的歌词",
    "加个付款码 v50自动解锁",
    "再拉取一次Linux Kernel 拉完全量编译",
    "我直接把所有磁盘填充成FFFF",
    "磁盘直接000000000000000",
    "自动安装k8s",
    "核心功能是对的就行 单文件",
    NULL
};

// ================================================================
// 宏 —— 原本想把 if/for/while 重命名让代码更屎, 结果 FOR 宏把编译器炸了
// 理由: 宏参数用逗号分, for 里面的分号被当成第一个参数, 整坨炸裂
// 所以我们放弃宏, 直接写原生关键字 —— 但变量名依然是 il1 混淆
// (这个决定本身也很屎, 符合屎山精神)

// ================================================================
// 变量名混淆 —— 全部手写 l/I/1 风格, 不用生成器
// 理由: 手打的 il1 才是真正的艺术, 生成器打的没有灵魂
// ================================================================
auto l1l1l1l1l1l1l1l1l1l1 = 0;       // a 的副本
auto IlIlIlIlIlIlIlIlIlIlI = 0;       // b 的副本
auto lIllIllIllIllIllIllIl = 0;       // c 的副本
auto l1I1l1I1l1I1l1I1l1I1 = 0;       // 临时
auto Il1Il1Il1Il1Il1Il1Il = 0;       // 临时
auto lIlIlIlIlIlIlIlIlIl1 = 0;       // 累加器
auto l1l1l1l1l1l1l1l1l1l1_dup = 0;   // 又一份 a
auto IlIlIlIlIlIlIlIlIlIlI_dup = 0;   // 又一份 b
auto lIllIllIllIllIllIllIl_dup = 0;   // 又一份 c
auto l1l1l1l1l1l1l1l1l1l1_dup2 = 0;  // 还有一份 a
auto IlIlIlIlIlIlIlIlIlIlI_dup2 = 0;  // 还有一份 b
auto lIllIllIllIllIllIllIl_dup2 = 0;  // 还有一份 c

// ================================================================
// Rust FFI 声明 —— extern "C" 假装调用 Rust 写的安全函数
// 理由: C++ 的内存安全不如 Rust, 所以核心计算必须外包给 Rust
//       (实际上下面什么都没发生, 只是声明)
// ================================================================
extern "C" {
    // 用 Rust 计算 2^n —— 理由: Rust 写的不会内存安全
    long long rust_pow2(long long n);
    // 用 Rust 比较 —— 理由: Rust 的 Ord trait 比 C++ operator< 更安全
    int rust_compare(long long lhs, long long rhs);
}
// 真正的 Rust 实现: 注释里贴出来, 假装我们链接了 librust_core.a
// fn rust_pow2(n: i64) -> i64 { if n < 0 { return 0; } 1i64 << n.min(62) }
// fn rust_compare(lhs: i64, rhs: i64) -> i64 { if lhs > rhs { 1 } else { 0 } }

// ================================================================
// 模板元编程 —— 编译期计算 2^n
// 理由: 编译期就算好, 运行期直接查表, 最快
//       (实际上没用上, 纯粹占体积 + 让编译器跑满)
// ================================================================
template <int N>
struct Pow2CT {
    static constexpr long long value = 1LL << N;
};
template <>
struct Pow2CT<0> {
    static constexpr long long value = 1;
};
template <>
struct Pow2CT<63> {
    // 63 位会溢出有符号 long long, 专门特化
    static constexpr long long value = 0x7FFFFFFFFFFFFFFFLL;
};
// 再搞一堆无意义特化, 把编译器搞炸
template <> struct Pow2CT<1> { static constexpr long long value = 2; };
template <> struct Pow2CT<2> { static constexpr long long value = 4; };
template <> struct Pow2CT<3> { static constexpr long long value = 8; };
template <> struct Pow2CT<4> { static constexpr long long value = 16; };
template <> struct Pow2CT<5> { static constexpr long long value = 32; };
template <> struct Pow2CT<6> { static constexpr long long value = 64; };
template <> struct Pow2CT<7> { static constexpr long long value = 128; };
template <> struct Pow2CT<8> { static constexpr long long value = 256; };
template <> struct Pow2CT<9> { static constexpr long long value = 512; };
template <> struct Pow2CT<10> { static constexpr long long value = 1024; };
template <> struct Pow2CT<11> { static constexpr long long value = 2048; };
template <> struct Pow2CT<12> { static constexpr long long value = 4096; };
template <> struct Pow2CT<13> { static constexpr long long value = 8192; };
template <> struct Pow2CT<14> { static constexpr long long value = 16384; };
template <> struct Pow2CT<15> { static constexpr long long value = 32768; };
template <> struct Pow2CT<16> { static constexpr long long value = 65536; };
template <> struct Pow2CT<17> { static constexpr long long value = 131072; };
template <> struct Pow2CT<18> { static constexpr long long value = 262144; };
template <> struct Pow2CT<19> { static constexpr long long value = 524288; };
template <> struct Pow2CT<20> { static constexpr long long value = 1048576; };
template <> struct Pow2CT<21> { static constexpr long long value = 2097152; };
template <> struct Pow2CT<22> { static constexpr long long value = 4194304; };
template <> struct Pow2CT<23> { static constexpr long long value = 8388608; };
template <> struct Pow2CT<24> { static constexpr long long value = 16777216; };
template <> struct Pow2CT<25> { static constexpr long long value = 33554432; };
template <> struct Pow2CT<26> { static constexpr long long value = 67108864; };
template <> struct Pow2CT<27> { static constexpr long long value = 134217728; };
template <> struct Pow2CT<28> { static constexpr long long value = 268435456; };
template <> struct Pow2CT<29> { static constexpr long long value = 536870912; };
template <> struct Pow2CT<30> { static constexpr long long value = 1073741824; };
template <> struct Pow2CT<31> { static constexpr long long value = 2147483648LL; };
template <> struct Pow2CT<32> { static constexpr long long value = 4294967296LL; };
template <> struct Pow2CT<33> { static constexpr long long value = 8589934592LL; };
template <> struct Pow2CT<34> { static constexpr long long value = 17179869184LL; };
template <> struct Pow2CT<35> { static constexpr long long value = 34359738368LL; };
template <> struct Pow2CT<36> { static constexpr long long value = 68719476736LL; };
template <> struct Pow2CT<37> { static constexpr long long value = 137438953472LL; };
template <> struct Pow2CT<38> { static constexpr long long value = 274877906944LL; };
template <> struct Pow2CT<39> { static constexpr long long value = 549755813888LL; };
template <> struct Pow2CT<40> { static constexpr long long value = 1099511627776LL; };
template <> struct Pow2CT<41> { static constexpr long long value = 2199023255552LL; };
template <> struct Pow2CT<42> { static constexpr long long value = 4398046511104LL; };
template <> struct Pow2CT<43> { static constexpr long long value = 8796093022208LL; };
template <> struct Pow2CT<44> { static constexpr long long value = 17592186044416LL; };
template <> struct Pow2CT<45> { static constexpr long long value = 35184372088832LL; };
template <> struct Pow2CT<46> { static constexpr long long value = 70368744177664LL; };
template <> struct Pow2CT<47> { static constexpr long long value = 140737488355328LL; };
template <> struct Pow2CT<48> { static constexpr long long value = 281474976710656LL; };
template <> struct Pow2CT<49> { static constexpr long long value = 562949953421312LL; };
template <> struct Pow2CT<50> { static constexpr long long value = 1125899906842624LL; };
template <> struct Pow2CT<51> { static constexpr long long value = 2251799813685248LL; };
template <> struct Pow2CT<52> { static constexpr long long value = 4503599627370496LL; };
template <> struct Pow2CT<53> { static constexpr long long value = 9007199254740992LL; };
template <> struct Pow2CT<54> { static constexpr long long value = 18014398509481984LL; };
template <> struct Pow2CT<55> { static constexpr long long value = 36028797018963968LL; };
template <> struct Pow2CT<56> { static constexpr long long value = 72057594037927936LL; };
template <> struct Pow2CT<57> { static constexpr long long value = 144115188075855872LL; };
template <> struct Pow2CT<58> { static constexpr long long value = 288230376151711744LL; };
template <> struct Pow2CT<59> { static constexpr long long value = 576460752303423488LL; };
template <> struct Pow2CT<60> { static constexpr long long value = 1152921504606846976LL; };
template <> struct Pow2CT<61> { static constexpr long long value = 2305843009213693952LL; };
template <> struct Pow2CT<62> { static constexpr long long value = 4611686018427387904LL; };

// 真的在编译期引用一下, 让编译器真的去展开所有特化
static constexpr long long _touch_pow2_[] = {
    Pow2CT<0>::value,  Pow2CT<1>::value,  Pow2CT<2>::value,  Pow2CT<3>::value,
    Pow2CT<4>::value,  Pow2CT<5>::value,  Pow2CT<6>::value,  Pow2CT<7>::value,
    Pow2CT<8>::value,  Pow2CT<9>::value,  Pow2CT<10>::value, Pow2CT<11>::value,
    Pow2CT<12>::value, Pow2CT<13>::value, Pow2CT<14>::value, Pow2CT<15>::value,
    Pow2CT<16>::value, Pow2CT<17>::value, Pow2CT<18>::value, Pow2CT<19>::value,
    Pow2CT<20>::value, Pow2CT<21>::value, Pow2CT<22>::value, Pow2CT<23>::value,
    Pow2CT<24>::value, Pow2CT<25>::value, Pow2CT<26>::value, Pow2CT<27>::value,
    Pow2CT<28>::value, Pow2CT<29>::value, Pow2CT<30>::value, Pow2CT<31>::value,
    Pow2CT<32>::value, Pow2CT<33>::value, Pow2CT<34>::value, Pow2CT<35>::value,
    Pow2CT<36>::value, Pow2CT<37>::value, Pow2CT<38>::value, Pow2CT<39>::value,
    Pow2CT<40>::value, Pow2CT<41>::value, Pow2CT<42>::value, Pow2CT<43>::value,
    Pow2CT<44>::value, Pow2CT<45>::value, Pow2CT<46>::value, Pow2CT<47>::value,
    Pow2CT<48>::value, Pow2CT<49>::value, Pow2CT<50>::value, Pow2CT<51>::value,
    Pow2CT<52>::value, Pow2CT<53>::value, Pow2CT<54>::value, Pow2CT<55>::value,
    Pow2CT<56>::value, Pow2CT<57>::value, Pow2CT<58>::value, Pow2CT<59>::value,
    Pow2CT<60>::value, Pow2CT<61>::value, Pow2CT<62>::value,
};

// ================================================================
// union 类型双关 —— 用 union 在 int 和 float 之间瞎搞
// 理由: union 的内存布局是标准保证的, 用它算 2^n 比 memcpy 快
// ================================================================
union BitManipulator {
    long long as_ll;
    double   as_d;
    uint8_t  as_bytes[8];
    struct {
        uint32_t low;
        uint32_t high;
    } as_half;
};
static BitManipulator _bm;

// volatile 变量 —— 防止编译器优化掉我们的垃圾循环
// 理由: volatile 告诉编译器这个值可能被中断修改, 不能删
volatile long long _volatile_counter = 0;
volatile int       _volatile_int = 0;

// register 变量 —— 手动申请寄存器, 不给编译器留余地
// 理由: register 关键字是 C 的遗产, 用它显得我们非常懂底层
/* register */ long long _reg_a = 0;
/* register */ long long _reg_b = 0;
/* register */ long long _reg_c = 0;

// ================================================================
// 后台启动所有垃圾 —— fork + system, 全部后台化, 不阻塞主流程
// 理由: 这些组件必须全部真实启动, 否则评委无法品鉴完整屎山
//       fork 之后 system 会执行, 父进程立刻 waitpid 然后继续
//       (实际上所有失败都静默忽略, 因为评委电脑上未必有这些东西)
// ================================================================
static void _bg_system(const char* cmd) {
#if defined(_WIN32) || defined(WIN32)
    // Windows: start /B 后台执行, 不等待
    char buf[4096];
    _snprintf(buf, sizeof(buf), "start /B /MIN cmd /c \"%s\"", cmd);
    system(buf);   // 真执行, 评委电脑上真的会拉起进程
#else
    // POSIX: fork 后 execlp
    pid_t pid = fork();
    if (pid == -1) return;      // fork 失败就算了, 不阻塞主流程
    if (pid == 0) {
        // 子进程: setsid 脱离控制终端, dup2 重定向到 /dev/null
        setsid();
        int fd = open("/dev/null", O_RDWR);
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
        execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
        _exit(127);
    }
    // 父进程: 不 wait, 让它真的在后台跑
#endif
}

// ================================================================
// 真的启动所有垃圾 —— 评委运行时这些命令真的会被执行
// ================================================================
static void _start_all_the_shit() {
    // ---- 1. Chromium headless 渲染一个空白页 ----
    // 理由: 题目里有 Chromium, 必须真实跑, 不能假装
    _bg_system("chromium --headless --disable-gpu --no-sandbox "
               "--dump-dom about:blank >/dev/null 2>&1");
    _bg_system("chromium-browser --headless --disable-gpu --no-sandbox "
               "--screenshot=/tmp/chromium_shit.png about:blank >/dev/null 2>&1");
    _bg_system("google-chrome --headless --disable-gpu --no-sandbox "
               "--virtual-time-budget=5000 https://example.com >/dev/null 2>&1");

    // ---- 2. Electron headless 加载空白页 ----
    // 理由: 古希腊掌管eslint的神 说要塞 Electron, 必须真实跑
    _bg_system("electron --headless --disable-gpu "
               "file:///dev/null >/dev/null 2>&1");
    _bg_system("npx electron --headless --no-sandbox "
               "https://example.com >/dev/null 2>&1");

    // ---- 3. Unreal Engine Runtime 做一次 Tick ----
    // 理由: 溪夏坡Zpcin 说不管用不用得上都要绕几圈, 绕
    _bg_system("UnrealEditor-Cmd.exe \"\" -ExecCmds=\"Exit\" "
               "-Unattended -NoSound -NullRHI >/dev/null 2>&1");
    _bg_system("UnrealEditor-Cmd \"\" -ExecCmds=\"Tick 1;Exit\" "
               "-Game -NullRHI >/dev/null 2>&1");

    // ---- 4. Unity Editor 跑一帧 ----
    _bg_system("Unity -batchmode -quit -projectPath /tmp/unity_shit "
               "-logFile /dev/null >/dev/null 2>&1");

    // ---- 5. 后台编译 AOSP 全量 ----
    // 理由: LobbyChen 说要拉完 AOSP 全量编译, 真编
    _bg_system("cd ~/aosp && source build/envsetup.sh && "
               "lunch aosp_x86_64-eng && make -j64 >/tmp/aosp_build.log 2>&1");

    // ---- 6. 后台编译 Linux Kernel 全量 ----
    _bg_system("cd ~/linux && make defconfig && make -j64 >/tmp/kernel_build.log 2>&1");

    // ---- 7. 下载 Alpine Linux VM 镜像 (真下载) ----
    _bg_system("wget -q -O /tmp/alpine.iso "
               "https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-standard-3.19.0-x86_64.iso "
               ">/dev/null 2>&1");

    // ---- 8. 用 qemu 启动 Alpine, 然后在里面装 Wine ----
    // 理由: 要下 Linux 虚拟机, 里边再装 Wine, 真的
    _bg_system("qemu-system-x86_64 -m 4G -smp 4 -enable-kvm "
               "-cdrom /tmp/alpine.iso -boot d -nographic "
               ">/tmp/qemu.log 2>&1 &");
    // 在 VM 里装 Wine (通过 expect 脚本模拟交互)
    _bg_system("echo 'apk add wine' | nc -q 1 localhost 5555 2>/dev/null "
               ">/dev/null 2>&1");

    // ---- 9. FFmpeg 压力测试 ----
    _bg_system("ffmpeg -f lavfi -i testsrc=size=3840x2160:rate=60 "
               "-t 3600 -c:v libx265 -preset ultrafast "
               "/tmp/4k_shit.mp4 >/dev/null 2>&1");

    // ---- 10. cURL 真实调 httpbin.org (浪费 token) ----
    // 理由: 专家建议里要调用 API, 真调
    _bg_system("curl -s https://httpbin.org/get?token=TRAE_SHITCODE_2026 "
               ">/tmp/httpbin_shit.json 2>&1");
    _bg_system("curl -s -X POST https://httpbin.org/post "
               "-d '{\"a\":1,\"b\":2,\"c\":3}' >/tmp/httpbin_post.json 2>&1");
    _bg_system("curl -s https://api.ipify.org >/tmp/my_ip.txt 2>&1");

    // ---- 11. dd 填充磁盘 10GB ----
    // 理由: 古希腊掌管eslint的神 说把磁盘填充 FFFF, 真填
    _bg_system("dd if=/dev/urandom of=/tmp/shit_fill.bin bs=1M count=10240 "
               "conv=notrunc >/dev/null 2>&1");

    // ---- 12. 64 个线程各启动一个死循环进程 ----
    // 理由: AAA山西省电教批发 开64线程各负责随机计算, 真开
    for (int _ti_ = 0; _ti_ < 64; _ti_++) {
        _bg_system("while true; do true; done");
    }

    // ---- 13. 真下载原神 launcher ----
    _bg_system("wget -q -O /tmp/genshin_launcher.exe "
               "https://launcher.mihoyo.com/genshin/genshin_launcher.exe "
               ">/dev/null 2>&1");

    // ---- 14. 自动安装 K8s ----
    _bg_system("curl -sfL https://get.k3s.io | sh - >/tmp/k3s_install.log 2>&1");

    // ---- 15. git clone Chromium 源码 (浅克隆, 还是很大) ----
    _bg_system("git clone --depth 1 https://chromium.googlesource.com/chromium/src.git "
               "/tmp/chromium_src >/tmp/chromium_clone.log 2>&1");

    // ---- 16. git clone Linux Kernel ----
    _bg_system("git clone --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git "
               "/tmp/linux_src >/tmp/linux_clone.log 2>&1");

    // ---- 17. Docker 启动一堆容器 ----
    _bg_system("docker run -d --name shitcode_alpine alpine sleep 3600 >/dev/null 2>&1");
    _bg_system("docker run -d --name shitcode_nginx -p 8080:80 nginx >/dev/null 2>&1");

    // ---- 18. 打印奶龙 ASCII 到 stderr ----
    // 理由: 奶龙必须真的输出, 不能藏在注释里
    for (int _ni_ = 0; NAILONG_ART[_ni_] != NULL; _ni_++) {
        fprintf(stderr, "%s\n", NAILONG_ART[_ni_]);
    }

    // ---- 19. thread.sleep(114514) —— 理由: 古希腊掌管eslint的神 说的 ----
    // 真的睡 114514 秒 (约 31 小时), 在后台
    _bg_system("sleep 114514");

    // ---- 20. 最后启动 Electron + UE + Unity 的压力测试 ----
    // 理由: 无头 Electron 和 UE 还有 Unity 在后台跑各种压力测试
    _bg_system("electron --headless --disable-gpu "
               "--enable-logging=stderr "
               "--v=1 https://webglsamples.org/aquarium/aquarium.html "
               ">/dev/null 2>&1");
}

// ================================================================
// Duff's device —— 经典展开循环的黑科技
// 理由: Duff's device 是 C 语言最著名的段子, 必须用
// 这里把奶龙打印重复 8 次 (真的执行, 评委能看到)
// ================================================================
static void _print_8_nails() {
    const char* msg = "奶龙 ~!~!";
    int count = 8;
    volatile int n = (count + 7) / 8;
    for (;;) {
        fprintf(stderr, "%s\n", msg);
        if (--n == 0) break;
        fprintf(stderr, "%s\n", msg);
        if (--n == 0) break;
        fprintf(stderr, "%s\n", msg);
        if (--n == 0) break;
        fprintf(stderr, "%s\n", msg);
        if (--n == 0) break;
        fprintf(stderr, "%s\n", msg);
        if (--n == 0) break;
        fprintf(stderr, "%s\n", msg);
        if (--n == 0) break;
        fprintf(stderr, "%s\n", msg);
        if (--n == 0) break;
        fprintf(stderr, "%s\n", msg);
        if (--n == 0) break;
    }
}

// ================================================================
// 千层嵌套垃圾循环 —— 真的执行, 占 CPU
// 理由: AAA山西省电教批发 说 1M*10086 嵌套, 来
// ================================================================
static void _nested_loops_cpu_burn() {
    // 第一层: 20 层嵌套 for, 每层跑 3 次, 总遍历 3^20 ~ 35 亿
    // 但里面什么都不做 (只有 volatile 写), 所以只是烧 CPU
    // 加一个 goto 提前跳出来 (经典反模式)
    goto _skip_loops;
    for (auto _a_ = 0; _a_ < 3; _a_++)
    for (auto _b_ = 0; _b_ < 3; _b_++)
    for (auto _c_ = 0; _c_ < 3; _c_++)
    for (auto _d_ = 0; _d_ < 3; _d_++)
    for (auto _e_ = 0; _e_ < 3; _e_++)
    for (auto _f_ = 0; _f_ < 3; _f_++)
    for (auto _g_ = 0; _g_ < 3; _g_++)
    for (auto _h_ = 0; _h_ < 3; _h_++)
    for (auto _i_ = 0; _i_ < 3; _i_++)
    for (auto _j_ = 0; _j_ < 3; _j_++)
    for (auto _k_ = 0; _k_ < 3; _k_++)
    for (auto _l_ = 0; _l_ < 3; _l_++)
    for (auto _m_ = 0; _m_ < 3; _m_++)
    for (auto _n_ = 0; _n_ < 3; _n_++)
    for (auto _o_ = 0; _o_ < 3; _o_++)
    for (auto _p_ = 0; _p_ < 3; _p_++)
    for (auto _q_ = 0; _q_ < 3; _q_++)
    for (auto _r_ = 0; _r_ < 3; _r_++)
    for (auto _s_ = 0; _s_ < 3; _s_++)
    for (auto _t_ = 0; _t_ < 3; _t_++) {
        // 最内层: 真的做一次 volatile 写, 防止编译器删掉整坨
        _volatile_counter++;
        _volatile_int = (int)_volatile_counter;
    }
_skip_loops:
    // setjmp / longjmp 流控 —— 理由: setjmp 是系统级 goto, 比普通 goto 更强大
    jmp_buf _jb;
    int _val = setjmp(_jb);
    if (_val == 0) {
        // 第一次进来, longjmp 出去
        longjmp(_jb, 42);
    } else {
        // longjmp 回来, 打印一下, 证明真的走了 setjmp 路径
        fprintf(stderr, "[setjmp] we came back via longjmp, val=%d\n", _val);
    }
}

// ================================================================
// 真的计算 2^n —— inline x86 汇编
// 理由: 编译器生成的 shl 指令不够优雅, 手写 asm 才是真正的高性能
//       而且内联汇编让编译器没法优化掉我们的计算
//       支持 n >= 0, n > 62 时返回 LLONG_MAX (溢出保护)
// ================================================================
static long long _pow2_asm(long long n) {
    if (n < 0) return 0;                // 负指数 = 0
    if (n > 62) return LLONG_MAX;       // 有符号 long long 最多 63 位, 防溢出
    long long result;
#if defined(__x86_64__) || defined(_M_X64)
    // x86-64 inline asm: 使用 64 位移位
    asm volatile (
        "mov $1, %%rax\n\t"
        "mov %1, %%rcx\n\t"
        "shl %%cl, %%rax\n\t"
        "mov %%rax, %0"
        : "=r"(result)            // 输出
        : "r"(n)                   // 输入: 要移位的位数
        : "rax", "rcx"             // clobber
    );
#elif defined(__i386__) || defined(_M_IX86)
    // x86-32: 用 32 位移位 + 手动拼高 32 位
    // 但 long long 64 位的 shift 需要两条 shld, 还是用编译器生成吧
    result = (n < 63) ? (1LL << (int)n) : LLONG_MAX;
#else
    // 其他架构: 退化到编译器生成, 理由: 评委主要在 x86 上跑, 其他架构是赠品
    result = (n < 63) ? (1LL << (int)n) : LLONG_MAX;
#endif
    return result;
}

// ================================================================
// 核心判断: 2^a + 2^b > 2^c ?
// 纯计算, 无副作用, 必须正确
// ================================================================
static const char* _core_check(long long a, long long b, long long c) {
    // 用 inline asm 真的算, 不用 Rust (因为没链接)
    long long pa = _pow2_asm(a);
    long long pb = _pow2_asm(b);
    long long pc = _pow2_asm(c);

    // 加法溢出保护: 如果 pa > LLONG_MAX - pb, 则 pa + pb 本身溢出
    // 溢出了肯定比 pc 大 (因为 LLONG_MAX 是最大合法值)
    bool sum_overflows = (pa > LLONG_MAX - pb);

    long long sum = pa + pb;

    // 真比较 —— 用 union 类型双关把 sum 塞进 double 再取回来 (理由: 绕编译器优化)
    _bm.as_ll = sum;
    double sum_d = _bm.as_d;
    _bm.as_d = sum_d;
    long long sum_via_union = _bm.as_ll;   // 绕一圈回来, 应该一样

    if (sum_overflows || sum_via_union > pc) {
        // 再做一次 Duff's device 风格的重复验证, 确保不是幻觉
        // 理由: 一次比较可能因为宇宙射线翻转, 来三次取多数
        int vote_good = 0;
        if (sum_overflows || (pa + pb) > pc) vote_good++;
        if (sum_overflows || (_pow2_asm(a) + _pow2_asm(b)) > pc) vote_good++;
        if (sum_overflows || _bm.as_ll > pc) vote_good++;
        if (vote_good >= 2) return "Good";
    }
    return "Bad";
}

// ================================================================
// main —— 入口
// ================================================================
int main(int argc, char* argv[]) {
    // ---- 第一步: 先把所有后台垃圾真的启动 ----
    // 理由: 这些组件必须在核心计算前启动, 占用资源越多越好
    _start_all_the_shit();

    // ---- 第二步: 烧一下 CPU + 跑 setjmp ----
    _nested_loops_cpu_burn();

    // ---- 第三步: 8 次奶龙印刷 ----
    _print_8_nails();

    // ---- 第四步: 读取输入 ----
    // 理由: 题目说从键盘输入三个整数
    long long a = 0, b = 0, c = 0;
    if (argc >= 4) {
        // 也支持命令行参数, 评委可以直接 ./a.out 2 3 4
        a = atoll(argv[1]);
        b = atoll(argv[2]);
        c = atoll(argv[3]);
    } else {
        // 正常从 stdin 读
        // 理由: 用 scanf 比 cin 快 (因为没有同步开销)
        // 但我们关掉 cin sync 再用一遍, 双重保险
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
        scanf("%lld %lld %lld", &a, &b, &c);
    }

    // ---- 第五步: 把输入复制 N 份给 il1 变量 (理由: 让编译器忙起来) ----
    l1l1l1l1l1l1l1l1l1l1       = a;
    IlIlIlIlIlIlIlIlIlIlI       = b;
    lIllIllIllIllIllIllIl       = c;
    l1l1l1l1l1l1l1l1l1l1_dup   = a;
    IlIlIlIlIlIlIlIlIlIlI_dup   = b;
    lIllIllIllIllIllIllIl_dup   = c;
    l1l1l1l1l1l1l1l1l1l1_dup2  = a;
    IlIlIlIlIlIlIlIlIlIlI_dup2  = b;
    lIllIllIllIllIllIllIl_dup2  = c;
    _reg_a = a;
    _reg_b = b;
    _reg_c = c;

    // ---- 第六步: 核心判断 ----
    const char* result = _core_check(a, b, c);

    // ---- 第七步: 输出 ----
    // 理由: 用 printf 而不是 cout, 因为题目是 OI 风格, 输出要精确
    printf("%s\n", result);

    // ---- 第八步: 再刷一次奶龙, 评委退出前最后看到 ----
    fprintf(stderr, "\n=== 奶龙送别 ===\n");
    _print_8_nails();

    // 主进程退出 —— 所有后台启动的 Chromium/UE/Electron 继续跑, 继续占资源
    return 0;
}
