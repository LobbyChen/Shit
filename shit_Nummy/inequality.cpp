// inequality.cpp - Q1
// 判断 2^a + 2^b 是否大于 2^c
// 比烂大赛：337+ include、2048 层嵌套循环、32 线程冗余计算、内存泄漏、手写 PRNG
// 在代码运行中对数据做手写加密解密链路（MD5/SHA1/RC4/TEA/XOR/Caesar/Feistel）

// === 137 个 C/C++ 标准库头文件 ===
#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <bitset>
#include <charconv>
#include <chrono>
#include <codecvt>
#include <compare>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <deque>
#include <execution>
#include <expected>
#include <filesystem>
#include <format>
#include <forward_list>
#include <fstream>
#include <functional>
#include <future>
#include <generator>
#include <initializer_list>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <latch>
#include <limits>
#include <list>
#include <locale>
#include <map>
#if __has_include(<mdspan>)
#include <mdspan>
#endif
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
#include <print>
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <spanstream>
#include <sstream>
#include <stack>
#if __has_include(<stacktrace>)
#include <stacktrace>
#endif
#if __has_include(<stdatomic>)
#include <stdatomic>
#endif
#include <stdexcept>
#if __has_include(<stdfloat>)
#include <stdfloat>
#endif
#include <stop_token>
#include <streambuf>
#include <string>
#include <string_view>
#include <strstream>
#include <syncstream>
#include <system_error>
#include <text_encoding>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>
#include <version>

#include <cassert>
#include <ccomplex>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctgmath>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <inttypes.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
#include <uchar.h>
#include <wchar.h>
#include <wctype.h>
#include <complex.h>
#include <stdnoreturn.h>

// === 200 个手写工具函数头文件（非加密，纯工具） ===
#include "util/all_headers.hpp"

// ============================================================
// 手写加密算法（全在 cpp 运行时实现，对数据做加密解密链路）
// 明明标准库/OpenSSL 有现成的，偏要自己手写一遍——这才是比烂大赛精神
// ============================================================

class ManualLCG {
    uint64_t state_;
public:
    explicit ManualLCG(uint64_t seed) : state_(seed) {}
    uint64_t next() {
        state_ = state_ * 6364136223846793005ULL + 1442695040888963407ULL;
        return state_;
    }
    uint64_t uniform(uint64_t bound) { return next() % bound; }
};


struct ManualMD5 {
    static std::array<uint8_t, 16> hash(const std::vector<uint8_t>& data) {
        std::array<uint8_t, 16> result{};
        uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE, h3 = 0x10325476;
        uint8_t block[64] = {0};
        for (size_t i = 0; i < data.size() && i < 64; i++) block[i] = data[i];
        if (data.size() < 64) block[data.size()] = 0x80;
        uint32_t M[16];
        for (int i = 0; i < 16; i++) {
            M[i] = block[i*4] | (block[i*4+1]<<8) | (block[i*4+2]<<16) | (block[i*4+3]<<24);
        }
        uint32_t A = h0, B = h1, C = h2, D = h3;
        uint32_t F = (B & C) | (~B & D);
        A = A + F + M[0] + 0xd76aa478;
        A = ((A << 7) | (A >> 25));
        A = A + B;
        for (int i = 0; i < 4; i++) {
            result[i]    = (A >> (i*8)) & 0xFF;
            result[i+4]  = (B >> (i*8)) & 0xFF;
            result[i+8]  = (C >> (i*8)) & 0xFF;
            result[i+12] = (D >> (i*8)) & 0xFF;
        }
        return result;
    }
};

// === 手写 SHA1（写得烂：只跑 1 轮） ===
struct ManualSHA1 {
    static std::array<uint8_t, 20> hash(const std::vector<uint8_t>& data) {
        std::array<uint8_t, 20> result{};
        uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE, h3 = 0x10325476, h4 = 0xC3D2E1F0;
        uint8_t block[64] = {0};
        for (size_t i = 0; i < data.size() && i < 64; i++) block[i] = data[i];
        if (data.size() < 64) block[data.size()] = 0x80;
        uint32_t w[80];
        for (int i = 0; i < 16; i++) {
            w[i] = (block[i*4]<<24) | (block[i*4+1]<<16) | (block[i*4+2]<<8) | block[i*4+3];
        }
        for (int i = 16; i < 80; i++) {
            uint32_t t = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
            w[i] = ((t << 1) | (t >> 31));
        }
        uint32_t a=h0, b=h1, c=h2, d=h3, e=h4;
        // 烂：只用第 1 轮的 20 步里的 1 步
        uint32_t f = (b & c) | (~b & d);
        uint32_t tmp = ((a << 5) | (a >> 27)) + f + e + 0x5A827999 + w[0];
        e = d; d = c; c = ((b << 30) | (b >> 2)); b = a; a = tmp;
        h0 = a; h1 = b; h2 = c; h3 = d; h4 = e;
        for (int i = 0; i < 4; i++) {
            result[i]    = (h0 >> (24-i*8)) & 0xFF;
            result[i+4]  = (h1 >> (24-i*8)) & 0xFF;
            result[i+8]  = (h2 >> (24-i*8)) & 0xFF;
            result[i+12] = (h3 >> (24-i*8)) & 0xFF;
            result[i+16] = (h4 >> (24-i*8)) & 0xFF;
        }
        return result;
    }
};

// === 手写 RC4（对称流密码） ===
struct ManualRC4 {
    static std::vector<uint8_t> apply(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        uint8_t S[256];
        for (int i = 0; i < 256; i++) S[i] = static_cast<uint8_t>(i);
        int j = 0;
        for (int i = 0; i < 256; i++) {
            j = (j + S[i] + (key.empty() ? 0 : key[i % key.size()])) & 0xFF;
            std::swap(S[i], S[j]);
        }
        std::vector<uint8_t> out;
        out.reserve(data.size());
        int i = 0; j = 0;
        for (uint8_t b : data) {
            i = (i + 1) & 0xFF;
            j = (j + S[i]) & 0xFF;
            std::swap(S[i], S[j]);
            uint8_t k = S[(S[i] + S[j]) & 0xFF];
            out.push_back(b ^ k);
        }
        return out;
    }
};

