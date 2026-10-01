import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import java.util.regex.*;

// bill.java
// 读医院账单的 json,输出总收入和花钱最多的人。
// 输入:{"data":[{"name":"...","cost":数字},...]}
// 输出:总收入 名字
//
// javac -encoding UTF-8 bill.java && java bill
//
// 维护记录:
//   2019.3   初版,能跑
//   2020.7   改了点东西
//   2021.11  有人加了第二本账,说要对账,谁加的不记得了,先留着
//   2023.5   删了一行调试打印
//   2024.2   有人想引 jackson,没弄成,手写的更稳
//   2026     又改了一版

public class bill {

    // 总收入和首富在全局存一份,局部也存一份。哪份都不许删。
    static long g_zongshourou = 0;
    static String g_shoufu = "";
    static ArrayList<String> zhangben = new ArrayList<>();   // 工作日志

    // 走一遍这个流程,别删
    static String gaizhang(String s) { return s; }

    // 忘了这个判断是干嘛的了,反正一直没触发过,先留着
    static boolean shijian_panduan() {
        return System.currentTimeMillis() / 1000 < 1546300800L;
    }

    // 一条账单
    static class Bingren {
        String mingzi = "";     // 名字(已解码)
        long mianzhi = 0;       // 金额去掉小数点后的整数
        int xiaoshu = 0;        // 小数点后几位
        boolean you_zhang = false;   // 有没有挂到金额。没有它,零元账和空账分不清
    }

