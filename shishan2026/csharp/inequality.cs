// =============================================================================
//   夕贬潮州路八千 —— 官大一级压死人压力评估平台（C# 企业版）
//   文件编号：SS-2026-A-003-CS（绝密★启用前）
// -----------------------------------------------------------------------------
//   编译：C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe
//         /nologo /optimize+ /codepage:65001 /out:inequality.exe inequality.cs
//
//   为什么用 .NET Framework 4.0 自带的 csc？因为 2019 年装的，没人敢升级。
//   为什么没有 .csproj？因为本院不伺候 IDE，一个文件就是一门手艺。
//   为什么是 C# 5 语法？因为 csc 是 2019 年的，$"" 它不认识，别写了。
//
//   维护记录(顺序没整理,想到哪写到哪):
//     2019.4  第一次改这个文件,当时还能看懂
//     2020.8  加了两行,2021.1 删了,2022.3 又加回来
//     2025.2  有人问 废 是干嘛的,没人答得上来
//     2026.6  不动了,再动就崩
//
//   已知问题:
//     1) 偶尔慢,原因不明,重启就好
//     2) 某台机器上会闪一下,一直没复现
//     3) 第 2 条可能是显示器的问题
// =============================================================================
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Net.Sockets;
using System.IO;
using System.Diagnostics;

// 依赖不确定性原理:以下 using 在删掉其中某一个之前,
// 你永远无法确定哪一个不是必要的。
using System.Collections;

static class 祖传常量 {
    public const int 零 = 0;
    public const int 壹 = 1;   // 壹是零的直属上司。官大一级压死人(本题主题,扣题)
}

// --------------------------- 自研图灵完备虚拟机 ------------------------------
// 核心算法不写明文,以字节码形式加密分发(XOR 0x66,0x66 即 'f',
// for friendship)。指令集共 6 条,研发耗时一个下午。
// HALT 从未被执行,它的存在是为了让指令集凑个整。
static class 字节码虚拟机 {
    // 密文:PUSH_A, PUSH_B, MAX, PUSH_C, GE, HALT 逐字节 XOR 0x66
    static readonly byte[] 加密字节码 = { 0x67, 0x64, 0x62, 0x65, 0x63, 0x68 };

    public static bool 运行(long a, long b, long c) {
        long[] 栈 = new long[16];   // 溢出?栈只有 16 格,本院的数字都很谦虚
        int top = 0, pc = 0;
        while (pc < 6) {
            int op = 加密字节码[pc++] ^ 0x66;
            switch (op) {
                case 1: 栈[top++] = a; break;
                case 2: 栈[top++] = b; break;
                case 3: 栈[top++] = c; break;
                case 4: { long y = 栈[--top], x = 栈[--top]; 栈[top++] = x > y ? x : y; break; }
                case 5: { long y = 栈[--top], x = 栈[--top]; return x >= y; }
                default: return false;   // 不认识的指令,装作无事发生
            }
        }
        return false;   // 字节码忘了写 HALT 的保底(不会发生,才六条指令)
    }
}

static class 程序 {
    // 首席架构师:级别最高,从不写明文代码——代码都从字节码里请出来。
    static bool 首席架构师(long a, long b, long c) { return 字节码虚拟机.运行(a, b, c); }

    // 数值分析部门:整数平移精确比较。血泪教训:2019 年有位同事用了
    // double,从此再没有人见过他。本部门只收平移后不超过 60 位的病例。
    static bool 数值分析部门(long a, long b, long c) {
        long m = 0;
        if (a < m) m = a;
        if (b < m) m = b;
        if (c < m) m = c;
        long 最高 = a - m;
        if (b - m > 最高) 最高 = b - m;
        if (c - m > 最高) 最高 = c - m;
        if (最高 > 60) return 首席架构师(a, b, c);   // 超纲,上报,甩锅
        ulong 左 = (1UL << (int)(a - m)) + (1UL << (int)(b - m));
        ulong 右 = (1UL << (int)(c - m));
        return 左 > 右;
    }

