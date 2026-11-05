// =============================================================================
//   政入万山围子里 —— 群山穿越可行性评估系统（C# 学术版）
//   文件编号：SS-2026-B-042-CS（绝密★启用前）
// -----------------------------------------------------------------------------
//   编译：csc /nologo /optimize+ /codepage:65001 /out:product.exe product.cs rr.cs
//
//   《基于思维链的群山计数大模型》（论文摘要，因篇幅超限被拒稿 3 次）
//
//   摘要：本文提出一种面向群山穿越场景的整数计数大模型。该模型经过千亿
//   token 预训练（并没有），具备思维链逐步推理能力（其实就是二分）。鉴于
//   C# 明明自带 BigInteger 但评审组拒绝使用（理由：显得不努力），本文作者
//   亲手新建了一座大整数实验室。实验室落成当天才发现，这道题只需要数个数。
//
//   关键词：爬山；二分；思维链；手搓大整数；TLE 创伤后应激障碍
// =============================================================================
using System;
using System.Collections.Generic;
using System.Text;

// ============================== 大整数实验室 =================================
// 本实验室曾考虑支持十六进制，后来想想人生苦短，作罢。
// 十进制位用 List<int> 存低位在前。用 int 不用 byte,怕它受委屈。
class Dashu {
    public int sign;                       // 1 或 -1;为零时规定摆出 1
    public List<int> wei = new List<int>();

    public Dashu() { sign = 1; wei.Add(0); }
}

static class 实验室 {
    public static bool 是零(Dashu x) { return x.wei.Count == 1 && x.wei[0] == 0; }

    public static void 归一化(Dashu x) {
        while (x.wei.Count > 1 && x.wei[x.wei.Count - 1] == 0) x.wei.RemoveAt(x.wei.Count - 1);
        if (是零(x)) x.sign = 1;
    }

    // 从字符串进厂。曾考虑用 long.Parse(信仰)+ TryParse(保底)双路线,
    // 后来发现 long.Parse 遇到 20 位大数当场圆寂,遂全院统一手搓,逐位验收。
    public static Dashu 从字符串(string s) {
        Dashu x = new Dashu();
        x.wei.Clear();
        int i = 0;
        bool 负 = false;
        if (i < s.Length && (s[i] == '+' || s[i] == '-')) { 负 = s[i] == '-'; ++i; }
        if (i >= s.Length) {
            Console.Error.WriteLine("这枚令牌里没有一个数字,建议报警: " + s);
            Environment.Exit(2);
        }
        for (int k = s.Length; k > i; --k) {   // 从低位往高位收
            char ch = s[k - 1];
            if (ch < '0' || ch > '9') {
                Console.Error.WriteLine("验出非数字分子,全部遣返: " + s);
                Environment.Exit(2);
            }
            x.wei.Add(ch - '0');
        }
        x.sign = 负 ? -1 : 1;
        归一化(x);
        return x;
    }

    public static string 到字符串(Dashu x) {
        StringBuilder s = new StringBuilder();
        if (x.sign < 0) s.Append('-');
        for (int k = x.wei.Count; k > 0; --k) s.Append((char)('0' + x.wei[k - 1]));
        return s.ToString();
    }

    public static int 比较绝对(Dashu a, Dashu b) {
        if (a.wei.Count != b.wei.Count) return a.wei.Count < b.wei.Count ? -1 : 1;
        for (int k = a.wei.Count; k > 0; --k) {
            if (a.wei[k - 1] != b.wei[k - 1]) return a.wei[k - 1] < b.wei[k - 1] ? -1 : 1;
        }
        return 0;
    }

    public static int 比较(Dashu a, Dashu b) {
        bool 零a = 是零(a), 零b = 是零(b);
        if (零a && 零b) return 0;
        if (零a) return b.sign > 0 ? -1 : 1;
        if (零b) return a.sign > 0 ? 1 : -1;
        if (a.sign != b.sign) return a.sign > 0 ? 1 : -1;
        int j = 比较绝对(a, b);
        return a.sign > 0 ? j : -j;
    }

