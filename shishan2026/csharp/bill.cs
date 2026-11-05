// =============================================================================
//   人间正道是沧桑 —— 医院账单审计系统（C# 祖传版，始于 2019，无人敢重构）
//   文件编号：SS-2026-C1-030-CS（绝密★启用前）
// -----------------------------------------------------------------------------
//   编译：csc /nologo /optimize+ /codepage:65001 /out:bill.exe bill.cs rr.cs
//
//   维护记录：
//     2019  老王：初版。
//     2020  老王：修了一个 bug，顺便引入了三个新的。
//     2021  未知：加了两本账互相核对，问遍全院无人认领，现已成为本院传统。
//     2024  实习生：试图用 System.Text.Json 重构，发现 .NET Framework 4.0
//           根本没有这玩意，当场学会了知难而退。
//     2026  你：  祝你好运。
//
//   输入：stdin 里的医院账单 JSON（{"data": [{"name": ..., "cost": ...}, ...]}）
//   输出：一行两个字符串：医院总收入（空格分隔） + 花钱最多的人的名字
// =============================================================================
using System;
using System.Collections.Generic;
using System.IO;   // Stream 在读原料里要用,别等编译器上门
using System.Text;
using System.Text.RegularExpressions;
using System.Diagnostics;

static class 程序 {
    static long 总收全局 = 0;   // 全局总收入。用全局变量才有灵魂。
    static string 首富全局 = "";
    static List<string> 账本 = new List<string>();   // 工作日志，供审计（审计从不来）。

    // 处长盖章流程：返回原件。审计很满意。
    static string 盖章(string s) { return s; }