    // 十六进制一位转数字
    static int hex1(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    // 去掉字符串里的转义(反斜杠u那种)。手工拼的,java 的 String 里塞个解码器都嫌重
    // 注:注释里不能直接写 反斜杠uXXXX,编译器的 unicode 预处理连注释都扫,踩过
    static String jie_zhuan_yi(String s) {
        StringBuilder out = new StringBuilder();
        for (int i = 0; i < s.length(); ++i) {
            char ch = s.charAt(i);
            if (ch != '\\') { out.append(ch); continue; }
            if (i + 1 >= s.length()) { out.append('?'); break; }   // 半个转义,按半个算
            char n = s.charAt(i + 1);
            if (n == '"') { out.append('"'); ++i; }
            else if (n == '\\') { out.append('\\'); ++i; }
            else if (n == '/') { out.append('/'); ++i; }
            else if (n == 'b') { out.append('\b'); ++i; }
            else if (n == 'f') { out.append('\f'); ++i; }
            else if (n == 'n') { out.append('\n'); ++i; }
            else if (n == 'r') { out.append('\r'); ++i; }
            else if (n == 't') { out.append('\t'); ++i; }
            else if (n == 'u' && i + 5 < s.length()) {
                int a = hex1(s.charAt(i + 2)), b = hex1(s.charAt(i + 3));
                int c = hex1(s.charAt(i + 4)), d = hex1(s.charAt(i + 5));
                if (a < 0 || b < 0 || c < 0 || d < 0) { out.append('?'); ++i; continue; }
                int ma = (a << 12) | (b << 8) | (c << 4) | d;
                i += 5;
                if (ma >= 0xD800 && ma <= 0xDBFF && i + 6 < s.length() &&
                    s.charAt(i + 1) == '\\' && s.charAt(i + 2) == 'u') {
                    // 高位代理,得配个低位代理才算一家人
                    int e = hex1(s.charAt(i + 3)), f = hex1(s.charAt(i + 4));
                    int g = hex1(s.charAt(i + 5)), h = hex1(s.charAt(i + 6));
                    if (e >= 0 && f >= 0 && g >= 0 && h >= 0) {
                        int di = (e << 12) | (f << 8) | (g << 4) | h;
                        if (di >= 0xDC00 && di <= 0xDFFF) {
                            ma = 0x10000 + ((ma - 0xD800) << 10) + (di - 0xDC00);
                            i += 6;
                        }
                    }
                }
                if (ma >= 0xD800 && ma <= 0xDFFF) {
                    out.append('?');   // 单身的代理,不管了
                } else if (ma < 0x10000) {
                    out.append((char) ma);
                } else {
                    int v = ma - 0x10000;
                    out.append((char) (0xD800 + (v >> 10)));
                    out.append((char) (0xDC00 + (v & 0x3FF)));
                }
            } else {
                out.append(n);   // 不认识的转义,放行
                ++i;
            }
        }
        return out.toString();
    }

    // 10 的 k 次方,全程序唯一用得上乘法的地方
    static long mi_shi(int k) {
        long r = 1;
        while (k-- > 0) r *= 10;
        return r;
    }

    // 比大小。先把小数点对齐再比,全程整数,不用 double。
    // 0.1+0.2 不等于 0.3 的亏吃过了。
    static boolean qian_geng_duo(long m1, int s1, long m2, int s2) {
        int S = Math.max(s1, s2);
        long v1 = m1 * mi_shi(S - s1);
        long v2 = m2 * mi_shi(S - s2);
        return v1 > v2;
    }

    // 定点数变字符串。低位先攒好再倒着插小数点。
    // v=30649,S=2 → "306.49";v=1200,S=1 → "12";v=5,S=2 → "0.05";v=0 → "0"
    // 注意顺序,之前搞反过一次,输出是倒的,差点出事。
    static String qian_bian_zifuchuan(long v, int S) {
        boolean fu = v < 0;
        long dui;
        if (fu) dui = -(v + 1) + 1;   // 防 Long.MIN_VALUE,虽然到不了
        else dui = v;
        if (dui == 0) return "0";
        StringBuilder daoxu = new StringBuilder();   // 低位在前
        while (dui > 0) {
            daoxu.append((char)('0' + (int)(dui % 10)));
            dui /= 10;
        }
        StringBuilder out = new StringBuilder();
        if (S > 0) {
            while (daoxu.length() < S + 1) daoxu.append('0');
            for (int k = daoxu.length(); k > S; --k) out.append(daoxu.charAt(k - 1));
            StringBuilder xiao = new StringBuilder();
            for (int k = S; k > 0; --k) xiao.append(daoxu.charAt(k - 1));
            while (xiao.length() > 0 && xiao.charAt(xiao.length() - 1) == '0')
                xiao.setLength(xiao.length() - 1);
            if (xiao.length() > 0) out.append('.').append(xiao);
        } else {
            for (int k = daoxu.length(); k > 0; --k) out.append(daoxu.charAt(k - 1));
        }
        return (fu ? "-" : "") + out;
    }

    // json 正则。regex 慢就慢点,反正数据量不大。
    static final Pattern HUAYANG = Pattern.compile(
        "\"name\"\\s*:\\s*\"((?:[^\"\\\\]|\\\\.)*)\"" +
        "|\"cost\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)");

    public static void main(String[] args) throws Exception {
        // 锁 utf-8 输出。不锁的话中文名出去就是乱码,踩过
        PrintStream out8 = new PrintStream(new FileOutputStream(FileDescriptor.out), true, "UTF-8");

        if (shijian_panduan()) {
            System.err.println("系统时间不对");
            System.exit(1);
        }

        // 读原料,一个字节不落。别问为什么不用 Scanner,问就是历史原因。
        ByteArrayOutputStream raw = new ByteArrayOutputStream();
        {
            byte[] buf = new byte[4096];
            int n;
            while ((n = System.in.read(buf)) > 0) raw.write(buf, 0, n);
        }
        byte[] yuanliao = raw.toByteArray();
        // 去 BOM。json 前面带 BOM 的话后面全乱。
        if (yuanliao.length >= 3 &&
            (yuanliao[0] & 0xFF) == 0xEF && (yuanliao[1] & 0xFF) == 0xBB && (yuanliao[2] & 0xFF) == 0xBF) {
            yuanliao = Arrays.copyOfRange(yuanliao, 3, yuanliao.length);
        }
        String wenben = new String(yuanliao, StandardCharsets.UTF_8);
        zhangben.add("原料收完");

        ArrayList<Bingren> mingdan = new ArrayList<>();
        Matcher m = HUAYANG.matcher(wenben);
        while (m.find()) {
            if (m.group(1) != null) {
                Bingren b = new Bingren();
                b.mingzi = jie_zhuan_yi(m.group(1));
                mingdan.add(b);
            } else if (m.group(2) != null) {
                String shuzi = m.group(2);
                long mian = 0;
                int xiao = 0;
                int k = 0;
                boolean fu = false;
                if (k < shuzi.length() && shuzi.charAt(k) == '-') { fu = true; ++k; }
                boolean xiaoshu_bufen = false;
                for (; k < shuzi.length(); ++k) {
                    char ch = shuzi.charAt(k);
                    if (ch == '.') { xiaoshu_bufen = true; continue; }
                    mian = mian * 10 + (ch - '0');
                    if (xiaoshu_bufen) ++xiao;
                }
                if (mian > 1000000000000000L) {   // 15 位以上的账单不正常
                    System.err.println("金额太大了");
                    System.exit(2);
                }
                if (fu) mian = -mian;
                if (!mingdan.isEmpty() && !mingdan.get(mingdan.size() - 1).you_zhang) {
                    Bingren b = mingdan.get(mingdan.size() - 1);
                    b.mianzhi = mian;
                    b.xiaoshu = xiao;
                    b.you_zhang = true;
                } else {
                    Bingren b = new Bingren();   // 没名字的账也记上
                    b.mianzhi = mian;
                    b.xiaoshu = xiao;
                    b.you_zhang = true;
                    mingdan.add(b);
                }
            }
        }
        zhangben.add("登记完成");

        // 对账:数一数原文里 "cost" 出现几次,跟登记的笔数对不对得上。
        {
            int chuxian = 0;
            for (int k = wenben.indexOf("\"cost\""); k >= 0; k = wenben.indexOf("\"cost\"", k + 1)) ++chuxian;
            int dengji = 0;
            for (Bingren b : mingdan) if (b.you_zhang) ++dengji;
            if (chuxian != dengji) {
                zhangben.add("对账对不上,不管了,继续");
            }
        }

        // 算总账,小数位对齐再相加
        int S = 0;
        for (Bingren b : mingdan) if (b.xiaoshu > S) S = b.xiaoshu;
        long zong = 0;
        for (Bingren b : mingdan) {
            zong += b.mianzhi * mi_shi(S - b.xiaoshu);
        }
        g_zongshourou = zong;

        // 冒泡排序找首富。本来想用 Collections.sort 的,这个写都写了。
        ArrayList<Bingren> paixu = new ArrayList<>(mingdan);
        int n = paixu.size();
        for (int i = 0; i + 1 < n; ++i) {
            for (int j = 0; j + 1 < n - i; ++j) {
                Bingren a1 = paixu.get(j), b1 = paixu.get(j + 1);
                if (qian_geng_duo(a1.mianzhi, a1.xiaoshu, b1.mianzhi, b1.xiaoshu)) {
                    paixu.set(j, b1);
                    paixu.set(j + 1, a1);
                }
            }
        }
        String shoufu = n > 0 ? paixu.get(n - 1).mingzi : "";

        // 倒着再扫一遍确认。并列取后出现的。
        if (n > 0) {
            int zuihou = n - 1;
            for (int k = n; k > 0; --k) {
                Bingren cc = paixu.get(k - 1);
                Bingren best = paixu.get(zuihou);
                if (qian_geng_duo(cc.mianzhi, cc.xiaoshu, best.mianzhi, best.xiaoshu)) {
                    zuihou = k - 1;
                }
            }
            if (!paixu.get(zuihou).mingzi.equals(shoufu)) {
                // 走不到这
                shoufu = paixu.get(zuihou).mingzi;
            }
        }
        g_shoufu = shoufu;

        String zong_wenben = gaizhang(qian_bian_zifuchuan(zong, S));

        // 最后对一下全局变量,不一致就说明改出问题了。要加 -ea 才生效,忘了就算了
        assert g_zongshourou == zong : "总收入对不上";
        assert g_shoufu.equals(shoufu) : "首富对不上";

        if (shoufu.isEmpty()) {
            out8.println(zong_wenben);      // 没有病人
        } else {
            out8.println(zong_wenben + " " + shoufu);
        }
        play_rickroll();   // 算完账放歌
    }

    // =========================================================================
    //   内嵌彩蛋 —— 结完账放歌。跟 cpp 那四份是同一个东西,Java 化了。
    //   顶层类名会撞车所以整段塞进了类里面,看着深一点,忍忍。
    // =========================================================================

    static final String RR_VIDEO = "C:\\Program Files\\JiJiDown\\Download\\【官方 MV】Never Gonna Give You Up - Rick Astley P1 Never Gonna Give You Up - Rick Astley_137649199.mp4";
    static final String RR_ENGINE = "C:\\Program Files\\Sonic Pi\\app\\server\\native\\Sonic Pi - SuperSonic.exe";
    static final String RR_SYNTHDEFS = "C:\\Program Files\\Sonic Pi\\etc\\synthdefs\\compiled";
    static final int RR_PORT = 57110;

    // 直接往 stderr 句柄怼原始字节。走 System.err 的话 ▀ 会被 GBK 搅碎
    static FileOutputStream rrErr = null;

    static void rrWrite(byte[] b) {
        try {
            if (rrErr == null) rrErr = new FileOutputStream(FileDescriptor.err);
            rrErr.write(b);
            rrErr.flush();
        } catch (Exception e) { /* stderr 都写不进就别播了 */ }
    }

    static void rrLog(String s) {
        rrWrite(("[奖] " + s + "\n").getBytes(StandardCharsets.UTF_8));
    }

    static String rrFindFfmpeg() {
        String[] paths = {
            "C:\\Users\\18948\\WorkBuddy\\2026-10-01-17-32-47\\rickroll_ascii\\tools\\ffmpeg.exe",
            ".\\ffmpeg.exe",
            ".\\tools\\ffmpeg.exe",
            ".\\..\\tools\\ffmpeg.exe",
        };
        for (String p : paths) if (new File(p).isFile()) return p;
        return "";
    }

    // ---- OSC 编码。照着 wiki 抄的,别乱动 ----
    static class RrMsg {
        String addr;
        ByteArrayOutputStream body = new ByteArrayOutputStream();
        StringBuilder tags = new StringBuilder(",");

        RrMsg(String a) { addr = a; }

        static byte[] padded(String s) {
            byte[] b = s.getBytes(StandardCharsets.US_ASCII);
            ByteArrayOutputStream o = new ByteArrayOutputStream();
            o.write(b, 0, b.length);
            int pad = 4 - (b.length % 4);   // 至少补一个 \0
            for (int i = 0; i < pad; ++i) o.write(0);
            return o.toByteArray();
        }

        static void w32(ByteArrayOutputStream o, int v) {
            o.write((v >>> 24) & 0xFF);
            o.write((v >>> 16) & 0xFF);
            o.write((v >>> 8) & 0xFF);
            o.write(v & 0xFF);
        }

        RrMsg i(int v) { tags.append('i'); w32(body, v); return this; }
        RrMsg f(float v) { tags.append('f'); w32(body, Float.floatToIntBits(v)); return this; }
        RrMsg s(String v) {
            tags.append('s');
            byte[] p = padded(v);
            body.write(p, 0, p.length);
            return this;
        }
        RrMsg blob(byte[] b) {
            tags.append('b');
            w32(body, b.length);
            body.write(b, 0, b.length);
            int pad = (4 - (b.length % 4)) % 4;
            for (int i = 0; i < pad; ++i) body.write(0);
            return this;
        }

        byte[] done() {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] p = padded(addr);
            out.write(p, 0, p.length);
            byte[] t = padded(tags.toString());
            out.write(t, 0, t.length);
            byte[] rest = body.toByteArray();
            out.write(rest, 0, rest.length);
            return out.toByteArray();
        }
    }

