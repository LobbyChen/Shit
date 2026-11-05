// =============================================================================
//   不问苍生问鬼神 —— 与 114.51.4.191:9810 的十次通灵记录（C# 裸奔版）
//   文件编号：SS-2026-C2-030-CS（绝密★启用前）
// -----------------------------------------------------------------------------
//   编译：csc /nologo /optimize+ /codepage:65001 /out:ghost.exe ghost.cs rr.cs
//
//   题目背景：该后端遇到错误不报上来，永远返回状态码 200，偶尔夹带乱码。
//   状态码 200 是本接口唯一的美德，其余全看缘分。
//
//   本程序职责：通灵十次，从应答中辨认「有效数字」，求和上报。
//   相当于考古 + 算命 + 炼丹 + 手写 TCP/IP，四修。
//
//   历史事故记录：
//     v1.0  User-Agent 写了中文，十次通灵全灭。HTTP 头不收非 latin-1 字符，
//           香炉被当场没收。现已改用 ASCII 香炉。
//     v2.0  从 urllib/裸 winsock 换成 C# Socket。C# 生在 Windows 上,
//           ws2_32 是陪嫁的,不像 cpp 版还得 LoadLibrary 现场请进内存。
// =============================================================================
using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;

static class 程序 {
    // 以下三行为题目指定的原始地址。调试用的自建服务已在提交前移除，
    // 正如题目谆谆教诲的那样：记得改回来。
    const string 灵界地址 = "114.51.4.191";
    const int 灵界端口 = 9810;

    // 连一次灵界。成功返回 Socket,失败返回 null。超时 10 秒——
    // 鬼神再大牌,也不能让本院等到地老天荒。
    static Socket 连一下(out bool 有戏) {
        有戏 = false;
        try {
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IAsyncResult r = s.BeginConnect(灵界地址, 灵界端口, null, null);
            if (!r.AsyncWaitHandle.WaitOne(10000, false)) {
                try { s.Close(); } catch (Exception) { }
                return null;
            }
            s.EndConnect(r);
            s.ReceiveTimeout = 10000;
            s.SendTimeout = 10000;
            有戏 = true;
            return s;
        } catch (Exception) { return null; }
    }