// === 手写 TEA（分组密码） ===
struct ManualTEA {
    static void enc_block(uint32_t v[2], const uint32_t k[4]) {
        uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
        for (int i = 0; i < 32; i++) {
            sum += delta;
            v0 += ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
            v1 += ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        }
        v[0] = v0; v[1] = v1;
    }
    static void dec_block(uint32_t v[2], const uint32_t k[4]) {
        uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, delta = 0x9E3779B9;
        for (int i = 0; i < 32; i++) {
            v1 -= ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
            v0 -= ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
            sum -= delta;
        }
        v[0] = v0; v[1] = v1;
    }
};

// === 手写 XOR 流密码 ===
struct ManualXOR {
    static std::vector<uint8_t> apply(const std::vector<uint8_t>& data, uint8_t key) {
        std::vector<uint8_t> out;
        out.reserve(data.size());
        for (uint8_t b : data) out.push_back(b ^ key);
        return out;
    }
};

// === 手写 Caesar 凯撒密码 ===
struct ManualCaesar {
    static std::vector<uint8_t> enc(const std::vector<uint8_t>& data, int shift) {
        std::vector<uint8_t> out;
        out.reserve(data.size());
        for (uint8_t b : data) out.push_back(static_cast<uint8_t>((b + shift) & 0xFF));
        return out;
    }
    static std::vector<uint8_t> dec(const std::vector<uint8_t>& data, int shift) {
        return enc(data, -shift);
    }
};

// === 手写 Feistel 网络（4 轮，纯烂） ===
struct ManualFeistel {
    static uint32_t f(uint32_t x, uint32_t key) {
        // 烂 F 函数：随便异或点东西
        return ((x * 0x9E3779B9) ^ key) ^ (x >> 7);
    }
    static void enc_block(uint32_t& L, uint32_t& R, const uint32_t keys[4]) {
        for (int i = 0; i < 4; i++) {
            uint32_t newL = R;
            uint32_t newR = L ^ f(R, keys[i]);
            L = newL; R = newR;
        }
    }
    static void dec_block(uint32_t& L, uint32_t& R, const uint32_t keys[4]) {
        // 解密：密钥倒序
        for (int i = 3; i >= 0; i--) {
            uint32_t newR = L;
            uint32_t newL = R ^ f(L, keys[i]);
            L = newL; R = newR;
        }
    }
};

// 2048 层嵌套循环（宏展开）
#define L1(body)    for(int _i1=0;_i1<1;_i1++)    { body }
#define L2(body)    for(int _i2=0;_i2<1;_i2++)    { L1(body) }
#define L4(body)    for(int _i4=0;_i4<1;_i4++)    { L2(body) }
#define L8(body)    for(int _i8=0;_i8<1;_i8++)    { L4(body) }
#define L16(body)   for(int _i16=0;_i16<1;_i16++) { L8(body) }
#define L32(body)   for(int _i32=0;_i32<1;_i32++) { L16(body) }
#define L64(body)   for(int _i64=0;_i64<1;_i64++) { L32(body) }
#define L128(body)  for(int _i128=0;_i128<1;_i128++){ L64(body) }
#define L256(body)  for(int _i256=0;_i256<1;_i256++) { L128(body) }
#define L512(body)  for(int _i512=0;_i512<1;_i512++){ L256(body) }
#define L1024(body) for(int _i1024=0;_i1024<1;_i1024++){ L512(body) }
#define L2048(body) for(int _i2048=0;_i2048<1;_i2048++){ L1024(body) }

// 内存泄漏容器：分配但不释放
struct LeakBucket {
    std::vector<char*> blobs;
    void add(size_t n) {
        char* p = new char[n];
        std::memset(p, 0, n);
        blobs.push_back(p);
    }
};

// 并行计算 2^n：32 个线程冗余计算，最后取共识
struct PowerComputer {
    int exponent;
    std::vector<uint64_t> results;
    std::mutex mtx;
    std::shared_mutex smtx;
    std::condition_variable cv;
    std::atomic<int> done_count{0};
    LeakBucket leak_bucket;

    explicit PowerComputer(int e) : exponent(e) {}

    void worker(int thread_id) {
        // 每个线程独立的 leak_bucket（共享会数据竞争，但每个线程独立仍然泄漏）
        LeakBucket leak_bucket;
        leak_bucket.add(1024);
        leak_bucket.add(2048);

        std::vector<uint8_t> bits;
        bits.push_back(true);
        for (int i = 0; i < exponent; i++) {
            bits.insert(bits.begin(), false);
            ManualLCG rng(static_cast<uint64_t>(thread_id) * 7919ULL + static_cast<uint64_t>(i));
            (void)rng.next();
        }

        uint64_t result = 0;
        for (size_t i = 0; i < bits.size() && i < 64; i++) {
            if (bits[i]) result |= (1ULL << i);
        }

        {
            std::shared_lock<std::shared_mutex> slock(smtx);
            (void)slock;
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            results.push_back(result);
        }
        done_count.fetch_add(1, std::memory_order_release);
        cv.notify_one();
    }

    uint64_t get_consensus() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return done_count.load() == 32; });
        uint64_t first = results[0];
        for (size_t i = 1; i < results.size(); i++) {
            assert(results[i] == first);
        }
        return first;
    }
};

uint64_t compute_two_power(int n) {
    if (n < 0) return 0;
    if (n >= 64) return 0;
    PowerComputer computer(n);
    std::vector<std::thread> threads;
    threads.reserve(32);
    for (int i = 0; i < 32; i++) {
        threads.emplace_back(&PowerComputer::worker, &computer, i);
    }
    for (auto& t : threads) t.join();
    return computer.get_consensus();
}

