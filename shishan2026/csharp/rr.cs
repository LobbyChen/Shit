// =============================================================================
//   rr.cs —— Rr 彩蛋表演层(C# 共享版)
//   本院唯一的共享部。因为 C# 的编译器会看文件夹,不用像 cpp/java/rust
//   那样把三百行彩蛋复制粘贴四份——本院的复制粘贴预算省下来买了咖啡。
//
//   答完题后,若机器上恰有 ffmpeg、rickroll 视频与 Sonic Pi 引擎,
//   就在 stderr 上用真彩 ANSI 播一段《Never Gonna Give You Up》,
//   音频走 OSC over TCP 推给 scsynth。
//   任何一环缺失都优雅降级为两条 stderr 字幕,绝不影响答案
//   (stdout 只有答案一行,这是本院对评测程序最后的温柔)。
//
//   编译:与任一题目源码一起喂给 csc 即可:
//     csc /nologo /optimize+ /codepage:65001 /out:X.exe X.cs rr.cs
// =============================================================================
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.IO;
using System.Diagnostics;

static class Rr {
    const string 视频路径 = @"C:\Program Files\JiJiDown\Download\【官方 MV】Never Gonna Give You Up - Rick Astley P1 Never Gonna Give You Up - Rick Astley_137649199.mp4";
    static readonly string[] ffmpeg候选 = {
        @"C:\Users\18948\WorkBuddy\2026-10-01-17-32-47\rickroll_ascii\tools\ffmpeg.exe",
        @".\ffmpeg.exe",
        @".\tools\ffmpeg.exe",
        @".\..\tools\ffmpeg.exe",
    };

    public static void Log(string s) { Console.Error.WriteLine("[奖] " + s); }

    static string 找ffmpeg() {
        foreach (string p in ffmpeg候选)
            if (File.Exists(p)) return p;
        return null;
    }

    // 总入口。全量 try/catch 兜底:彩蛋炸了也算无事发生,答案早已交卷。
    public static void 彩蛋() {
        try { 彩蛋本体(); }
        catch (Exception 噩耗) { Log("彩蛋炸了,但答案早已交卷,无事发生: " + 噩耗.Message); }
    }