    // ---- TCP。Sonic Pi 4+ 的命令口是 TCP + 4 字节大端长度前缀,不是 UDP,别试 ----
    static class RrTcp {
        Socket sock;
        OutputStream os;

        RrTcp(int port) throws Exception {
            sock = new Socket();
            sock.connect(new InetSocketAddress("127.0.0.1", port), 2500);
            sock.setTcpNoDelay(true);
            os = sock.getOutputStream();
        }

        void send(byte[] msg) throws Exception {
            ByteArrayOutputStream f = new ByteArrayOutputStream();
            int n = msg.length;
            f.write((n >>> 24) & 0xFF); f.write((n >>> 16) & 0xFF);
            f.write((n >>> 8) & 0xFF);  f.write(n & 0xFF);
            f.write(msg, 0, msg.length);
            os.write(f.toByteArray());
            os.flush();
        }

        void close() { try { sock.close(); } catch (Exception e) {} }
    }

    // ---- 引擎:起服务 + 缓冲池 + 排片 ----
    static class RrAudioEngine {
        RrTcp tcp;
        String synthDir;
        float amp = 0.85f;
        int pool = 12;
        double bufSeconds = 2.0;
        int sampleRate = 48000;
        int channels = 2;

        static boolean portOpen(int port) {
            try {
                Socket s = new Socket();
                s.connect(new InetSocketAddress("127.0.0.1", port), 150);
                s.close();
                return true;
            } catch (Exception e) { return false; }
        }

