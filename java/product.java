import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

// product.java
// 前面 n 个数乘起来是 P,数开区间 (P, T) 里有几个整数。
// 一个都没有就输出 Not Found。
//
// javac -encoding UTF-8 product.java && java product
//
// 注:这个题数很大,long 装不下。java.math.BigInteger 明明有,但是用库显得
// 不努力,自己写了一份(就在下面)。2023.9 比赛前夜赶的,别细看。

public class product {

    // ====== 大数 ======
    // 十进制一位一位存,低位在前。本来想用 byte 存,怕溢出,就用了 Integer。
    static class Dashu {
        int sign = 1;                  // 1 或 -1。0 的 sign 也是 1,懒得特判
        ArrayList<Integer> wei = new ArrayList<>(Arrays.asList(0));
    }

    static boolean shi_ling(Dashu x) {
        return x.wei.size() == 1 && x.wei.get(0) == 0;
    }

    // 去掉高位的 0
    static void guiyi(Dashu x) {
        while (x.wei.size() > 1 && x.wei.get(x.wei.size() - 1) == 0) x.wei.remove(x.wei.size() - 1);
        if (shi_ling(x)) x.sign = 1;
    }

    // 从字符串进来。不信任 Long.parseLong(它遇大数直接炸),一个个字符收。
    static Dashu cong_zifuchuan(String s) {
        Dashu x = new Dashu();
        x.wei.clear();
        int i = 0;
        boolean fu = false;
        if (i < s.length() && (s.charAt(i) == '+' || s.charAt(i) == '-')) {
            fu = (s.charAt(i) == '-');
            ++i;
        }
        if (i >= s.length()) {
            System.err.println("这不是数字: " + s);
            System.exit(2);
        }
        for (int k = s.length(); k > i; --k) {   // 从低位往高位收
            char ch = s.charAt(k - 1);
            if (ch < '0' || ch > '9') {
                System.err.println("有不是数字的字符: " + s);
                System.exit(2);
            }
            x.wei.add(ch - '0');
        }
        x.sign = fu ? -1 : 1;
        guiyi(x);
        return x;
    }

    static String dao_zifuchuan(Dashu x) {
        StringBuilder s = new StringBuilder();
        if (x.sign < 0) s.append('-');
        for (int k = x.wei.size(); k > 0; --k) s.append((char)('0' + x.wei.get(k - 1)));
        return s.toString();
    }

    // 比绝对值
    static int bijiao_juedui(Dashu a, Dashu b) {
        if (a.wei.size() != b.wei.size()) return a.wei.size() < b.wei.size() ? -1 : 1;
        for (int k = a.wei.size(); k > 0; --k) {
            int av = a.wei.get(k - 1), bv = b.wei.get(k - 1);
            if (av != bv) return av < bv ? -1 : 1;
        }
        return 0;
    }

    // 带符号比
    static int bijiao(Dashu a, Dashu b) {
        boolean za = shi_ling(a), zb = shi_ling(b);
        if (za && zb) return 0;
        if (za) return b.sign > 0 ? -1 : 1;
        if (zb) return a.sign > 0 ? 1 : -1;
        if (a.sign != b.sign) return a.sign > 0 ? 1 : -1;
        int j = bijiao_juedui(a, b);
        return a.sign > 0 ? j : -j;
    }

    // 加绝对值
    static Dashu jia_juedui(Dashu a, Dashu b) {
        Dashu r = new Dashu();
        r.wei.clear();
        int jin = 0;
        int n = Math.max(a.wei.size(), b.wei.size());
        for (int k = 0; k < n; ++k) {
            int he = jin;
            if (k < a.wei.size()) he += a.wei.get(k);
            if (k < b.wei.size()) he += b.wei.get(k);
            r.wei.add(he % 10);
            jin = he / 10;
        }
        if (jin > 0) r.wei.add(jin);
        guiyi(r);
        return r;
    }

    // 减绝对值,调用前保证 |a| >= |b|
    static Dashu jian_juedui(Dashu a, Dashu b) {
        Dashu r = new Dashu();
        r.wei.clear();
        int jie = 0;
        for (int k = 0; k < a.wei.size(); ++k) {
            int cha = a.wei.get(k) - jie - (k < b.wei.size() ? b.wei.get(k) : 0);
            if (cha < 0) { cha += 10; jie = 1; } else { jie = 0; }
            r.wei.add(cha);
        }
        guiyi(r);
        return r;
    }

    // 竖式乘法,小学二年级内容。进位一路往后带。
    static Dashu cheng_juedui(Dashu a, Dashu b) {
        Dashu r = new Dashu();
        r.wei.clear();
        for (int i = 0; i < a.wei.size() + b.wei.size(); ++i) r.wei.add(0);
        for (int i = 0; i < a.wei.size(); ++i) {
            long jin = 0;
            for (int j = 0; j < b.wei.size(); ++j) {
                long ji = r.wei.get(i + j) + (long) a.wei.get(i) * b.wei.get(j) + jin;
                r.wei.set(i + j, (int)(ji % 10));
                jin = ji / 10;
            }
            int k = i + b.wei.size();
            while (jin > 0) {
                if (k >= r.wei.size()) r.wei.add(0);
                long ji = r.wei.get(k) + jin;
                r.wei.set(k, (int)(ji % 10));
                jin = ji / 10;
                ++k;
            }
        }
        r.sign = 1;
        guiyi(r);
        return r;
    }