    static void 彩蛋本体() {
        string ffmpeg = 找ffmpeg();
        if (ffmpeg == null) { Log("找不到 ffmpeg,跳过彩蛋"); return; }
        if (!File.Exists(视频路径)) { Log("找不到视频,跳过彩蛋"); return; }

        // 判断 stdin 是不是被重定向:被重定向时 KeyAvailable 会炸,
        // 正好当探测器使(本院从不写多余的平台 API,异常就是信号)。
        bool 管道喂入 = false;
        try { bool 可用 = Console.KeyAvailable; 管道喂入 = false; }
        catch (InvalidOperationException) { 管道喂入 = true; }
        catch (IOException) { 管道喂入 = true; }

        int durSec = 管道喂入 ? 5 : 0;
        string env = Environment.GetEnvironmentVariable("RR_DURATION");
        if (!string.IsNullOrEmpty(env)) {
            int v;
            if (int.TryParse(env.Trim(), out v)) {
                if (v <= 0) { Log("RR_DURATION<=0,彩蛋取消,全体回工位"); return; }
                durSec = v;
            }
        }
        long maxFrames = durSec > 0 ? (long)durSec * 30 : 30L * 60 * 10;

        int cols = 79, rows = 21;
        int pixW = cols * 2, pixH = rows * 4;
        const int 采样率 = 48000;

        // ---------------- Sonic Pi 引擎(起不来就静音,不挡路) ----------------
        AudioEngine 引擎 = null;
        Thread 引擎线程 = null;
        if (引擎维护处.确保运行中()) {
            Log("引擎就绪(端口 " + 引擎维护处.端口 + ")");
            引擎 = new AudioEngine();
            if (引擎.初始化()) {
                引擎线程 = new Thread(引擎.运行循环);
                引擎线程.IsBackground = true;   // 主线程走人时别赖着不走
                引擎线程.Start();
            } else {
                Log("音频初始化失败,静音模式");
                引擎 = null;
            }
        } else {
            Log("引擎起不来,静音模式继续");
        }

        Process 视频进程 = null, 音频进程 = null;
        Thread 喂食线程 = null;
        try {
            视频进程 = 起ffmpeg(ffmpeg, "-v error -i \"" + 视频路径 + "\""
                + " -f rawvideo -pix_fmt rgba"
                + " -vf \"scale=" + pixW + ":" + pixH + ":flags=bilinear\""
                + " -r 30 -an -sn -");
            if (引擎 != null) {
                音频进程 = 起ffmpeg(ffmpeg, "-v error -i \"" + 视频路径 + "\""
                    + " -f f32le -acodec pcm_f32le -ac 2 -ar " + 采样率 + " -vn -sn -");
                Process 音频 = 音频进程;
                AudioEngine eng = 引擎;
                喂食线程 = new Thread(delegate() { 喂食(音频, eng, 采样率); });
                喂食线程.IsBackground = true;
                喂食线程.Start();
            }

            try { Console.CursorVisible = false; } catch (Exception) { }
            Console.Error.Write("\x1b[2J");

            int 帧字节 = pixW * pixH * 4;
            byte[] 帧 = new byte[帧字节];
            Stream 输出流 = 视频进程.StandardOutput.BaseStream;
            StringBuilder 手 = new StringBuilder(pixW * 4);
            long 帧号 = 0;
            while (true) {
                // 帧率节流:30fps,睡到点再干活,做一名有素质的播放器
                double 目标 = 0;
                double 现在 = 0;
                // (节流省略——本院实测管道读帧本身就 30fps 上下,睡眠是给 CPU 的面子)
                if (!读满(输出流, 帧, 帧字节)) break;
                渲染(帧, pixW, pixH, cols, rows, 手);
                Console.Error.Write(手.ToString());

                if (!管道喂入) {
                    try {
                        if (Console.KeyAvailable) {
                            ConsoleKeyInfo k = Console.ReadKey(true);
                            if (k.Key == ConsoleKey.Q || k.Key == ConsoleKey.Escape) break;
                        }
                    } catch (Exception) { }
                }
                ++帧号;
                if (帧号 >= maxFrames) break;
            }
            try { Console.CursorVisible = true; } catch (Exception) { }
            Console.Error.Write("\x1b[0m\x1b[2J\x1b[H");
        } finally {
            try { if (音频进程 != null && !音频进程.HasExited) 音频进程.Kill(); } catch (Exception) { }
            try { if (视频进程 != null && !视频进程.HasExited) 视频进程.Kill(); } catch (Exception) { }
            if (引擎 != null) {
                try { 引擎.全体解散(); } catch (Exception) { Log("收尾广播无人接收"); }
            }
            Log("彩蛋播完,用时请以秒表为准");
        }
    }

    static Process 起ffmpeg(string ffmpeg, string 参数) {
        ProcessStartInfo ps = new ProcessStartInfo();
        ps.FileName = ffmpeg;
        ps.Arguments = 参数;
        ps.UseShellExecute = false;
        ps.RedirectStandardOutput = true;   // 帧数据走管道
        ps.RedirectStandardError = false;   // ffmpeg 的报错直接透传给人类的 stderr
        ps.CreateNoWindow = true;
        return Process.Start(ps);
    }

    static void 喂食(Process 音频, AudioEngine eng, int 采样率) {
        try {
            Stream 流 = 音频.StandardOutput.BaseStream;
            byte[] buf = new byte[(采样率 / 10) * 2 * 4];   // 0.1 秒的立体声 f32
            while (true) {
                int 已读 = 0;
                bool 断了 = false;
                while (已读 < buf.Length) {
                    int n = 流.Read(buf, 已读, buf.Length - 已读);
                    if (n <= 0) { 断了 = true; break; }
                    已读 += n;
                }
                if (断了) break;
                float[] pcm = new float[buf.Length / 4];
                Buffer.BlockCopy(buf, 0, pcm, 0, buf.Length);
                eng.推送(pcm);
                Thread.Sleep(20);   // 喂完这口喘口气
            }
        } catch (Exception) { }
        try { eng.收工(); } catch (Exception) { }   // 收尾也要兜,背景线程炸一个全队陪葬
    }

    static bool 读满(Stream s, byte[] buf, int len) {
        int 已读 = 0;
        while (已读 < len) {
            int n = s.Read(buf, 已读, len - 已读);
            if (n <= 0) return false;
            已读 += n;
        }
        return true;
    }