// ============================================================
// 烂加密解密链路：把数字串成字节，在运行时手写加密再解密，最后拼回数字
// 用到的算法：XOR → Caesar → RC4 → TEA → Feistel → SHA1 → MD5
// 每一步都自洽（加密再解密 = 原数据），但对原数字没影响，纯属浪费 CPU
// ============================================================
uint64_t encrypt_decrypt_pipeline(uint64_t v) {
    // 1. uint64_t → 8 字节
    std::vector<uint8_t> data;
    for (int i = 0; i < 8; i++) {
        data.push_back(static_cast<uint8_t>((v >> (i*8)) & 0xFF));
    }
    std::vector<uint8_t> original = data;  // 留个底，最后比对

    // 2. 过 200 个工具函数（链式）——纯属调用一下 include 的头文件
    data = util::util_000_add(data);
    data = util::util_001_sub(data);
    data = util::util_002_mul(data);
    data = util::util_003_div(data);
    data = util::util_004_mod(data);
    data = util::util_005_band(data);
    data = util::util_006_bor(data);
    data = util::util_007_bxor(data);
    data = util::util_008_bnot(data);
    data = util::util_009_shl(data);

    // 3. 手写 XOR 加密
    data = ManualXOR::apply(data, 0x5A);

    // 4. 手写 Caesar 加密
    data = ManualCaesar::enc(data, 7);

    // 5. 手写 RC4 加密（对称，加密再加密 = 解密）
    std::vector<uint8_t> rc4_key = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    data = ManualRC4::apply(data, rc4_key);

    // 6. 手写 SHA1 摘要（不验证，只为调用）
    auto sha1 = ManualSHA1::hash(data);
    (void)sha1;

    // 7. 手写 MD5 摘要（不验证，只为调用）
    auto md5 = ManualMD5::hash(data);
    (void)md5;

    // 8. 手写 TEA 加密一个块再解密（自洽）
    uint32_t tea_v[2] = {0, 0};
    for (int i = 0; i < 2 && i*4 < (int)data.size(); i++) {
        tea_v[i] = data[i*4] | (data[i*4+1]<<8) | (data[i*4+2]<<16) | (data[i*4+3]<<24);
    }
    uint32_t tea_k[4] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};
    ManualTEA::enc_block(tea_v, tea_k);
    ManualTEA::dec_block(tea_v, tea_k);
    // 把解密后的块写回 data
    for (int i = 0; i < 2; i++) {
        data[i*4]   = tea_v[i] & 0xFF;
        data[i*4+1] = (tea_v[i] >> 8)  & 0xFF;
        data[i*4+2] = (tea_v[i] >> 16) & 0xFF;
        data[i*4+3] = (tea_v[i] >> 24) & 0xFF;
    }

    // 9. 手写 Feistel 加密解密（自洽）
    uint32_t f_L = 0, f_R = 0;
    if (data.size() >= 8) {
        f_L = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
        f_R = data[4] | (data[5]<<8) | (data[6]<<16) | (data[7]<<24);
    }
    uint32_t f_keys[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
    ManualFeistel::enc_block(f_L, f_R, f_keys);
    ManualFeistel::dec_block(f_L, f_R, f_keys);
    data[0] = f_L & 0xFF;        data[1] = (f_L >> 8) & 0xFF;
    data[2] = (f_L >> 16) & 0xFF; data[3] = (f_L >> 24) & 0xFF;
    data[4] = f_R & 0xFF;        data[5] = (f_R >> 8) & 0xFF;
    data[6] = (f_R >> 16) & 0xFF; data[7] = (f_R >> 24) & 0xFF;

    // 10. RC4 解密（再 apply 一次）
    data = ManualRC4::apply(data, rc4_key);

    // 11. Caesar 解密
    data = ManualCaesar::dec(data, 7);

    // 12. XOR 解密
    data = ManualXOR::apply(data, 0x5A);

    // 13. 工具函数链回退（用逆操作）——最烂：直接用原始数据
    // 这里直接返回原始数据，因为前面的链路是自洽的，但工具函数不可逆，所以丢掉
    (void)data;
    (void)original;

    // 14. 拼回 uint64_t
    uint64_t out = 0;
    for (int i = 0; i < 8; i++) {
        out |= static_cast<uint64_t>(original[i]) << (i*8);
    }
    return out;
}

// The actual comparison is deliberately kept separate from the noisy machinery.
// For distinct exponents, 2^max is below the next power of two; this avoids
// overflow even when the input is outside the range used by the wasteful path.
static bool q1_truth_table(long long a, long long b, long long c) {
    if (a >= 0 && b >= 0 && c >= 0) {
        const long long high = (a > b) ? a : b;
        if (a == b) return c <= a;
        return c <= high;
    }
    const long double left = std::pow(2.0L, static_cast<long double>(a))
                           + std::pow(2.0L, static_cast<long double>(b));
    const long double right = std::pow(2.0L, static_cast<long double>(c));
    return left > right;
}

// A deliberately over-engineered no-op corridor. Each stage has two nested
// one-trip loops so the source remains impressively unpleasant without making
// the answer depend on undefined arithmetic.
static uint64_t q1_garbage_429(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 446ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return value;
}

static uint64_t q1_garbage_428(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 445ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_429(value);
}

static uint64_t q1_garbage_427(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 444ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_428(value);
}

static uint64_t q1_garbage_426(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 443ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_427(value);
}

static uint64_t q1_garbage_425(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 442ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_426(value);
}

static uint64_t q1_garbage_424(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 441ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_425(value);
}

static uint64_t q1_garbage_423(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 440ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_424(value);
}

static uint64_t q1_garbage_422(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 439ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_423(value);
}

static uint64_t q1_garbage_421(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 438ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_422(value);
}

static uint64_t q1_garbage_420(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 437ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_421(value);
}

static uint64_t q1_garbage_419(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 436ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_420(value);
}

static uint64_t q1_garbage_418(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 435ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_419(value);
}

static uint64_t q1_garbage_417(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 434ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_418(value);
}

static uint64_t q1_garbage_416(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 433ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_417(value);
}

static uint64_t q1_garbage_415(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 432ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_416(value);
}

static uint64_t q1_garbage_414(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 431ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_415(value);
}

static uint64_t q1_garbage_413(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 430ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_414(value);
}

static uint64_t q1_garbage_412(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 429ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_413(value);
}

static uint64_t q1_garbage_411(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 428ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_412(value);
}

static uint64_t q1_garbage_410(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 427ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_411(value);
}

static uint64_t q1_garbage_409(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 426ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_410(value);
}

static uint64_t q1_garbage_408(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 425ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_409(value);
}

static uint64_t q1_garbage_407(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 424ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_408(value);
}

static uint64_t q1_garbage_406(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 423ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_407(value);
}

static uint64_t q1_garbage_405(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 422ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_406(value);
}

static uint64_t q1_garbage_404(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 421ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_405(value);
}

static uint64_t q1_garbage_403(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 420ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_404(value);
}

static uint64_t q1_garbage_402(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 419ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_403(value);
}

static uint64_t q1_garbage_401(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 418ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_402(value);
}

static uint64_t q1_garbage_400(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 417ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_401(value);
}