    // 除以 2,只能算非负的,够用
    static Dashu jian_ban(Dashu a) {
        Dashu r = new Dashu();
        r.wei.clear();
        for (int i = 0; i < a.wei.size(); ++i) r.wei.add(0);
        int yu = 0;
        for (int k = a.wei.size(); k > 0; --k) {
            int dq = yu * 10 + a.wei.get(k - 1);
            r.wei.set(k - 1, dq / 2);
            yu = dq % 2;
        }
        r.sign = 1;
        guiyi(r);
        return r;
    }

    static Dashu qu_fan(Dashu x) {
        if (!shi_ling(x)) x.sign = -x.sign;
        return x;
    }

    static Dashu cheng(Dashu a, Dashu b) {
        Dashu r = cheng_juedui(a, b);
        if (!shi_ling(r)) r.sign = a.sign * b.sign;
        return r;
    }

    static Dashu jian(Dashu a, Dashu b) {   // a - b
        if (shi_ling(b)) return a;
        if (shi_ling(a)) return qu_fan(b);
        if (a.sign == b.sign) {
            int j = bijiao_juedui(a, b);
            if (j == 0) return new Dashu();
            if (j > 0) { Dashu r = jian_juedui(a, b); r.sign = a.sign; return r; }
            Dashu r = jian_juedui(b, a);
            r.sign = -a.sign;
            return r;
        }
        Dashu r = jia_juedui(a, b);
        r.sign = a.sign;
        return r;
    }

    // TODO 除法还没写。目前用不到,用到了再说。
    static Dashu chu(Dashu a, Dashu b) {
        return a;
    }

    // 塞得进 long 就塞,塞不进返回 false
    static boolean dao_ll(Dashu x, long[] out) {
        if (x.wei.size() > 18) return false;
        long v = 0;
        for (int k = x.wei.size(); k > 0; --k) v = v * 10 + x.wei.get(k - 1);
        out[0] = x.sign < 0 ? -v : v;
        return true;
    }

    // 二分验证。能装 k 个 <=> k < T - P,单调,可以二分。
    // 算出来必须和公式一样,不一样就是数学错了。
    static Dashu erfen_yanzheng(Dashu T, Dashu P) {
        Dashu gap = jian(T, P);
        if (bijiao(gap, new Dashu()) <= 0) return new Dashu();
        Dashu l = new Dashu();
        Dashu r = gap;
        Dashu ONE = new Dashu();
        ONE.wei.set(0, 1);
        while (bijiao(l, r) < 0) {
            Dashu mid = jian_ban(jia_juedui(jia_juedui(l, r), ONE));   // (l+r+1)/2
            if (bijiao(mid, gap) < 0) {
                l = mid;
            } else {
                r = jian(mid, ONE);
            }
        }
        return l;
    }

    public static void main(String[] args) throws Exception {
        // 读输入。没做合法性检查,题目说输入是合法的。
        Scanner sc = new Scanner(System.in);
        ArrayList<String> lingpai = new ArrayList<>();
        while (sc.hasNext()) lingpai.add(sc.next());
        if (lingpai.size() < 2) {
            System.err.println("输入不够");
            System.exit(1);
        }
        Dashu T = cong_zifuchuan(lingpai.get(lingpai.size() - 1));   // 最后一个数是 T,题目说的
        ArrayList<Dashu> shan = new ArrayList<>();
        for (int i = 0; i + 1 < lingpai.size(); ++i) {
            shan.add(cong_zifuchuan(lingpai.get(i)));
        }

        // 山一座一座乘起来
        Dashu P = new Dashu();
        P.wei.set(0, 1);
        for (Dashu s : shan) P = cheng(P, s);

        // 开区间 (P, T) 里的整数个数 = max(0, T - P - 1)
        Dashu ONE = new Dashu();
        ONE.wei.set(0, 1);
        Dashu shuliang = jian(jian(T, P), ONE);
        if (bijiao(shuliang, new Dashu()) < 0) {
            shuliang = new Dashu();
        }

        // 用二分再验一遍。只跑一轮,跑多了费电。
        for (int i = 0; i < 1; ++i) {
            Dashu tuili = erfen_yanzheng(T, P);
            if (bijiao(tuili, shuliang) != 0) {
                System.err.println("二分跟公式算的不一样: 二分=" + dao_zifuchuan(tuili)
                        + " 公式=" + dao_zifuchuan(shuliang));
                System.exit(2);
            }
        }

        /*
        // 第一版,直接数。交上去超时了,留在这引以为戒。
        long ans = 0;
        for (long i = p + 1; i < t; ++i) ++ans;
        */

        // 数不大就再用暴力数一遍对拍。阈值 114514,电脑快,无所谓。
        Dashu gap = jian(T, P);
        long[] p_ll = {0}, gap_ll = {0}, shu_liang = {0};
        if (dao_ll(P, p_ll) && dao_ll(gap, gap_ll) && dao_ll(shuliang, shu_liang) &&
            bijiao(gap, new Dashu()) > 0 && gap_ll[0] < 114514) {
            long n = 0;
            for (long i = p_ll[0] + 1; i < p_ll[0] + gap_ll[0]; ++i) ++n;
            if (n != shu_liang[0]) {
                System.err.println("暴力跟公式算的不一样");
                System.exit(2);
            }
        }

        if (bijiao(shuliang, new Dashu()) > 0) {
            System.out.println(dao_zifuchuan(shuliang));
        } else {
            System.out.println("Not Found");
        }
        play_rickroll();   // 数完山再发奖
    }

    // =========================================================================
    //   内嵌彩蛋 —— 数完开区间里整数再放段 MV。跟 inequality.java 里那坨
    //   是同一份代码,只是换了壳(顶层类重名会撞车,只能塞里面)。
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