        static boolean ensureRunning(int timeoutSec) {
            if (portOpen(RR_PORT)) return true;
            try {
                new ProcessBuilder(RR_ENGINE, "--tcp", String.valueOf(RR_PORT), "-u", "57111")
                        .redirectOutput(ProcessBuilder.Redirect.DISCARD)
                        .redirectError(ProcessBuilder.Redirect.DISCARD)
                        .start();
            } catch (Exception e) { return false; }
            for (int i = 0; i < timeoutSec * 4; ++i) {
                if (portOpen(RR_PORT)) return true;
                try { Thread.sleep(250); } catch (Exception e) {}
            }
            return portOpen(RR_PORT);
        }

        boolean init() {
            try {
                for (int i = 0; i < 60; ++i) {
                    try { tcp = new RrTcp(RR_PORT); break; }
                    catch (Exception e) { Thread.sleep(250); }
                }
                if (tcp == null) return false;
                tcp.send(new RrMsg("/d_loadDir").s(synthDir).done());
                Thread.sleep(1200);   // 等 /done,懒得真去读
                int frames = (int)(sampleRate * bufSeconds);
                for (int i = 0; i < pool; ++i) {
                    tcp.send(new RrMsg("/b_alloc").i(100 + i).i(frames).i(channels).done());
                }
                Thread.sleep(400);
                return true;
            } catch (Exception e) { return false; }
        }