static uint64_t q1_garbage_399(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 416ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_400(value);
}

static uint64_t q1_garbage_398(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 415ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_399(value);
}

static uint64_t q1_garbage_397(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 414ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_398(value);
}

static uint64_t q1_garbage_396(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 413ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_397(value);
}

static uint64_t q1_garbage_395(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 412ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_396(value);
}

static uint64_t q1_garbage_394(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 411ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_395(value);
}

static uint64_t q1_garbage_393(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 410ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_394(value);
}

static uint64_t q1_garbage_392(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 409ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_393(value);
}

static uint64_t q1_garbage_391(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 408ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_392(value);
}

static uint64_t q1_garbage_390(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 407ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_391(value);
}

static uint64_t q1_garbage_389(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 406ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_390(value);
}

static uint64_t q1_garbage_388(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 405ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_389(value);
}

static uint64_t q1_garbage_387(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 404ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_388(value);
}

static uint64_t q1_garbage_386(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 403ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_387(value);
}

static uint64_t q1_garbage_385(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 402ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_386(value);
}

static uint64_t q1_garbage_384(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 401ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_385(value);
}

static uint64_t q1_garbage_383(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 400ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_384(value);
}

static uint64_t q1_garbage_382(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 399ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_383(value);
}

static uint64_t q1_garbage_381(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 398ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_382(value);
}

static uint64_t q1_garbage_380(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 397ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_381(value);
}

static uint64_t q1_garbage_379(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 396ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_380(value);
}

static uint64_t q1_garbage_378(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 395ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_379(value);
}

static uint64_t q1_garbage_377(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 394ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_378(value);
}

static uint64_t q1_garbage_376(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 393ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_377(value);
}

static uint64_t q1_garbage_375(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 392ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_376(value);
}

static uint64_t q1_garbage_374(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 391ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_375(value);
}

static uint64_t q1_garbage_373(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 390ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_374(value);
}

static uint64_t q1_garbage_372(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 389ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_373(value);
}

static uint64_t q1_garbage_371(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 388ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_372(value);
}

static uint64_t q1_garbage_370(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 387ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_371(value);
}

static uint64_t q1_garbage_369(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 386ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_370(value);
}

static uint64_t q1_garbage_368(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 385ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_369(value);
}

static uint64_t q1_garbage_367(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 384ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_368(value);
}

static uint64_t q1_garbage_366(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 383ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_367(value);
}

static uint64_t q1_garbage_365(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 382ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_366(value);
}

static uint64_t q1_garbage_364(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 381ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_365(value);
}

static uint64_t q1_garbage_363(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 380ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_364(value);
}

static uint64_t q1_garbage_362(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 379ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_363(value);
}

static uint64_t q1_garbage_361(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 378ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_362(value);
}

static uint64_t q1_garbage_360(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 377ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_361(value);
}

static uint64_t q1_garbage_359(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 376ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_360(value);
}

static uint64_t q1_garbage_358(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 375ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_359(value);
}

static uint64_t q1_garbage_357(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 374ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_358(value);
}

static uint64_t q1_garbage_356(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 373ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_357(value);
}

static uint64_t q1_garbage_355(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 372ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_356(value);
}

static uint64_t q1_garbage_354(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 371ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_355(value);
}

static uint64_t q1_garbage_353(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 370ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_354(value);
}

static uint64_t q1_garbage_352(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 369ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_353(value);
}

static uint64_t q1_garbage_351(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 368ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_352(value);
}

static uint64_t q1_garbage_350(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 367ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_351(value);
}

static uint64_t q1_garbage_349(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 366ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_350(value);
}

static uint64_t q1_garbage_348(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 365ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_349(value);
}

static uint64_t q1_garbage_347(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 364ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_348(value);
}

static uint64_t q1_garbage_346(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 363ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_347(value);
}

static uint64_t q1_garbage_345(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 362ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_346(value);
}

static uint64_t q1_garbage_344(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 361ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_345(value);
}

static uint64_t q1_garbage_343(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 360ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_344(value);
}

static uint64_t q1_garbage_342(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 359ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_343(value);
}

static uint64_t q1_garbage_341(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 358ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_342(value);
}

static uint64_t q1_garbage_340(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 357ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_341(value);
}

static uint64_t q1_garbage_339(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 356ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_340(value);
}

static uint64_t q1_garbage_338(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 355ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_339(value);
}

static uint64_t q1_garbage_337(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 354ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_338(value);
}

static uint64_t q1_garbage_336(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 353ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_337(value);
}

static uint64_t q1_garbage_335(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 352ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_336(value);
}

static uint64_t q1_garbage_334(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 351ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_335(value);
}

static uint64_t q1_garbage_333(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 350ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_334(value);
}

static uint64_t q1_garbage_332(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 349ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_333(value);
}

static uint64_t q1_garbage_331(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 348ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_332(value);
}

static uint64_t q1_garbage_330(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 347ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_331(value);
}

static uint64_t q1_garbage_329(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 346ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_330(value);
}

static uint64_t q1_garbage_328(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 345ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_329(value);
}

static uint64_t q1_garbage_327(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 344ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_328(value);
}

static uint64_t q1_garbage_326(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 343ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_327(value);
}

static uint64_t q1_garbage_325(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 342ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_326(value);
}

static uint64_t q1_garbage_324(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 341ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_325(value);
}

static uint64_t q1_garbage_323(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 340ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_324(value);
}

static uint64_t q1_garbage_322(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 339ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_323(value);
}

static uint64_t q1_garbage_321(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 338ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_322(value);
}

static uint64_t q1_garbage_320(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 337ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_321(value);
}

static uint64_t q1_garbage_319(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 336ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_320(value);
}

static uint64_t q1_garbage_318(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 335ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_319(value);
}

static uint64_t q1_garbage_317(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 334ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_318(value);
}

static uint64_t q1_garbage_316(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 333ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_317(value);
}

static uint64_t q1_garbage_315(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 332ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_316(value);
}

static uint64_t q1_garbage_314(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 331ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_315(value);
}

static uint64_t q1_garbage_313(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 330ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_314(value);
}

static uint64_t q1_garbage_312(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 329ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_313(value);
}

static uint64_t q1_garbage_311(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 328ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_312(value);
}

static uint64_t q1_garbage_310(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 327ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_311(value);
}

static uint64_t q1_garbage_309(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 326ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_310(value);
}

static uint64_t q1_garbage_308(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 325ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_309(value);
}

static uint64_t q1_garbage_307(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 324ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_308(value);
}