    // 真彩 ANSI 半像素渲染:两个像素挤进一个「▀」的上下半,
    // 分辨率直接翻倍,硬件厂商看了沉默,显卡厂商看了流泪。
    static void 渲染(byte[] rgba, int w, int h, int cols, int rows, StringBuilder 手) {
        手.Length = 0;
        手.Append("\x1b[H");
        for (int cy = 0; cy < rows; ++cy) {
            for (int cx = 0; cx < cols; ++cx) {
                int x = (cx * w) / cols;
                int yT = ((cy * 2) * h) / (rows * 2);
                int yB = ((cy * 2 + 1) * h) / (rows * 2);
                if (x >= w) x = w - 1;
                if (yT >= h) yT = h - 1;
                if (yB >= h) yB = h - 1;
                int iT = (yT * w + x) * 4, iB = (yB * w + x) * 4;
                手.Append("\x1b[38;2;").Append(rgba[iT]).Append(';').Append(rgba[iT + 1]).Append(';').Append(rgba[iT + 2]).Append('m');
                手.Append("\x1b[48;2;").Append(rgba[iB]).Append(';').Append(rgba[iB + 1]).Append(';').Append(rgba[iB + 2]).Append('m');
                手.Append("▀");
            }
            手.Append("\x1b[0m");
            if (cy + 1 < rows) 手.Append("\r\n");
        }
        手.Append("\x1b[0m");
    }
}

// ------------------------------ Sonic Pi 桥接 --------------------------------
static class 引擎维护处 {
    public const int 端口 = 57110;
    const string 引擎路径 = @"C:\Program Files\Sonic Pi\app\server\native\Sonic Pi - SuperSonic.exe";

    public static bool 确保运行中() {
        if (端口开着(端口)) return true;
        try {
            if (File.Exists(引擎路径)) {
                ProcessStartInfo ps = new ProcessStartInfo();
                ps.FileName = 引擎路径;
                ps.Arguments = "--tcp " + 端口 + " -u " + (端口 + 1);
                ps.UseShellExecute = false;
                ps.CreateNoWindow = true;
                Process.Start(ps);   // spawn 出去就不管了,孤儿进程是成长的必经之路
            }
        } catch (Exception) { }
        for (int i = 0; i < 25 * 4; ++i) {
            if (端口开着(端口)) return true;
            Thread.Sleep(250);
        }
        return 端口开着(端口);
    }

    static bool 端口开着(int port) {
        try {
            TcpClient c = new TcpClient();
            IAsyncResult r = c.BeginConnect("127.0.0.1", port, null, null);
            bool ok = r.AsyncWaitHandle.WaitOne(150, false);
            bool 连上 = ok && c.Connected;
            c.Close();
            return 连上;
        } catch (Exception) { return false; }
    }
}

// OSC over TCP:4 字节大端长度帧 + OSC 包。与 Sonic Pi 的 scsynth 通话。
// (防火墙除外。防火墙面前,众生平等。)
class AudioEngine {
    TcpClient 客户端;
    NetworkStream 流;
    object 锁 = new object();
    Queue<float[]> 队列 = new Queue<float[]>();
    bool 收工了 = false;
    const int 缓冲池 = 12;
    int 节点号 = 10000;

    public bool 初始化() {
        try {
            TcpClient c = new TcpClient();
            for (int i = 0; i < 60; ++i) {
                try { c.Connect("127.0.0.1", 引擎维护处.端口); break; }
                catch (Exception) { Thread.Sleep(250); }
            }
            if (!c.Connected) return false;
            客户端 = c;
            流 = c.GetStream();
            发送(消息("/d_loadDir").s(@"C:\Program Files\Sonic Pi\etc\synthdefs\compiled"));
            Thread.Sleep(1200);   // 等 synthdef 装填,本院的等待从不解释
            int 帧 = (int)(48000 * 2.0);
            for (int i = 0; i < 缓冲池; ++i)
                发送(消息("/b_alloc").i(100 + i).i(帧).i(2));
            Thread.Sleep(400);
            return true;
        } catch (Exception) { return false; }
    }

    public void 推送(float[] pcm) {
        // Pulse 必须在 lock 里——写在外面会 SynchronizationLockException,
        // 后台线程当场爆炸,退出码 0xE0434352,答案再对也是零分(真实事故)。
        lock (锁) { 队列.Enqueue(pcm); Monitor.Pulse(锁); }
    }

    public void 收工() {
        lock (锁) { 收工了 = true; Monitor.PulseAll(锁); }
    }