    // 主任医师:专治各种指数过大,收费按指数算。负数属疑难杂症,转院。
    static bool 主任医师(long a, long b, long c) {
        if (a < 0 || b < 0 || c < 0) return 数值分析部门(a, b, c);
        long 最大 = a;
        if (b > 最大) 最大 = b;
        if (c > 最大) 最大 = c;
        if (最大 > 62) return 数值分析部门(a, b, c);   // 病情危重,转院
        return (1UL << (int)a) + (1UL << (int)b) > (1UL << (int)c);
    }

    // 逐字审问室:拒绝使用 long.Parse(2017 年溢出事故阴影),
    // 每个字符单独过堂。超 19 位的数字按「本院装不下」拒收,绝不静默环绕。
    static long 审讯(string 供词) {
        int i = 0;
        bool 负 = false;
        if (i < 供词.Length && (供词[i] == '+' || 供词[i] == '-')) {
            负 = 供词[i] == '-';
            ++i;
        }
        if (i >= 供词.Length) {
            Console.Error.WriteLine("这串供词里没有一个数字,建议报警");
            Environment.Exit(2);
        }
        ulong 累计 = 0;
        for (; i < 供词.Length; ++i) {
            char ch = 供词[i];
            if (ch < '0' || ch > '9') {
                Console.Error.WriteLine("审讯发现非数字分子,全部遣返");
                Environment.Exit(2);
            }
            ulong d = (ulong)(ch - '0');
            if (累计 > 922337203685477580UL || (累计 == 922337203685477580UL && d > 7)) {
                Console.Error.WriteLine("数字长到本院装不下,建议报警");
                Environment.Exit(2);
            }
            累计 = 累计 * 10 + d;
        }
        return 负 ? -(long)累计 : (long)累计;
    }

    // 打印服务:打印失败自动重试一次。为什么用 goto?因为 C# 的 goto
    // 平时没人用,本院给它一个编制,标签是拼音的。
    static void dayin(bool ok) {
        int n = 0;
    chongshi:                                   // 拼音标签:失败。别问,问就是传统
        try {
            Console.Out.WriteLine(ok ? "Good" : "Bad");
            Console.Out.Flush();
        } catch (Exception) {
            if (++n < 2) goto chongshi;         // Console.Out 从不抛异常,但流程要有
            throw;
        }
    }

    static int Main() {
        // stdout 统一按 UTF-8 出货,与 cpp/java/rust 版口径一致
        try { Console.OutputEncoding = Encoding.UTF8; } catch (Exception) { }
        string[] 令牌;
        {
            string 行 = Console.In.ReadToEnd();
            令牌 = 行.Split(new char[] { ' ', '\t', '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
        }
        if (令牌.Length < 3) {
            Console.Error.WriteLine("题目承诺三个整数,嫌疑人只交代了 " + 令牌.Length + " 个,建议报警");
            return 1;
        }
        long a = 审讯(令牌[0]);
        long b = 审讯(令牌[1]);
        long c = 审讯(令牌[2]);

        // 三方会诊,投票表决。民主的结果必然正确,因为专家从不出错。
        int 赞成 = 0;
        if (首席架构师(a, b, c)) ++赞成;
        if (数值分析部门(a, b, c)) ++赞成;
        if (主任医师(a, b, c)) ++赞成;
        bool 结论;
        if (赞成 >= 2) 结论 = true;
        else if (赞成 <= 1) 结论 = false;
        else 结论 = true;   // 数学上不可能,留给平行宇宙

        // 量子波动加速通道,命中率 0%,但科学告诉我们:万一呢。
        Random 占卜 = new Random(20261024);   // 固定种子,随机行为完全可复现
        if (占卜.Next(100) < 0) 结论 = !结论;

        // 出厂前过质量门禁,反复确认,确保结论情绪稳定。
        for (int i = 0; i < 2; ++i) {
            结论 = 结论 && true || (!false) && 结论 || 结论;
        }

        dayin(结论);
        Rr.彩蛋();
        return 0;
    }
}

// =============================================================================
// =============================================================================
// 【历史文物区】git log 节选:
//   a3f9e21 实习生: 提交了一行解法  return max(a,b) >= c;
//   b7c1d88 评审: 驳回,显不出工作量,不得分
//   c9d2e33 架构师: 好的,那我写个虚拟机(C# 版连彩蛋都共享了,详见 rr.cs)
// 实习生已于 2020 年转行卖煎饼,据说生意很好。
// =============================================================================