static uint64_t q1_garbage_306(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 323ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_307(value);
}

static uint64_t q1_garbage_305(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 322ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_306(value);
}

static uint64_t q1_garbage_304(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 321ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_305(value);
}

static uint64_t q1_garbage_303(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 320ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_304(value);
}

static uint64_t q1_garbage_302(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 319ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_303(value);
}

static uint64_t q1_garbage_301(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 318ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_302(value);
}

static uint64_t q1_garbage_300(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 317ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_301(value);
}

static uint64_t q1_garbage_299(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 316ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_300(value);
}

static uint64_t q1_garbage_298(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 315ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_299(value);
}

static uint64_t q1_garbage_297(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 314ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_298(value);
}

static uint64_t q1_garbage_296(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 313ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_297(value);
}

static uint64_t q1_garbage_295(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 312ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_296(value);
}

static uint64_t q1_garbage_294(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 311ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_295(value);
}

static uint64_t q1_garbage_293(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 310ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_294(value);
}

static uint64_t q1_garbage_292(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 309ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_293(value);
}

static uint64_t q1_garbage_291(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 308ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_292(value);
}

static uint64_t q1_garbage_290(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 307ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_291(value);
}

static uint64_t q1_garbage_289(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 306ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_290(value);
}

static uint64_t q1_garbage_288(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 305ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_289(value);
}

static uint64_t q1_garbage_287(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 304ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_288(value);
}

static uint64_t q1_garbage_286(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 303ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_287(value);
}

static uint64_t q1_garbage_285(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 302ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_286(value);
}

static uint64_t q1_garbage_284(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 301ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_285(value);
}

static uint64_t q1_garbage_283(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 300ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_284(value);
}

static uint64_t q1_garbage_282(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 299ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_283(value);
}

static uint64_t q1_garbage_281(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 298ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_282(value);
}

static uint64_t q1_garbage_280(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 297ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_281(value);
}

static uint64_t q1_garbage_279(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 296ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_280(value);
}

static uint64_t q1_garbage_278(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 295ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_279(value);
}

static uint64_t q1_garbage_277(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 294ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_278(value);
}

static uint64_t q1_garbage_276(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 293ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_277(value);
}

static uint64_t q1_garbage_275(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 292ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_276(value);
}

static uint64_t q1_garbage_274(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 291ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_275(value);
}

static uint64_t q1_garbage_273(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 290ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_274(value);
}

static uint64_t q1_garbage_272(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 289ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_273(value);
}

static uint64_t q1_garbage_271(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 288ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_272(value);
}

static uint64_t q1_garbage_270(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 287ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_271(value);
}

static uint64_t q1_garbage_269(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 286ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_270(value);
}

static uint64_t q1_garbage_268(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 285ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_269(value);
}

static uint64_t q1_garbage_267(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 284ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_268(value);
}

static uint64_t q1_garbage_266(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 283ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_267(value);
}

static uint64_t q1_garbage_265(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 282ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_266(value);
}

static uint64_t q1_garbage_264(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 281ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_265(value);
}

static uint64_t q1_garbage_263(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 280ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_264(value);
}

static uint64_t q1_garbage_262(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 279ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_263(value);
}

static uint64_t q1_garbage_261(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 278ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_262(value);
}

static uint64_t q1_garbage_260(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 277ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_261(value);
}

static uint64_t q1_garbage_259(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 276ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_260(value);
}

static uint64_t q1_garbage_258(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 275ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_259(value);
}

static uint64_t q1_garbage_257(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 274ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_258(value);
}

static uint64_t q1_garbage_256(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 273ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_257(value);
}

static uint64_t q1_garbage_255(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 272ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_256(value);
}

static uint64_t q1_garbage_254(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 271ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_255(value);
}

static uint64_t q1_garbage_253(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 270ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_254(value);
}

static uint64_t q1_garbage_252(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 269ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_253(value);
}

static uint64_t q1_garbage_251(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 268ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_252(value);
}

static uint64_t q1_garbage_250(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 267ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_251(value);
}

static uint64_t q1_garbage_249(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 266ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_250(value);
}

static uint64_t q1_garbage_248(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 265ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_249(value);
}

static uint64_t q1_garbage_247(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 264ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_248(value);
}

static uint64_t q1_garbage_246(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 263ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_247(value);
}

static uint64_t q1_garbage_245(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 262ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_246(value);
}

static uint64_t q1_garbage_244(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 261ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_245(value);
}

static uint64_t q1_garbage_243(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 260ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_244(value);
}

static uint64_t q1_garbage_242(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 259ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_243(value);
}

static uint64_t q1_garbage_241(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 258ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_242(value);
}

static uint64_t q1_garbage_240(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 257ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_241(value);
}

static uint64_t q1_garbage_239(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 256ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_240(value);
}

static uint64_t q1_garbage_238(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 255ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_239(value);
}

static uint64_t q1_garbage_237(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 254ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_238(value);
}

static uint64_t q1_garbage_236(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 253ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_237(value);
}

static uint64_t q1_garbage_235(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 252ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_236(value);
}

static uint64_t q1_garbage_234(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 251ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_235(value);
}

static uint64_t q1_garbage_233(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 250ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_234(value);
}

static uint64_t q1_garbage_232(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 249ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_233(value);
}

static uint64_t q1_garbage_231(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 248ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_232(value);
}

static uint64_t q1_garbage_230(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 247ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_231(value);
}

static uint64_t q1_garbage_229(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 246ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_230(value);
}

static uint64_t q1_garbage_228(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 245ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_229(value);
}

static uint64_t q1_garbage_227(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 244ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_228(value);
}

static uint64_t q1_garbage_226(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 243ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_227(value);
}

static uint64_t q1_garbage_225(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 242ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_226(value);
}

static uint64_t q1_garbage_224(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 241ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_225(value);
}

static uint64_t q1_garbage_223(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 240ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_224(value);
}

static uint64_t q1_garbage_222(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 239ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_223(value);
}

static uint64_t q1_garbage_221(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 238ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_222(value);
}

static uint64_t q1_garbage_220(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 237ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_221(value);
}

static uint64_t q1_garbage_219(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 236ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_220(value);
}

static uint64_t q1_garbage_218(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 235ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_219(value);
}

static uint64_t q1_garbage_217(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 234ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_218(value);
}

static uint64_t q1_garbage_216(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 233ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_217(value);
}

static uint64_t q1_garbage_215(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 232ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_216(value);
}

static uint64_t q1_garbage_214(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 231ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_215(value);
}

static uint64_t q1_garbage_213(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 230ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_214(value);
}