    public void 运行循环() {
        // 全量 try/catch:音频线被老鼠咬断(scsynth 掉线/流死)时,
        // 绝不能让后台线程的未处理异常把整个进程炸成 0xE0434352——
        // 评测程序只看退出码,炸了就是零分,答案再对也白搭。
        try {
            int 槽 = 0;
            while (true) {
                float[] 块;
                lock (锁) {
                    while (队列.Count == 0 && !收工了) Monitor.Wait(锁);
                    if (队列.Count == 0) { if (收工了) break; continue; }
                    块 = 队列.Dequeue();
                }
                int 帧 = 块.Length / 2;
                if (帧 == 0) continue;
                int 缓冲号 = 100 + 槽;
                发送(消息("/b_alloc").i(缓冲号).i(帧).i(2));
                byte[] 原始 = new byte[块.Length * 4];
                Buffer.BlockCopy(块, 0, 原始, 0, 原始.Length);
                发送(消息("/b_write").i(缓冲号).s("scsynth-buffer").i(0).i(帧 * 2).i(0).blob(原始));
                发送(消息("/s_new").s("sonic-pi-basic_stereo_player").i(节点号++).i(0).i(0)
                    .s("buf").i(缓冲号).s("rate").f(1.0f).s("amp").f(0.85f)
                    .s("pan").f(0.0f).s("attack").f(0.0f).s("release").f(0.0f)
                    .s("out_bus").i(0));
                槽 = (槽 + 1) % 缓冲池;
            }
        } catch (Exception) {
            Rr.Log("音频线被老鼠咬断了,静音继续");   // 不能裸写 Log:会撞上 System.Diagnostics.Log
        }
    }

    public void 全体解散() { 发送(消息("/g_freeAll").i(0)); }

    class Osc消息 {
        public string 地址;
        public List<object> 参数 = new List<object>();   // int / float / string / byte[]
        public Osc消息 i(int v) { 参数.Add(v); return this; }
        public Osc消息 f(float v) { 参数.Add(v); return this; }
        public Osc消息 s(string v) { 参数.Add(v); return this; }
        public Osc消息 blob(byte[] v) { 参数.Add(v); return this; }
    }

    static Osc消息 消息(string 地址) {
        Osc消息 m = new Osc消息();
        m.地址 = 地址;
        return m;
    }

    void 发送(Osc消息 m) {
        List<byte> 包 = new List<byte>();
        填充串(包, m.地址);
        StringBuilder 标签 = new StringBuilder(",");
        foreach (object o in m.参数) {
            if (o is int) 标签.Append('i');
            else if (o is float) 标签.Append('f');
            else if (o is string) 标签.Append('s');
            else 标签.Append('b');
        }
        填充串(包, 标签.ToString());
        foreach (object o in m.参数) {
            if (o is int) {
                int v = (int)o;
                包.Add((byte)(v >> 24)); 包.Add((byte)(v >> 16)); 包.Add((byte)(v >> 8)); 包.Add((byte)v);
            } else if (o is float) {
                float f = (float)o;
                byte[] b4 = BitConverter.GetBytes(f);
                Array.Reverse(b4);   // 网络字节序,大端,别问
                包.AddRange(b4);
            } else if (o is string) {
                填充串(包, (string)o);
            } else {
                byte[] blob = (byte[])o;
                int n = blob.Length;
                包.Add((byte)(n >> 24)); 包.Add((byte)(n >> 16)); 包.Add((byte)(n >> 8)); 包.Add((byte)n);
                包.AddRange(blob);
                int pad = 4 - (n % 4);
                if (pad != 4) for (int i = 0; i < pad; ++i) 包.Add(0);
            }
        }
        byte[] 主体 = 包.ToArray();
        byte[] 帧 = new byte[4 + 主体.Length];
        int len = 主体.Length;
        帧[0] = (byte)(len >> 24); 帧[1] = (byte)(len >> 16); 帧[2] = (byte)(len >> 8); 帧[3] = (byte)len;
        主体.CopyTo(帧, 4);
        lock (锁) { 流.Write(帧, 0, 帧.Length); 流.Flush(); }
    }

    static void 填充串(List<byte> 包, string s) {
        byte[] b = Encoding.UTF8.GetBytes(s);
        包.AddRange(b);
        int pad = 4 - (b.Length % 4);
        for (int i = 0; i < pad; ++i) 包.Add(0);
    }
}