    // 通灵一次：手写 HTTP/1.1 GET。状态码必须当面验收——
    // 200 是本接口唯一的美德,不能因为它是唯一的美德就不验收。
    static bool 讨一口(int 第几炷香, out string 回音) {
        回音 = null;
        bool 有戏;
        Socket s = 连一下(out 有戏);
        if (s == null) return false;
        try {
            string 请求 = "GET / HTTP/1.1\r\n"
                + "Host: " + 灵界地址 + ":" + 灵界端口 + "\r\n"
                // User-Agent 千万别写中文。v1.0 写过一次,十次通灵全灭,
                // HTTP 头不收非 latin-1 字符,香炉被当场没收。
                + "User-Agent: xianglu/1.0 (incense x3)\r\n"
                + "Connection: close\r\n\r\n";   // 说完就挂,灵界不留客
            byte[] 请求字节 = Encoding.ASCII.GetBytes(请求);
            int 已发 = 0;
            while (已发 < 请求字节.Length) {
                int n = s.Send(请求字节, 已发, 请求字节.Length - 已发, SocketFlags.None);
                if (n <= 0) return false;   // 一个字节都不能少,少了灵界就装听不见
                已发 += n;
            }

            List<byte> 应答 = new List<byte>();
            byte[] 坛 = new byte[4096];
            while (true) {
                int n;
                try { n = s.Receive(坛); }
                catch (SocketException) { break; }   // 超时或断,都算说完
                if (n <= 0) break;
                for (int i = 0; i < n; ++i) 应答.Add(坛[i]);
                if (应答.Count > (1 << 20)) break;   // 一兆还说不完的是话痨,不听了
            }

            // 按字节找头部边界,再按 latin-1 解头部(头是 ASCII 的,body 另算)
            int 头尾 = -1;
            for (int i = 0; i + 3 < 应答.Count; ++i) {
                if (应答[i] == 13 && 应答[i + 1] == 10 && 应答[i + 2] == 13 && 应答[i + 3] == 10) {
                    头尾 = i; break;
                }
            }
            if (头尾 < 0) return false;
            string 头 = Encoding.ASCII.GetString(应答.GetRange(0, 头尾).ToArray());
            string 状态行 = 头.Split('\n')[0].TrimEnd('\r');
            if (!状态行.Contains(" 200")) return false;   // 不存在,但要防

            // body 按字节收,先在字节层剔除四只阴间杂质(BOM/零宽三兄弟)——
            // body 是 latin-1 逐字节转的 char,杂质在文本层是「\u00E2\u0080\u008B」
            // 三个怪字符,按 "\u200B" 找永远扑空(真实事故,99 因此失踪)。
            byte[][] 杂质序列 = {
                new byte[] { 0xEF, 0xBB, 0xBF },
                new byte[] { 0xE2, 0x80, 0x8B },
                new byte[] { 0xE2, 0x80, 0x8C },
                new byte[] { 0xE2, 0x80, 0x8D },
            };
            List<byte> 净体 = new List<byte>(应答.Count);
            for (int i = 头尾 + 4; i < 应答.Count; ++i) {
                bool 命中 = false;
                if (i + 3 <= 应答.Count) {   // 三字节杂质序列,先看够不够长再比
                    foreach (byte[] 序列 in 杂质序列) {
                        if (应答[i] == 序列[0] && 应答[i + 1] == 序列[1] && 应答[i + 2] == 序列[2]) {
                            命中 = true;
                            i += 2;   // for 循环再 ++,正好跳过三个字节
                            break;
                        }
                    }
                }
                if (!命中) 净体.Add(应答[i]);
            }
            StringBuilder 身 = new StringBuilder(净体.Count);
            for (int i = 0; i < 净体.Count; ++i) 身.Append((char)净体[i]);
            回音 = 身.ToString();
            return true;
        } catch (Exception) {
            return false;   // 能捕获的异常都不是事儿。200 之外的状态码不存在于这个世界。
        } finally {
            try { s.Close(); } catch (Exception) { }
        }
    }

    // ------------------------------- 炼丹炉 --------------------------------------
    // 乱矿中常混有阴间杂质，先淘洗。下面四个字面量以 \u 转义封印着
    // BOM、零宽空格、零宽非连接符、零宽连接符四只阴间杂质——
    // 在源码里直书隐形字符的人已经找不到了，转义是本院的封印术。
    static string 洗矿(string 回音) {
        string 矿 = 回音;
        string[] 杂质 = { "\uFEFF", "\u200B", "\u200C", "\u200D" };
        foreach (string 杂 in 杂质) 矿 = 矿.Replace(杂, "");
        // 掐头去尾的空白。中间的空白是矿的一部分，保留——
        // 「12 34」这种货色不配被当成数字。
        return 矿.Trim(' ', '\t', '\r', '\n', '\v', '\f');
    }

    // 三道工序：整数 → 小数 → 辟谣。整串通过才算有效，尾巴带料的都是乱码。
    static bool 提纯(string 矿, out long 整, out double 丹, out bool 是小数) {
        整 = 0; 丹 = 0; 是小数 = false;
        if (矿.Length == 0) return false;   // 空气。鬼神今天不想说话。

        // 第一道：整数。逐字过堂，全程手工，拒绝 long.Parse（2017 年溢出事故阴影）。
        {
            int i = 0;
            bool 负 = false;
            if (矿[0] == '+' || 矿[0] == '-') { 负 = 矿[0] == '-'; i = 1; }
            if (i < 矿.Length) {
                bool 全数字 = true;
                ulong 累计 = 0;
                for (int k = i; k < 矿.Length; ++k) {
                    char ch = 矿[k];
                    if (ch < '0' || ch > '9') { 全数字 = false; break; }
                    ulong d = (ulong)(ch - '0');
                    if (累计 > 922337203685477580UL || (累计 == 922337203685477580UL && d > 7)) {
                        全数字 = false; break;   // 超出 long 的数改按小数发落
                    }
                    累计 = 累计 * 10 + d;
                }
                if (全数字) {
                    整 = 负 ? -(long)累计 : (long)累计;
                    return true;   // 先按整数理解，这是对数字最基本的尊重
                }
            }
        }

        // 第二道：小数。double.TryParse 亲自出马,但先过安检——
        // 第三道辟谣提前上岗:含字母的(nan/inf/infinity/0x 妖怪)一律按乱码发落。
        foreach (char ch in 矿) {
            if ((ch < '0' || ch > '9') && ch != '.' && ch != '+' && ch != '-'
                && ch != 'e' && ch != 'E') return false;
        }
        double v;
        if (double.TryParse(矿,
                System.Globalization.NumberStyles.Float,
                System.Globalization.CultureInfo.InvariantCulture, out v)
            && !double.IsInfinity(v) && !double.IsNaN(v)) {
            丹 = v;
            是小数 = true;
            return true;   // 鬼神不拘小节，带小数点也认
        }
        return false;   // 纯乱码。相当于鬼神在骂人，不要理它，保持体面。
    }