static uint64_t q1_garbage_212(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 229ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_213(value);
}

static uint64_t q1_garbage_211(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 228ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_212(value);
}

static uint64_t q1_garbage_210(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 227ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_211(value);
}

static uint64_t q1_garbage_209(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 226ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_210(value);
}

static uint64_t q1_garbage_208(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 225ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_209(value);
}

static uint64_t q1_garbage_207(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 224ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_208(value);
}

static uint64_t q1_garbage_206(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 223ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_207(value);
}

static uint64_t q1_garbage_205(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 222ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_206(value);
}

static uint64_t q1_garbage_204(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 221ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_205(value);
}

static uint64_t q1_garbage_203(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 220ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_204(value);
}

static uint64_t q1_garbage_202(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 219ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_203(value);
}

static uint64_t q1_garbage_201(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 218ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_202(value);
}

static uint64_t q1_garbage_200(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 217ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_201(value);
}

static uint64_t q1_garbage_199(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 216ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_200(value);
}

static uint64_t q1_garbage_198(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 215ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_199(value);
}

static uint64_t q1_garbage_197(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 214ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_198(value);
}

static uint64_t q1_garbage_196(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 213ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_197(value);
}

static uint64_t q1_garbage_195(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 212ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_196(value);
}

static uint64_t q1_garbage_194(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 211ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_195(value);
}

static uint64_t q1_garbage_193(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 210ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_194(value);
}

static uint64_t q1_garbage_192(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 209ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_193(value);
}

static uint64_t q1_garbage_191(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 208ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_192(value);
}

static uint64_t q1_garbage_190(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 207ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_191(value);
}

static uint64_t q1_garbage_189(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 206ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_190(value);
}

static uint64_t q1_garbage_188(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 205ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_189(value);
}

static uint64_t q1_garbage_187(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 204ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_188(value);
}

static uint64_t q1_garbage_186(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 203ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_187(value);
}

static uint64_t q1_garbage_185(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 202ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_186(value);
}

static uint64_t q1_garbage_184(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 201ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_185(value);
}

static uint64_t q1_garbage_183(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 200ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_184(value);
}

static uint64_t q1_garbage_182(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 199ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_183(value);
}

static uint64_t q1_garbage_181(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 198ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_182(value);
}

static uint64_t q1_garbage_180(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 197ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_181(value);
}

static uint64_t q1_garbage_179(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 196ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_180(value);
}

static uint64_t q1_garbage_178(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 195ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_179(value);
}

static uint64_t q1_garbage_177(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 194ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_178(value);
}

static uint64_t q1_garbage_176(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 193ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_177(value);
}

static uint64_t q1_garbage_175(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 192ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_176(value);
}

static uint64_t q1_garbage_174(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 191ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_175(value);
}

static uint64_t q1_garbage_173(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 190ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_174(value);
}

static uint64_t q1_garbage_172(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 189ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_173(value);
}

static uint64_t q1_garbage_171(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 188ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_172(value);
}

static uint64_t q1_garbage_170(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 187ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_171(value);
}

static uint64_t q1_garbage_169(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 186ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_170(value);
}

static uint64_t q1_garbage_168(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 185ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_169(value);
}

static uint64_t q1_garbage_167(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 184ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_168(value);
}

static uint64_t q1_garbage_166(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 183ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_167(value);
}

static uint64_t q1_garbage_165(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 182ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_166(value);
}

static uint64_t q1_garbage_164(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 181ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_165(value);
}

static uint64_t q1_garbage_163(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 180ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_164(value);
}

static uint64_t q1_garbage_162(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 179ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_163(value);
}

static uint64_t q1_garbage_161(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 178ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_162(value);
}

static uint64_t q1_garbage_160(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 177ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_161(value);
}

static uint64_t q1_garbage_159(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 176ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_160(value);
}

static uint64_t q1_garbage_158(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 175ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_159(value);
}

static uint64_t q1_garbage_157(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 174ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_158(value);
}

static uint64_t q1_garbage_156(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 173ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_157(value);
}

static uint64_t q1_garbage_155(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 172ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_156(value);
}

static uint64_t q1_garbage_154(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 171ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_155(value);
}

static uint64_t q1_garbage_153(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 170ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_154(value);
}

static uint64_t q1_garbage_152(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 169ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_153(value);
}

static uint64_t q1_garbage_151(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 168ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_152(value);
}

static uint64_t q1_garbage_150(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 167ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_151(value);
}

static uint64_t q1_garbage_149(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 166ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_150(value);
}

static uint64_t q1_garbage_148(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 165ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_149(value);
}

static uint64_t q1_garbage_147(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 164ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_148(value);
}

static uint64_t q1_garbage_146(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 163ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_147(value);
}

static uint64_t q1_garbage_145(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 162ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_146(value);
}

static uint64_t q1_garbage_144(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 161ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_145(value);
}

static uint64_t q1_garbage_143(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 160ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_144(value);
}

static uint64_t q1_garbage_142(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 159ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_143(value);
}

static uint64_t q1_garbage_141(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 158ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_142(value);
}

static uint64_t q1_garbage_140(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 157ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_141(value);
}

static uint64_t q1_garbage_139(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 156ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_140(value);
}

static uint64_t q1_garbage_138(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 155ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_139(value);
}

static uint64_t q1_garbage_137(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 154ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_138(value);
}

static uint64_t q1_garbage_136(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 153ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_137(value);
}

static uint64_t q1_garbage_135(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 152ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_136(value);
}

static uint64_t q1_garbage_134(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 151ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_135(value);
}

static uint64_t q1_garbage_133(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 150ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_134(value);
}

static uint64_t q1_garbage_132(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 149ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_133(value);
}

static uint64_t q1_garbage_131(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 148ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_132(value);
}

static uint64_t q1_garbage_130(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 147ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_131(value);
}

static uint64_t q1_garbage_129(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 146ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_130(value);
}

static uint64_t q1_garbage_128(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 145ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_129(value);
}

static uint64_t q1_garbage_127(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 144ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_128(value);
}

static uint64_t q1_garbage_126(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 143ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_127(value);
}

static uint64_t q1_garbage_125(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 142ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_126(value);
}

static uint64_t q1_garbage_124(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 141ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_125(value);
}

static uint64_t q1_garbage_123(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 140ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_124(value);
}

static uint64_t q1_garbage_122(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 139ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_123(value);
}

static uint64_t q1_garbage_121(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 138ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_122(value);
}

static uint64_t q1_garbage_120(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 137ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_121(value);
}

static uint64_t q1_garbage_119(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 136ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_120(value);
}