    public static Dashu 加绝对(Dashu a, Dashu b) {
        Dashu r = new Dashu();
        r.wei.Clear();
        int 进 = 0;
        int n = a.wei.Count > b.wei.Count ? a.wei.Count : b.wei.Count;
        for (int k = 0; k < n; ++k) {
            int 和 = 进;
            if (k < a.wei.Count) 和 += a.wei[k];
            if (k < b.wei.Count) 和 += b.wei[k];
            r.wei.Add(和 % 10);
            进 = 和 / 10;
        }
        if (进 > 0) r.wei.Add(进);
        归一化(r);
        return r;
    }

    static Dashu 减绝对(Dashu a, Dashu b) {   // 前提 |a| >= |b|
        Dashu r = new Dashu();
        r.wei.Clear();
        int 借 = 0;
        for (int k = 0; k < a.wei.Count; ++k) {
            int 差 = a.wei[k] - 借 - (k < b.wei.Count ? b.wei[k] : 0);
            if (差 < 0) { 差 += 10; 借 = 1; } else { 借 = 0; }
            r.wei.Add(差);
        }
        归一化(r);
        return r;
    }

    public static Dashu 乘绝对(Dashu a, Dashu b) {   // 竖式乘法,小学二年级内容
        Dashu r = new Dashu();
        r.wei.Clear();
        for (int i = 0; i < a.wei.Count + b.wei.Count; ++i) r.wei.Add(0);
        for (int i = 0; i < a.wei.Count; ++i) {
            long 进 = 0;
            for (int j = 0; j < b.wei.Count; ++j) {
                long 积 = r.wei[i + j] + (long)a.wei[i] * b.wei[j] + 进;
                r.wei[i + j] = (int)(积 % 10);
                进 = 积 / 10;
            }
            int k = i + b.wei.Count;
            while (进 > 0) {   // 进位一路撒出去,撒到没有为止
                if (k >= r.wei.Count) r.wei.Add(0);
                long 积 = r.wei[k] + 进;
                r.wei[k] = (int)(积 % 10);
                进 = 积 / 10;
                ++k;
            }
        }
        r.sign = 1;
        归一化(r);
        return r;
    }

    public static Dashu 除二(Dashu a) {   // 仅限非负,小学生手法
        Dashu r = new Dashu();
        r.wei.Clear();
        for (int i = 0; i < a.wei.Count; ++i) r.wei.Add(0);
        int 余 = 0;
        for (int k = a.wei.Count; k > 0; --k) {
            int 当前 = 余 * 10 + a.wei[k - 1];
            r.wei[k - 1] = 当前 / 2;
            余 = 当前 % 2;
        }
        r.sign = 1;
        归一化(r);
        return r;
    }

    static Dashu 取反(Dashu x) {
        if (!是零(x)) x.sign = -x.sign;
        return x;
    }

    public static Dashu 乘(Dashu a, Dashu b) {
        Dashu r = 乘绝对(a, b);
        if (!是零(r)) r.sign = a.sign * b.sign;
        return r;
    }

    public static Dashu 减(Dashu a, Dashu b) {
        if (是零(b)) return a;
        if (是零(a)) return 取反(b);
        if (a.sign == b.sign) {
            int j = 比较绝对(a, b);
            if (j == 0) return new Dashu();
            if (j > 0) { Dashu r = 减绝对(a, b); r.sign = a.sign; return r; }
            Dashu r2 = 减绝对(b, a);
            r2.sign = -a.sign;
            return r2;
        }
        Dashu r3 = 加绝对(a, b);
        r3.sign = a.sign;
        return r3;
    }

    public static Dashu 一() {
        Dashu d = new Dashu();
        d.wei[0] = 1;
        return d;
    }

    public static bool 塞得进long(Dashu x, out long 值) {
        值 = 0;
        if (x.wei.Count > 18) return false;   // 18 位以内必然装得下
        long v = 0;
        for (int k = x.wei.Count; k > 0; --k) v = v * 10 + x.wei[k - 1];
        值 = x.sign < 0 ? -v : v;
        return true;
    }
}