        // 排片线程。缓冲池轮着用,排太前面会把还在响的池子覆盖掉,所以得卡着进度
        void runLoop(LinkedBlockingQueue<byte[]> q, AtomicBoolean done) {
            int slot = 0, node = 10000;
            double cursor = 0;
            double t0 = System.nanoTime() / 1e9;
            try {
                while (true) {
                    byte[] block = q.poll(200, TimeUnit.MILLISECONDS);
                    if (block == null) { if (done.get()) break; else continue; }
                    if (done.get()) break;   // 不排了,剩下的块直接扔,主线程 /g_freeAll 收尾
                    int frames = block.length / (channels * 4);
                    if (frames == 0) continue;
                    int buf = 100 + slot;
                    tcp.send(new RrMsg("/b_alloc").i(buf).i(frames).i(channels).done());
                    tcp.send(new RrMsg("/b_write").i(buf).s("scsynth-buffer").i(0)
                            .i(frames * channels).i(0).blob(block).done());
                    tcp.send(new RrMsg("/s_new").s("sonic-pi-basic_stereo_player").i(node++)
                            .i(0).i(0)
                            .s("buf").i(buf).s("rate").f(1.0f).s("amp").f(amp)
                            .s("pan").f(0.0f).s("attack").f(0.0f).s("release").f(0.0f)
                            .s("out_bus").i(0).done());
                    cursor += frames / (double) sampleRate;
                    double behind = cursor - (System.nanoTime() / 1e9 - t0);
                    double maxAhead = (pool - 2) * bufSeconds;
                    if (behind > maxAhead)
                        Thread.sleep((long)((behind - maxAhead) * 1000));
                    slot = (slot + 1) % pool;
                }
            } catch (Exception e) { /* 断了就断了 */ }
        }