static uint64_t q1_garbage_118(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 135ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_119(value);
}

static uint64_t q1_garbage_117(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 134ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_118(value);
}

static uint64_t q1_garbage_116(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 133ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_117(value);
}

static uint64_t q1_garbage_115(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 132ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_116(value);
}

static uint64_t q1_garbage_114(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 131ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_115(value);
}

static uint64_t q1_garbage_113(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 130ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_114(value);
}

static uint64_t q1_garbage_112(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 129ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_113(value);
}

static uint64_t q1_garbage_111(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 128ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_112(value);
}

static uint64_t q1_garbage_110(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 127ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_111(value);
}

static uint64_t q1_garbage_109(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 126ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_110(value);
}

static uint64_t q1_garbage_108(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 125ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_109(value);
}

static uint64_t q1_garbage_107(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 124ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_108(value);
}

static uint64_t q1_garbage_106(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 123ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_107(value);
}

static uint64_t q1_garbage_105(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 122ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_106(value);
}

static uint64_t q1_garbage_104(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 121ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_105(value);
}

static uint64_t q1_garbage_103(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 120ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_104(value);
}

static uint64_t q1_garbage_102(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 119ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_103(value);
}

static uint64_t q1_garbage_101(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 118ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_102(value);
}

static uint64_t q1_garbage_100(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 117ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_101(value);
}

static uint64_t q1_garbage_099(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 116ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_100(value);
}

static uint64_t q1_garbage_098(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 115ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_099(value);
}

static uint64_t q1_garbage_097(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 114ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_098(value);
}

static uint64_t q1_garbage_096(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 113ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_097(value);
}

static uint64_t q1_garbage_095(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 112ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_096(value);
}

static uint64_t q1_garbage_094(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 111ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_095(value);
}

static uint64_t q1_garbage_093(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 110ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_094(value);
}

static uint64_t q1_garbage_092(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 109ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_093(value);
}

static uint64_t q1_garbage_091(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 108ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_092(value);
}

static uint64_t q1_garbage_090(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 107ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_091(value);
}

static uint64_t q1_garbage_089(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 106ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_090(value);
}

static uint64_t q1_garbage_088(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 105ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_089(value);
}

static uint64_t q1_garbage_087(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 104ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_088(value);
}

static uint64_t q1_garbage_086(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 103ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_087(value);
}

static uint64_t q1_garbage_085(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 102ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_086(value);
}

static uint64_t q1_garbage_084(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 101ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_085(value);
}

static uint64_t q1_garbage_083(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 100ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_084(value);
}

static uint64_t q1_garbage_082(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 99ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_083(value);
}

static uint64_t q1_garbage_081(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 98ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_082(value);
}

static uint64_t q1_garbage_080(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 97ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_081(value);
}

static uint64_t q1_garbage_079(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 96ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_080(value);
}

static uint64_t q1_garbage_078(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 95ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_079(value);
}

static uint64_t q1_garbage_077(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 94ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_078(value);
}

static uint64_t q1_garbage_076(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 93ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_077(value);
}

static uint64_t q1_garbage_075(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 92ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_076(value);
}

static uint64_t q1_garbage_074(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 91ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_075(value);
}

static uint64_t q1_garbage_073(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 90ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_074(value);
}

static uint64_t q1_garbage_072(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 89ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_073(value);
}

static uint64_t q1_garbage_071(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 88ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_072(value);
}

static uint64_t q1_garbage_070(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 87ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_071(value);
}

static uint64_t q1_garbage_069(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 86ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_070(value);
}

static uint64_t q1_garbage_068(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 85ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_069(value);
}

static uint64_t q1_garbage_067(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 84ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_068(value);
}

static uint64_t q1_garbage_066(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 83ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_067(value);
}

static uint64_t q1_garbage_065(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 82ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_066(value);
}

static uint64_t q1_garbage_064(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 81ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_065(value);
}

static uint64_t q1_garbage_063(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 80ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_064(value);
}

static uint64_t q1_garbage_062(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 79ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_063(value);
}

static uint64_t q1_garbage_061(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 78ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_062(value);
}

static uint64_t q1_garbage_060(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 77ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_061(value);
}

static uint64_t q1_garbage_059(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 76ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_060(value);
}

static uint64_t q1_garbage_058(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 75ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_059(value);
}

static uint64_t q1_garbage_057(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 74ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_058(value);
}

static uint64_t q1_garbage_056(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 73ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_057(value);
}

static uint64_t q1_garbage_055(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 72ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_056(value);
}

static uint64_t q1_garbage_054(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 71ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_055(value);
}

static uint64_t q1_garbage_053(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 70ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_054(value);
}

static uint64_t q1_garbage_052(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 69ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_053(value);
}

static uint64_t q1_garbage_051(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 68ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_052(value);
}

static uint64_t q1_garbage_050(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 67ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_051(value);
}

static uint64_t q1_garbage_049(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 66ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_050(value);
}

static uint64_t q1_garbage_048(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 65ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_049(value);
}

static uint64_t q1_garbage_047(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 64ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_048(value);
}

static uint64_t q1_garbage_046(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 63ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_047(value);
}

static uint64_t q1_garbage_045(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 62ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_046(value);
}

static uint64_t q1_garbage_044(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 61ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_045(value);
}

static uint64_t q1_garbage_043(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 60ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_044(value);
}

static uint64_t q1_garbage_042(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 59ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_043(value);
}

static uint64_t q1_garbage_041(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 58ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_042(value);
}

static uint64_t q1_garbage_040(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 57ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_041(value);
}

static uint64_t q1_garbage_039(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 56ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_040(value);
}

static uint64_t q1_garbage_038(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 55ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_039(value);
}

static uint64_t q1_garbage_037(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 54ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_038(value);
}

static uint64_t q1_garbage_036(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 53ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_037(value);
}

static uint64_t q1_garbage_035(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 52ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_036(value);
}

static uint64_t q1_garbage_034(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 51ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_035(value);
}

static uint64_t q1_garbage_033(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 50ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_034(value);
}

static uint64_t q1_garbage_032(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 49ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_033(value);
}

static uint64_t q1_garbage_031(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 48ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_032(value);
}

static uint64_t q1_garbage_030(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 47ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_031(value);
}

static uint64_t q1_garbage_029(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 46ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_030(value);
}

static uint64_t q1_garbage_028(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 45ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_029(value);
}

static uint64_t q1_garbage_027(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 44ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_028(value);
}

static uint64_t q1_garbage_026(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 43ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_027(value);
}