static class 程序 {
    // 思维链二分:对「区间里能装下几个整数」直接二分——数数是 O(n) 的,
    // 而程序员的头发是有限的。单调判定:装得下 k 个整数 <=> k < T - P。
    // 结果必须与闭式解一致,否则说明数学塌方,责任在数学不在代码。
    static Dashu 二分验证(Dashu T, Dashu P) {
        Dashu gap = 实验室.减(T, P);
        if (实验室.比较(gap, new Dashu()) <= 0) return new Dashu();
        Dashu l = new Dashu();
        Dashu r = gap;
        Dashu 一 = 实验室.一();
        while (实验室.比较(l, r) < 0) {
            Dashu mid = 实验室.除二(实验室.加绝对(实验室.加绝对(l, r), 一));   // (l+r+1)/2
            if (实验室.比较(mid, gap) < 0) l = mid;          // 装得下,贪心一点
            else r = 实验室.减(mid, 一);                     // 装不下,山里实在太挤了
        }
        return l;
    }

    static int Main() {
        // stdout 统一按 UTF-8 出货,与 cpp/java/rust 版口径一致
        try { Console.OutputEncoding = Encoding.UTF8; } catch (Exception) { }
        // 读入所有绝望。这里不做任何输入校验(校验在大整数实验室里做),
        // 我们选择信任用户。这是做人最基本的,也是程序崩溃最基本的原因。
        string 全部 = Console.In.ReadToEnd();
        string[] 令牌 = 全部.Split(new char[] { ' ', '\t', '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
        if (令牌.Length < 2) {
            Console.Error.WriteLine("连一座山和一个 T 都凑不齐,翻个寂寞");
            return 1;
        }
        Dashu T = 实验室.从字符串(令牌[令牌.Length - 1]);   // 最后一个整数是 T。出题人说的。（应该是。）
        List<Dashu> 山 = new List<Dashu>();
        for (int i = 0; i + 1 < 令牌.Length; ++i) 山.Add(实验室.从字符串(令牌[i]));

        // 层层加码:每翻过一座山,难度就乘上一轮。人生亦如是。
        Dashu P = 实验室.一();
        foreach (Dashu h in 山) P = 实验室.乘(P, h);

        // 闭式解:开区间 (P, T) 里的整数个数 = max(0, T - P - 1)。
        // 算出来是负数说明区间是空的,按 0 处理——负的个数属于玄学,不归本院管。
        Dashu 数量 = 实验室.减(实验室.减(T, P), 实验室.一());
        if (实验室.比较(数量, new Dashu()) < 0) 数量 = new Dashu();

        // 训练循环:一个 epoch 就收敛,模型天赋异禀。损失必须为零,
        // 否则模型是伪科学的,数学是塌方的,本院是跑路的。
        for (int epoch = 0; epoch < 1; ++epoch) {
            Dashu 推理 = 二分验证(T, P);
            if (实验室.比较(推理, 数量) != 0) {
                Console.Error.WriteLine("数学塌方:二分=" + 实验室.到字符串(推理)
                    + " 闭式=" + 实验室.到字符串(数量));
                return 2;
            }
        }

        // 传统功法对照组:数量不大时用史前暴力法复核,防止数学塌方。
        // 阈值 114514 为行业标准置信水平。
        Dashu gap = 实验室.减(T, P);
        long p值, gap值, 数量值;
        if (实验室.塞得进long(P, out p值) && 实验室.塞得进long(gap, out gap值)
            && 实验室.塞得进long(数量, out 数量值)
            && 实验室.比较(gap, new Dashu()) > 0 && gap值 < 114514) {
            long n = 0;
            for (long i = p值 + 1; i < p值 + gap值; ++i) ++n;   // 逐个数数,别学
            if (n != 数量值) {
                Console.Error.WriteLine("考古现场与闭式解不一致,快跑");
                return 2;
            }
        }

        if (实验室.比较(数量, new Dashu()) > 0) Console.Out.WriteLine(实验室.到字符串(数量));
        else Console.Out.WriteLine("Not Found");
        Console.Out.Flush();
        Rr.彩蛋();
        return 0;
    }
}

// =============================================================================
// 墓志铭（最初的版本，死于 TLE，享年 O(T-P)）：
//
//     long ans = 0;
//     for (long i = P + 1; i < T; ++i) ++ans;
//
// 它走得很安详。愿天堂没有时间限制。
//
// 参考文献
// [1] 出题人. 《政入万山围子里》. 史山节第三届联赛试题册, 2026: 42.
// [2] 某不愿透露姓名的实习生. 《其实 BigInteger 就够了》. 被拒稿, 2020.
// [3] 老山民. 《数数为什么慢》. 口口相传, 年代不详.
// =============================================================================