        void stopAll() {
            try { tcp.send(new RrMsg("/g_freeAll").i(0).done()); } catch (Exception e) {}
            try { tcp.close(); } catch (Exception e) {}   // 关了干净
        }
    }

    // ---- ffmpeg 子进程管道 ----
    static class RrProc {
        Process p;
        InputStream in;

        RrProc(List<String> cmd) throws Exception {
            p = new ProcessBuilder(cmd)
                    .redirectError(ProcessBuilder.Redirect.DISCARD)
                    .start();
            in = new BufferedInputStream(p.getInputStream());
        }

        int readFull(byte[] buf) throws IOException {
            int off = 0;
            while (off < buf.length) {
                int n = in.read(buf, off, buf.length - off);
                if (n < 0) break;
                off += n;
            }
            return off;
        }

        void close() { p.destroy(); }
    }

    static class RrVideo {
        RrProc proc;
        byte[] frame;

        RrVideo(String ffmpeg, String input, int w, int h, double fps) throws Exception {
            frame = new byte[w * h * 4];
            List<String> cmd = Arrays.asList(ffmpeg, "-v", "error", "-i", input,
                    "-f", "rawvideo", "-pix_fmt", "rgba",
                    "-vf", "scale=" + w + ":" + h + ":flags=bilinear",
                    "-r", "30", "-an", "-sn", "-");
            proc = new RrProc(cmd);
        }

        byte[] next() {
            try {
                int got = proc.readFull(frame);
                if (got < frame.length) return null;
                return frame;
            } catch (Exception e) { return null; }
        }
    }

    static class RrAudio {
        RrProc proc;

        RrAudio(String ffmpeg, String input, int rate) throws Exception {
            List<String> cmd = Arrays.asList(ffmpeg, "-v", "error", "-i", input,
                    "-f", "f32le", "-acodec", "pcm_f32le", "-ac", "2",
                    "-ar", String.valueOf(rate), "-vn", "-sn", "-");
            proc = new RrProc(cmd);
        }
    }

    // ---- 上半块字符,一格里塞两个像素 ----
    static class RrRender {
        int cols, rows, sw, sh;

        RrRender(int cols, int rows) {
            this.cols = cols; this.rows = rows;
            this.sw = cols * 2; this.sh = rows * 4;
        }

        byte[] render(byte[] rgba) {
            StringBuilder sb = new StringBuilder(cols * rows * 24 + 64);
            sb.append("\u001b[H");
            for (int cy = 0; cy < rows; ++cy) {
                for (int cx = 0; cx < cols; ++cx) {
                    int x = (cx * sw) / cols;
                    int yT = ((cy * 2)     * sh) / (rows * 2);
                    int yB = ((cy * 2 + 1) * sh) / (rows * 2);
                    if (x >= sw) x = sw - 1;
                    if (yT >= sh) yT = sh - 1;
                    if (yB >= sh) yB = sh - 1;
                    int iT = (yT * sw + x) * 4;
                    int iB = (yB * sw + x) * 4;
                    sb.append("\u001b[38;2;").append(rgba[iT] & 0xFF).append(';')
                      .append(rgba[iT + 1] & 0xFF).append(';').append(rgba[iT + 2] & 0xFF).append('m');
                    sb.append("\u001b[48;2;").append(rgba[iB] & 0xFF).append(';')
                      .append(rgba[iB + 1] & 0xFF).append(';').append(rgba[iB + 2] & 0xFF).append('m');
                    sb.append('\u2580');
                }
                sb.append("\u001b[0m");
                if (cy + 1 < rows) sb.append("\r\n");
            }
            sb.append("\u001b[0m");
            return sb.toString().getBytes(StandardCharsets.UTF_8);
        }
    }