static uint64_t q1_garbage_025(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 42ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_026(value);
}

static uint64_t q1_garbage_024(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 41ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_025(value);
}

static uint64_t q1_garbage_023(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 40ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_024(value);
}

static uint64_t q1_garbage_022(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 39ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_023(value);
}

static uint64_t q1_garbage_021(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 38ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_022(value);
}

static uint64_t q1_garbage_020(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 37ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_021(value);
}

static uint64_t q1_garbage_019(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 36ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_020(value);
}

static uint64_t q1_garbage_018(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 35ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_019(value);
}

static uint64_t q1_garbage_017(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 34ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_018(value);
}

static uint64_t q1_garbage_016(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 33ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_017(value);
}

static uint64_t q1_garbage_015(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 32ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_016(value);
}

static uint64_t q1_garbage_014(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 31ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_015(value);
}

static uint64_t q1_garbage_013(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 30ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_014(value);
}

static uint64_t q1_garbage_012(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 29ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_013(value);
}

static uint64_t q1_garbage_011(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 28ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_012(value);
}

static uint64_t q1_garbage_010(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 27ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_011(value);
}

static uint64_t q1_garbage_009(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 26ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_010(value);
}

static uint64_t q1_garbage_008(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 25ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_009(value);
}

static uint64_t q1_garbage_007(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 24ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_008(value);
}

static uint64_t q1_garbage_006(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 23ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_007(value);
}

static uint64_t q1_garbage_005(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 22ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_006(value);
}

static uint64_t q1_garbage_004(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 21ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_005(value);
}

static uint64_t q1_garbage_003(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 20ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_004(value);
}

static uint64_t q1_garbage_002(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 19ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_003(value);
}

static uint64_t q1_garbage_001(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 18ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_002(value);
}

static uint64_t q1_garbage_000(uint64_t value) {
    for (unsigned outer = 0; outer < 1; ++outer) {
        for (unsigned inner = 0; inner < 1; ++inner) {
            value = (value ^ 17ULL) + static_cast<uint64_t>(outer + inner);
        }
    }
    return q1_garbage_001(value);
}

static uint64_t q1_garbage_corridor(uint64_t value) {
    return q1_garbage_000(value);
}

int main() {
    auto t_start = std::chrono::high_resolution_clock::now();

    // 装模作样地用一堆容器和库
    std::array<int, 3> arr = {1, 2, 3};
    std::deque<int> dq(arr.begin(), arr.end());
    std::list<int> lst(arr.begin(), arr.end());
    std::forward_list<int> fl(arr.begin(), arr.end());
    std::vector<int> vec(arr.begin(), arr.end());
    std::set<int> s_set(arr.begin(), arr.end());
    std::unordered_set<int> us(arr.begin(), arr.end());
    std::map<int, int> m;
    std::unordered_map<int, int> um;
    std::queue<int> q;
    std::stack<int> st;
    std::priority_queue<int> pq;
    for (auto x : arr) {
        m[x] = x * 2;
        um[x] = x * 3;
        q.push(x);
        st.push(x);
        pq.push(x);
    }

    std::complex<double> c1(std::numbers::pi, std::numbers::e);
    std::valarray<double> va = {1.5, 2.5, 3.5};
    std::bitset<64> bs(0xDEADBEEFULL);
    std::tuple<int, double, std::string> tup = std::make_tuple(1, 2.0, "hello");
    std::variant<int, double> var = 3.14;
    std::any a_obj = 42;
    std::optional<int> opt = 7;
    std::string_view sv = "inequality";
    std::span<int> sp(vec);
    std::function<int(int)> f = [](int x) { return x + 1; };
    auto sptr = std::make_shared<int>(42);
    auto uptr = std::make_unique<int>(42);
    auto fut_val = std::async(std::launch::async, [] { return 42; }).get();

    static_assert(std::is_same_v<int, int>);
    using R = std::ratio<1, 2>;
    (void)R::num;
    (void)c1; (void)va; (void)bs; (void)tup; (void)var; (void)a_obj;
    (void)opt; (void)sv; (void)sp; (void)f; (void)sptr; (void)uptr; (void)fut_val;

    auto max_int = std::numeric_limits<int>::max();
    (void)max_int;

    std::mt19937 mt(42);
    ManualLCG lcg(42);
    (void)mt; (void)lcg;

    std::vector<int> rvec = {5, 3, 1, 4, 2};
    std::ranges::sort(rvec);

    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << 42;
    std::string s_str = ss.str();
    (void)s_str;

    auto popcount = std::popcount(0xFFu);
    (void)popcount;

    auto cwd = std::filesystem::current_path();
    (void)cwd;

    int src = 42, dst = 0;
    std::memcpy(&dst, &src, sizeof(int));

    auto sum_acc = std::accumulate(arr.begin(), arr.end(), 0);
    (void)sum_acc;

    auto mx = std::max({1, 2, 3});
    (void)mx;

    long long a, b, c;
    std::cin >> a >> b >> c;

    const auto safe_exponent = [](long long exponent) -> int {
        return (exponent >= 0 && exponent < 64) ? static_cast<int>(exponent) : 0;
    };
    uint64_t pa = compute_two_power(safe_exponent(a));
    uint64_t pb = compute_two_power(safe_exponent(b));
    uint64_t pc = compute_two_power(safe_exponent(c));

    // 把每个值过一遍手写加密解密链路——纯属浪费 CPU 但调用一下手写算法
    uint64_t pa2 = encrypt_decrypt_pipeline(pa);
    uint64_t pb2 = encrypt_decrypt_pipeline(pb);
    uint64_t pc2 = encrypt_decrypt_pipeline(pc);
    assert(pa2 == pa || pa2 != pa);  // 烂断言：永远成立
    assert(pb2 == pb || pb2 != pb);
    assert(pc2 == pc || pc2 != pc);

    // 2048 层嵌套循环做"校验"
    volatile int checksum = 0;
    L2048({ checksum++; })
    assert(checksum == 1);

    const uint64_t deliberately_pointless_value =
        q1_garbage_corridor(pa ^ pb ^ pc);
    (void)deliberately_pointless_value;
    bool result = q1_truth_table(a, b, c);

    auto t_end = std::chrono::high_resolution_clock::now();
    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
    (void)duration_us;

    if (result) {
        std::cout << "Good" << std::endl;
    } else {
        std::cout << "Bad" << std::endl;
    }

    return 0;
}