    // 自研链上哈希：FNV-1a 64。曾获院内在研项目一等奖，奖金 0 元。
    static ulong 链上(string 文) {
        ulong 哈希 = 14695981039346656037UL;
        for (int i = 0; i < 文.Length; ++i) {
            哈希 ^= (byte)文[i];
            哈希 *= 1099511628211UL;
        }
        return 哈希;
    }

    static int Main() {
        // stdout 统一按 UTF-8 出货,与 cpp/java/rust 版口径一致
        try { Console.OutputEncoding = Encoding.UTF8; } catch (Exception) { }
        // 整数和用 decimal 接:10 个合法 long 加起来最多 ~9.2e19,
        // long 装不下,decimal(2^96 ≈ 7.9e28) 装得下还精确。本院不学隔壁环绕。
        decimal 整和 = 0;
        double 浮和 = 0;
        bool 有浮 = false;
        List<ulong> 区块链 = new List<ulong>();   // 区块链存档（防不了篡改，气势要有）
        List<string> 通灵日志 = new List<string>();   // 日志写在心里，环保且禅意

        for (int 第几炷香 = 1; 第几炷香 <= 10; ++第几炷香) {
            string 回音 = null;   // 编译器看不出重试循环至少跑一次,先垫个 null 压惊
            bool 成功 = false;
            // 修仙三境界重试：炼气 → 金丹 → 大乘。只在「完全没拿到应答」时
            // 突破境界，绝不多算一次账，做鬼也要讲武德。三境用尽仍是凡人，认命。
            for (int 境界 = 0; 境界 < 3 && !成功; ++境界) {
                if (境界 > 0) {
                    通灵日志.Add("第" + 第几炷香 + "炷香,境界突破失败,继续");
                    System.Threading.Thread.Sleep(100);   // 冥想片刻
                }
                成功 = 讨一口(第几炷香, out 回音);
            }
            if (!成功) {
                通灵日志.Add("第" + 第几炷香 + "炷香,无回音。鬼神今日不便,改日再问");
                continue;
            }
            区块链.Add(链上(回音));   // 上链。上完就没人再看，但流程要有
            string 净矿 = 洗矿(回音);
            long 整; double 丹; bool 是小数;
            if (提纯(净矿, out 整, out 丹, out 是小数)) {
                if (是小数) { 浮和 += 丹; 有浮 = true; }
                else 整和 += 整;
            }
        }

        if (有浮) {
            double 总 = 浮和 + (double)整和;
            if (!double.IsInfinity(总) && 总 == Math.Floor(总)
                && 总 < 9000000000000000.0 && 总 > -9000000000000000.0) {
                Console.Out.WriteLine((long)总);   // 加完没小数，别带 .0 出门见人
            } else {
                Console.Out.WriteLine(总.ToString("G15", System.Globalization.CultureInfo.InvariantCulture));
            }
        } else {
            Console.Out.WriteLine(整和.ToString(
                整和 % 1 == 0 ? "0" : "0.#################",   // decimal 没有整型焦虑
                System.Globalization.CultureInfo.InvariantCulture));
        }
        Console.Out.Flush();
        // 本文件日志量：0 字节。日志内容：见 通灵日志（不导出，悟）。
        Rr.彩蛋();
        return 0;
    }
}