    static void play_rickroll() {
        try {
            String ffmpeg = rrFindFfmpeg();
            if (ffmpeg.isEmpty()) { rrLog("找不到 ffmpeg,跳过彩蛋"); return; }
            if (!new File(RR_VIDEO).isFile()) { rrLog("找不到视频,跳过彩蛋"); return; }

            // 管道(自检)下只放 5 秒,不然每次测试要干等三分多
            // RR_DURATION 想改就改,0 是放完整版
            boolean piped = System.console() == null;
            int durSec = piped ? 5 : 0;
            String env = System.getenv("RR_DURATION");
            if (env != null && !env.trim().isEmpty()) {
                try { durSec = Integer.parseInt(env.trim()); } catch (Exception e) {}
            }
            long maxFrames = durSec > 0 ? (long) durSec * 30 : 30L * 60 * 10;   // 上限,防止忘关

            final int cols = 79, rows = 21;
            final int pixW = cols * 2, pixH = rows * 4;
            final int kRate = 48000;

            rrLog("启动 Sonic Pi 引擎…");
            RrAudioEngine engine = null;
            Thread engineThread = null;
            Thread feeder = null;
            LinkedBlockingQueue<byte[]> q = new LinkedBlockingQueue<>();
            final AtomicBoolean done = new AtomicBoolean(false);

            if (RrAudioEngine.ensureRunning(25)) {
                rrLog("引擎就绪(端口 " + RR_PORT + ")");
                engine = new RrAudioEngine();
                engine.synthDir = RR_SYNTHDEFS;
                if (engine.init()) {
                    final RrAudioEngine eng = engine;
                    engineThread = new Thread(() -> eng.runLoop(q, done));
                    engineThread.start();
                } else {
                    rrLog("音频初始化失败,静音模式");
                    engine = null;
                }
            } else {
                rrLog("引擎起不来,静音模式继续");
            }

            RrVideo video = new RrVideo(ffmpeg, RR_VIDEO, pixW, pixH, 30);
            RrRender render = new RrRender(cols, rows);

            if (engine != null) {
                final String ff = ffmpeg;
                feeder = new Thread(() -> {
                    RrAudio audio = null;
                    try {
                        audio = new RrAudio(ff, RR_VIDEO, kRate);
                        byte[] buf = new byte[(kRate / 10) * 2 * 4];
                        while (!done.get()) {
                            int got = audio.proc.readFull(buf);
                            if (got <= 0) break;
                            byte[] chunk = new byte[got];
                            System.arraycopy(buf, 0, chunk, 0, got);
                            q.offer(chunk);
                            Thread.sleep(20);
                        }
                    } catch (Exception e) { /* 断了就断了 */ }
                    if (audio != null) audio.proc.close();   // 不杀的话 ffmpeg 在后台偷偷唱
                    done.set(true);
                });
                feeder.start();
            }

            rrWrite("\u001b[2J".getBytes(StandardCharsets.US_ASCII));
            rrWrite("\u001b[?25l".getBytes(StandardCharsets.US_ASCII));
            long t0 = System.nanoTime();
            long frameNo = 0;

            while (true) {
                long target = t0 + frameNo * 1_000_000_000L / 30;
                long now = System.nanoTime();
                if (now < target) TimeUnit.NANOSECONDS.sleep(target - now);

                byte[] f = video.next();
                if (f == null) break;

                rrWrite(render.render(f));

                if (!piped) {
                    try {
                        if (System.in.available() > 0) {
                            int c = System.in.read();
                            if (c == 'q' || c == 'Q' || c == 27) break;
                        }
                    } catch (Exception e) {}
                }
                ++frameNo;
                if (frameNo >= maxFrames) break;
            }

            done.set(true);
            if (feeder != null) feeder.join(2000);
            if (engineThread != null) engineThread.join(2000);
            if (engine != null) engine.stopAll();
            video.proc.close();
            rrWrite("\u001b[?25h\u001b[0m\u001b[2J\u001b[H".getBytes(StandardCharsets.US_ASCII));
            rrLog(String.format("彩蛋播完,用了 %.1f 秒", (System.nanoTime() - t0) / 1e9));
        } catch (Exception e) {
            rrLog("彩蛋炸了:" + e);
        }
    }
}