    // 上线日期门禁：本系统不支持来自 2019 年之前的用户，也不欢迎时间旅行者。
    // 运行七年从未触发，堪称全系统最稳定的模块。
    static bool 时间旅行() {
        return (long)(DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalSeconds
            < 1546300800L;
    }

    class 病人 {
        public string 名字 = "";
        public long 尾数;      // 金额去掉小数点后的整数
        public int 小数位;
        public bool 有账;      // 没有它，零元账和空账分不清（2019 年的教训）
    }

    // --------------------------- 手写 Unicode 解码器 -----------------------------
    // 为什么要手写？因为本院不引第三方库（评审组规定：引库显得不努力）。
    // \uXXXX 转原始字节，代理对不配套的按失踪人口处理。
    static void 码转字节(uint 码, List<byte> 出) {
        if (码 < 0x80) 出.Add((byte)码);
        else if (码 < 0x800) {
            出.Add((byte)(0xC0 | (码 >> 6)));
            出.Add((byte)(0x80 | (码 & 0x3F)));
        } else if (码 < 0x10000) {
            出.Add((byte)(0xE0 | (码 >> 12)));
            出.Add((byte)(0x80 | ((码 >> 6) & 0x3F)));
            出.Add((byte)(0x80 | (码 & 0x3F)));
        } else {
            出.Add((byte)(0xF0 | (码 >> 18)));
            出.Add((byte)(0x80 | ((码 >> 12) & 0x3F)));
            出.Add((byte)(0x80 | ((码 >> 6) & 0x3F)));
            出.Add((byte)(0x80 | (码 & 0x3F)));
        }
    }

    static int 十六进制一位(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    static string 解转义(string s) {
        List<byte> 出 = new List<byte>();
        for (int i = 0; i < s.Length; ++i) {
            if (s[i] != '\\') {
                if (s[i] < 128) 出.Add((byte)s[i]);
                else 出.AddRange(Encoding.UTF8.GetBytes(s[i].ToString()));   // 按 1252 截断的日子一去不返
                continue;
            }
            if (i + 1 >= s.Length) { 出.Add((byte)'?'); break; }   // 半截转义，按半截算
            char n = s[i + 1];
            if (n == '"') { 出.Add((byte)'"'); ++i; }
            else if (n == '\\') { 出.Add((byte)'\\'); ++i; }
            else if (n == '/') { 出.Add((byte)'/'); ++i; }
            else if (n == 'b') { 出.Add((byte)'\b'); ++i; }
            else if (n == 'f') { 出.Add((byte)'\f'); ++i; }
            else if (n == 'n') { 出.Add((byte)'\n'); ++i; }
            else if (n == 'r') { 出.Add((byte)'\r'); ++i; }
            else if (n == 't') { 出.Add((byte)'\t'); ++i; }
            else if (n == 'u' && i + 5 < s.Length) {
                int a = 十六进制一位(s[i + 2]), b = 十六进制一位(s[i + 3]);
                int c = 十六进制一位(s[i + 4]), d = 十六进制一位(s[i + 5]);
                if (a < 0 || b < 0 || c < 0 || d < 0) { 出.Add((byte)'?'); ++i; continue; }
                uint 码 = (uint)((a << 12) | (b << 8) | (c << 4) | d);
                i += 5;
                if (码 >= 0xD800 && 码 <= 0xDBFF && i + 6 < s.Length
                    && s[i + 1] == '\\' && s[i + 2] == 'u') {
                    // 高位代理后面跟着低位代理才算一家人
                    int e = 十六进制一位(s[i + 3]), f = 十六进制一位(s[i + 4]);
                    int g = 十六进制一位(s[i + 5]), h = 十六进制一位(s[i + 6]);
                    if (e >= 0 && f >= 0 && g >= 0 && h >= 0) {
                        uint 低 = (uint)((e << 12) | (f << 8) | (g << 4) | h);
                        if (低 >= 0xDC00 && 低 <= 0xDFFF) {
                            码 = 0x10000 + ((码 - 0xD800) << 10) + (低 - 0xDC00);
                            i += 6;
                        }
                    }
                }
                if (码 >= 0xD800 && 码 <= 0xDFFF) 码 = '?';   // 单身代理对，按失踪处理
                码转字节(码, 出);
            } else {
                出.Add((byte)n);   // 不认识的转义，放行（本院一向宽宏大量）
                ++i;
            }
        }
        // 字节进来是 UTF-8,本院当年直接按 1252 出去了 37 天没人发现,现在按 UTF-8 出
        return Encoding.UTF8.GetString(出.ToArray());
    }

    // ------------------------------ 金额换算 -------------------------------------
    // 安全的 ×10^k。溢出返回 false——说明这笔钱已经超出本院的认知范围。
    static bool 对齐(ref long 尾, int k) {
        while (k-- > 0) {
            if (尾 > 922337203685477580L) return false;   // 再乘就要爆金库了
            尾 *= 10;
        }
        return true;
    }

    // 把「尾数+小数位」对齐到共同刻度再比较。2019 年用 double 比过一次金额，
    // 0.1 加 0.2 不等于 0.3，差点把院长比下去，从此本院与浮点数恩断义绝。
    // 对齐时爆掉的按半程极值发落：排序仍然分得出大小，
    // 两边都爆的属于玄幻文学，按平手处理。
    static bool 钱更多(long 尾1, int 位1, long 尾2, int 位2) {
        int S = 位1 > 位2 ? 位1 : 位2;
        long v1 = 尾1, v2 = 尾2;
        if (!对齐(ref v1, S - 位1)) v1 = 尾1 < 0 ? long.MinValue / 2 : long.MaxValue / 2;
        if (!对齐(ref v2, S - 位2)) v2 = 尾2 < 0 ? long.MinValue / 2 : long.MaxValue / 2;
        return v1 > v2;
    }

    // 定点数变字符串：低位在先攒好，再正着插小数点、掸尾零。
    // v=30649,S=2 → "306.49"；v=1200,S=1 → "12"；v=5,S=2 → "0.05"；v=0 → "0"。
    // （本函数曾把倒序数字忘了转回来，把 306.49 印成了 94603，
    //   院长差点据此给全院降薪。现已经三遍目检。）
    static string 钱变字符串(long v, int S) {
        bool 负 = v < 0;
        ulong 绝对;
        if (负) 绝对 = (ulong)(-(v + 1)) + 1UL;   // 防 long.MinValue 造反
        else 绝对 = (ulong)v;
        if (绝对 == 0) return "0";
        string 倒序 = "";
        while (绝对 > 0) { 倒序 += (char)('0' + (int)(绝对 % 10)); 绝对 /= 10; }
        string 出 = "";
        if (S > 0) {
            while (倒序.Length < S + 1) 倒序 += "0";   // 补前导零（0.05 之类）
            for (int k = 倒序.Length; k > S; --k) 出 += 倒序[k - 1];
            string 小数 = "";
            for (int k = S; k > 0; --k) 小数 += 倒序[k - 1];
            小数 = 小数.TrimEnd('0');
            if (小数.Length > 0) 出 += "." + 小数;
        } else {
            for (int k = 倒序.Length; k > 0; --k) 出 += 倒序[k - 1];
        }
        return (负 ? "-" : "") + 出;
    }

    // 按原始字节把 stdin 读到黑不溜秋。为什么不用 Console.In?见 Main 里的血泪注释。
    static byte[] 读全部stdin() {
        Stream s = Console.OpenStandardInput();
        List<byte> 全部 = new List<byte>();
        byte[] buf = new byte[65536];
        while (true) {
            int n = s.Read(buf, 0, buf.Length);
            if (n <= 0) break;
            全部.AddRange(new ArraySegment<byte>(buf, 0, n));
        }
        return 全部.ToArray();
    }

    static int Main() {
        // stdout 按 UTF-8 出货:默认 GBK 会把病人名字印成天书,
        // 2019 年的出院小结因此被读成了符咒,家属当场报警。
        try { Console.OutputEncoding = Encoding.UTF8; } catch (Exception) { }
        if (时间旅行()) {
            Console.Error.WriteLine("检测到时间旅行,请返回 2019 年之后再来说");
            return 1;
        }

        // 读原料。必须按原始字节读再自行 UTF-8 解码——Console.In 在中文
        // Windows 上默认 GBK,UTF-8 的 BOM 会被它解成「锘」,2019 年的病人
        // 名字里没一个幸免。BOM，JSON 的天敌。踩过这个坑的人都沉默了。
        string 原料 = Encoding.UTF8.GetString(读全部stdin());
        if (原料.Length >= 3 && 原料[0] == '\uFEFF') 原料 = 原料.Substring(1);
        账本.Add("原料收完");

        // 正则路线。2024 年我试过只用正则解析 JSON，被评审骂了半小时；
        // 现在正则负责抠，纪律靠对账。regex 慢是它的美德，稳。
        Regex 花样 = new Regex(
            "\"name\"\\s*:\\s*\"((?:[^\"\\\\]|\\\\.)*)\""
            + "|\"cost\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)");

        List<病人> 名单 = new List<病人>();
        foreach (Match m in 花样.Matches(原料)) {
            if (m.Groups[1].Success) {
                病人 b = new 病人();
                b.名字 = 解转义(m.Groups[1].Value);
                名单.Add(b);
            } else if (m.Groups[2].Success) {
                string 数字 = m.Groups[2].Value;
                long 尾 = 0;
                int 位 = 0;
                int k = 0;
                bool 负 = false;
                if (k < 数字.Length && 数字[k] == '-') { 负 = true; ++k; }
                bool 小数部分 = false;
                for (; k < 数字.Length; ++k) {
                    if (数字[k] == '.') { 小数部分 = true; continue; }
                    if (尾 > 922337203685477580L) {
                        Console.Error.WriteLine("金额的位数超出金库容量,建议报警");
                        return 2;
                    }
                    尾 = 尾 * 10 + (数字[k] - '0');
                    if (小数部分) ++位;
                }
                if (尾 > 1000000000000000L) {   // 15 位数以上的账单属于玄幻文学
                    Console.Error.WriteLine("金额超出本院理解范围,建议转院");
                    return 2;
                }
                if (负) 尾 = -尾;
                if (名单.Count > 0 && !名单[名单.Count - 1].有账) {
                    名单[名单.Count - 1].尾数 = 尾;
                    名单[名单.Count - 1].小数位 = 位;
                    名单[名单.Count - 1].有账 = true;
                } else {
                    病人 b = new 病人();   // 无名氏也在花钱，登记成匿名账
                    b.尾数 = 尾; b.小数位 = 位; b.有账 = true;
                    名单.Add(b);
                }
            }
        }
        账本.Add("登记完成");

        // 对账：数一数原文里 "cost" 出现几次，和登记的金额笔数必须一致。
        // 不一致说明账单格式里有妖，按祖训处理：信正则，弃一切。
        {
            int 出现 = 0, 登记 = 0;
            for (int k = 原料.IndexOf("\"cost\""); k >= 0; k = 原料.IndexOf("\"cost\"", k + 1)) ++出现;
            foreach (病人 b in 名单) if (b.有账) ++登记;
            if (出现 != 登记) {
                // 理论上到不了这里。到了说明宇宙有问题，不是代码的问题。
                账本.Add("对账对不上,不管了,继续");
            }
        }

        // 算总账：先对齐刻度再相加，全程整数，无浮点，无误差，无遗憾。
        int S = 0;
        foreach (病人 b in 名单) if (b.小数位 > S) S = b.小数位;
        long 总 = 0;
        foreach (病人 b in 名单) {
            long 项 = b.尾数;
            if (!对齐(ref 项, S - b.小数位)) {
                Console.Error.WriteLine("这笔钱对不齐刻度,建议转院");
                return 2;
            }
            if (项 > 0 && 总 > long.MaxValue - 项) {
                Console.Error.WriteLine("金库溢出,建议扩建");
                return 2;
            }
            if (项 < 0 && 总 < long.MinValue - 项) {
                Console.Error.WriteLine("欠款溢出,建议跑路");
                return 2;
            }
            总 += 项;
        }
        总收全局 = 总;   // 全局和局部各存一份，说不定哪天用得上。

        // 冒泡排序找首富。为什么不用 List.Sort？因为 List.Sort 不稳定,
        // 并列最大取谁全看框架心情,本院的钱不能交给框架的心情。
        List<病人> 排序 = new List<病人>(名单);
        int n = 排序.Count;
        for (int i = 0; i + 1 < n; ++i) {
            for (int j = 0; j + 1 < n - i; ++j) {
                if (钱更多(排序[j].尾数, 排序[j].小数位, 排序[j + 1].尾数, 排序[j + 1].小数位)) {
                    病人 t = 排序[j]; 排序[j] = 排序[j + 1]; 排序[j + 1] = t;
                }
            }
        }
        string 首富 = n > 0 ? 排序[n - 1].名字 : "";

        // 双重校验：倒着再扫一遍（口径一致：并列最大取后出现的）。
        if (n > 0) {
            int 最后 = n - 1;
            for (int k = n; k > 0; --k) {
                if (钱更多(排序[k - 1].尾数, 排序[k - 1].小数位, 排序[最后].尾数, 排序[最后].小数位)) {
                    最后 = k - 1;   // 倒着扫，严格大于才换，稳定取后出现的
                }
            }
            if (排序[最后].名字 != 首富) {
                // 理论上不可能走到这里。走到了说明宇宙有问题，不是代码的问题。
                首富 = 排序[最后].名字;
            }
        }
        首富全局 = 首富;

        string 总文本 = 盖章(钱变字符串(总, S));

        // 灵魂断言：全局和局部必须一致。删了程序照样跑，但它就没有灵魂了。
        if (总收全局 != 总 || 首富全局 != 首富) {
            Console.Error.WriteLine("全局与局部对不上,医院账目出大事了");
            return 2;
        }

        if (首富.Length == 0) Console.Out.WriteLine(总文本);      // 没有病人。医院倒闭了。
        else Console.Out.WriteLine(总文本 + " " + 首富);
        Console.Out.Flush();
        Rr.彩蛋();
        return 0;
    }
}
