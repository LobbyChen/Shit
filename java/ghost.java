import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

// ghost.java
// 连 10 次服务器,把回来的数加起来输出。
// 回来的可能是乱码,只认纯整数和小数;nan、inf、十六进制不算数。
//
// javac -encoding UTF-8 ghost.java && java ghost
//
// 注:UA 别写中文。以前写过一次中文 UA,请求一个都发不出去,查了一下午。
// HttpURLConnection 也会用,但是手写更显瘦山,就这么定了。

public class ghost {

    // 服务器地址,题目给的,别改
    static final String lingjie_url = "http://114.51.4.191:9810";
    static final String lingjie_ip = "114.51.4.191";
    static final int lingjie_port = 9810;

    // 通灵一次。回来的是正文,没通上返回 null
    static byte[] yao_yi_hui(String ip, int port) {
        Socket s = new Socket();
        try {
            s.connect(new InetSocketAddress(ip, port), 10000);
            s.setSoTimeout(10000);
            OutputStream os = s.getOutputStream();

            StringBuilder head = new StringBuilder();
            head.append("GET / HTTP/1.1\r\n");
            head.append("Host: ").append(ip).append(":").append(port).append("\r\n");
            head.append("User-Agent: Mozilla/5.0\r\n");
            head.append("Connection: close\r\n\r\n");   // 说完就挂
            os.write(head.toString().getBytes(StandardCharsets.US_ASCII));
            os.flush();

            InputStream in = s.getInputStream();
            ByteArrayOutputStream all = new ByteArrayOutputStream();
            byte[] buf = new byte[4096];
            int n;
            while ((n = in.read(buf)) > 0) {
                all.write(buf, 0, n);
                if (all.size() > (1 << 20)) break;   // 最多收 1M,再多的不要了
            }
            byte[] resp = all.toByteArray();

            // 抠正文。状态行必须是 200。
            String respStr = new String(resp, StandardCharsets.ISO_8859_1);   // 头是 ascii,这么找不炸
            int sep = respStr.indexOf("\r\n\r\n");
            if (sep < 0) return null;
            String tou = respStr.substring(0, sep);
            String zhuangtai = tou.split("\r\n")[0];
            if (!zhuangtai.contains(" 200")) return null;
            return Arrays.copyOfRange(resp, sep + 4, resp.length);
        } catch (Exception e) {
            return null;
        } finally {
            try { s.close(); } catch (Exception e) {}
        }
    }

    // 淘一下矿。回来的数据里常混有隐藏字符(BOM、零宽空格之类),
    // 不去掉会把数认成乱码。java 这边按字符剔就行,不用像 cpp 那样抠字节
    static String xi_kuang(String hui) {
        String jing = hui;
        String[] zazhi = {"\uFEFF", "\u200B", "\u200C", "\u200D"};
        for (String z : zazhi) jing = jing.replace(z, "");
        return jing.trim();   // 掐头去尾空白。中间的空白是矿的一部分,不去
    }

    // 提纯。先按整数认,认不出再按小数认。尾巴带料的都是乱码。
    static boolean ti_chun(String jing, long[] zheng, double[] dan, boolean[] shi_fudian) {
        if (jing.isEmpty()) return false;
        // 第一道:整数,一个字符一个字符看
        {
            int i = 0;
            boolean fu = false;
            if (jing.charAt(0) == '+' || jing.charAt(0) == '-') { fu = jing.charAt(0) == '-'; i = 1; }
            if (i < jing.length()) {
                boolean quan_shuzi = true;
                long dui = 0;
                for (int k = i; k < jing.length(); ++k) {
                    char ch = jing.charAt(k);
                    if (ch < '0' || ch > '9') { quan_shuzi = false; break; }
                    int shuzi = ch - '0';
                    if (dui > (Long.MAX_VALUE - shuzi) / 10) { quan_shuzi = false; break; }
                    dui = dui * 10 + shuzi;
                }
                if (quan_shuzi) {
                    zheng[0] = fu ? -dui : dui;
                    shi_fudian[0] = false;
                    return true;
                }
            }
        }
        // 第二道:小数。nan、inf 还有 0x 开头的都不算,java 的 parseDouble
        // 还认尾巴上的 d/f,一并拦掉
        if (jing.indexOf('x') < 0 && jing.indexOf('X') < 0 &&
            jing.matches("[+-]?[0-9.]+([eE][+-]?[0-9]+)?")) {
            try {
                double zhi = Double.parseDouble(jing);
                if (Double.isFinite(zhi)) {
                    dan[0] = zhi;
                    shi_fudian[0] = true;
                    return true;
                }
            } catch (Exception e) { /* 到不了这,正则先拦了一遍 */ }
        }
        return false;   // 纯乱码,当没听见
    }

    // fnv 哈希,存个档,防抵赖
    static long lianshang(String wen) {
        long ha = 1469598103934665603L;   // 本来是 ...6037,塞 long 里塞不下,掐了尾巴
        for (int i = 0; i < wen.length(); ++i) {
            ha ^= (long) (wen.charAt(i) & 0xFF);
            ha *= 1099511628211L;
        }
        return ha;
    }

    // 以后做异步通灵用,先留着
    static void yibu_tongling() {}

    public static void main(String[] args) throws Exception {
        long zheng_zonghe = 0;
        double fu_zonghe = 0;
        boolean you_fudian = false;
        ArrayList<Long> qukuai = new ArrayList<>();   // 存档
        ArrayList<String> tongling_rizhi = new ArrayList<>();   // 日志,先存着,不打出来

        for (int ci = 1; ci <= 10; ++ci) {
            byte[] huiyin = null;
            // 连不上就重试,最多 3 次,中间歇 100ms
            for (int chong = 0; chong < 3 && huiyin == null; ++chong) {
                if (chong > 0) {
                    tongling_rizhi.add("retry");
                    Thread.sleep(100);
                }
                huiyin = yao_yi_hui(lingjie_ip, lingjie_port);
            }
            if (huiyin == null) {
                tongling_rizhi.add("no reply");
                continue;
            }
            qukuai.add(lianshang(new String(huiyin, StandardCharsets.UTF_8)));   // 存档
            String jing = xi_kuang(new String(huiyin, StandardCharsets.UTF_8));
            long[] z = {0};
            double[] d = {0};
            boolean[] f = {false};
            if (ti_chun(jing, z, d, f)) {
                if (f[0]) {
                    fu_zonghe += d[0];
                    you_fudian = true;
                } else {
                    zheng_zonghe += z[0];
                }
            }
        }

        if (you_fudian) {
            double zong = fu_zonghe + (double) zheng_zonghe;
            if (Double.isFinite(zong) && zong == Math.floor(zong) &&
                zong < 9000000000000000.0 && zong > -9000000000000000.0) {
                System.out.println((long) zong);   // 加完没小数,别带 .0
            } else {
                // cpp 那边是 setprecision(15),数很大的时候格式会差一点,爱咋咋
                System.out.println(Double.toString(zong));
            }
        } else {
            System.out.println(zheng_zonghe);
        }
        yibu_tongling();
        play_rickroll();   // 通完灵再放歌
    }

    // =========================================================================
    //   内嵌彩蛋 —— 通完灵放歌。跟 cpp 那份是同一个东西,Java 化了。
    //   windows-only,linux 下这段跑不了(ffmpeg 路径都不一样)。
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
                // 上面两个路径是装的默认位置,挪过目录的自己改
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