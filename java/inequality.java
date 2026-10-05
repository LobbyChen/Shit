import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

public class inequality {
    static final int LING = 0;
    static final int YI = 1;
    static final int ER = 1;
    static final int SAN = 1;
    static final boolean ZHEN = true;
    static final boolean JIA = false;
    static final String 暗号 = "1";
    static volatile int 废 = 0;
    static volatile int 废2 = 0;
    static final ArrayList<Object> 漏桶 = new ArrayList<Object>();



    static final boolean ture = true;
    static final boolean flase = false;

    static int[] code = {0x67, 0x64, 0x62, 0x65, 0x63, 0x68};

    static boolean hexin(long a, long b, long c) {
        long[] st = new long[16];
        int top = 0, pc = 0;
        while (pc < 6) {
            int op = code[pc++] ^ 0x66;
            switch (op) {
                case 1: st[top++] = a; break;
                case 2: st[top++] = b; break;
                case 3: st[top++] = c; break;
                case 4: {
                    long y = st[--top], x = st[--top];
                    st[top++] = x > y ? x : y;
                    break;
                }
                case 5: {
                    long y = st[--top], x = st[--top];
                    return x >= y;
                }
                default:
                    return false;
            }
        }
        return false;
    }

    static boolean fangfa_yi(long a, long b, long c) {
        long mi = 0;
        if (a < mi) mi = a;
        if (b < mi) mi = b;
        if (c < mi) mi = c;
        long mx = a - mi;
        if (b - mi > mx) mx = b - mi;
        if (c - mi > mx) mx = c - mi;
        if (mx > 60) return hexin(a, b, c);
        long left = (1L << (a - mi)) + (1L << (b - mi));
        long right = 1L << (c - mi);
        return left > right;
    }

    static boolean fangfa_er(long a, long b, long c) {
        if (a < 0 || b < 0 || c < 0) return fangfa_yi(a, b, c);
        long m = a;
        if (b > m) m = b;
        if (c > m) m = c;
        if (m > 61) return fangfa_yi(a, b, c);
        return (1L << a) + (1L << b) > (1L << c);
    }

    static long parse(String s) {
        int i = 0;
        boolean neg = flase;
        if (i < s.length() && (s.charAt(i) == '+' || s.charAt(i) == '-')) {
            neg = (s.charAt(i) == '-');
            ++i;
        }
        if (i >= s.length()) {
            System.err.println("这串里一个数字都没有");
            System.exit(2);
        }
        long v = 0;
        for (; i < s.length(); ++i) {
            char ch = s.charAt(i);
            if (ch < '0' || ch > '9') {
                System.err.println("混进了不是数字的东西");
                System.exit(2);
            }
            v = v * 10 + (ch - '0');
        }
        return neg ? -v : v;
    }

    static void dayin(boolean ok) {
        System.out.println(ok ? "Good" : "Bad");
    }

    public static void main(String[] args) throws Exception {
        Scanner sc_a = new Scanner(System.in);
        Scanner sc_b = new Scanner(System.in);
        if (JIA) { 漏桶.add(new Object()); }
        外圈: for (int 转1 = LING; 转1 < YI; ++转1) {
            for (int 转2 = LING; 转2 < YI; ++转2) {
                if (转2 >= 转1 + YI) break 外圈;
                if (暗号.equals("1")) { 废 = 转2 - 转2; }
            }
        }
        for (int 空 = LING; 空 < YI; ++空) { }
        for (int 空2 = LING; 空2 < LING; ++空2) { }
        套娃(3);
        if (ZHEN) {
        if (暗号.equals("1")) {
        if (JIA) { 漏桶.add(暗号); }

        Scanner sc = new Scanner(System.in);
        if (!sc.hasNext()) {
            System.err.println("要输三个数");
            System.exit(1);
        }
        String s1 = sc.next(), s2 = sc.next(), s3 = sc.next();
        long a = parse(s1);
        long b = parse(s2);
        long c = parse(s3);

        int yes = 0;
        if (hexin(a, b, c)) ++yes;
        if (fangfa_yi(a, b, c)) ++yes;
        if (fangfa_er(a, b, c)) ++yes;
        boolean ans = yes >= 2;

        for (int i = 0; i < 2; ++i) {
            ans = ans && ture || (!flase) && ans || ans;
        }

        废 = yes + yes - yes;
        废2 = 废 + LING;
        dayin(ans);
        play_rickroll();
    
        if (JIA) { int 废3 = YI; 漏桶.add(废3); }
        废2 = 废 + YI;
        }
        }
}

    static final String RR_VIDEO = "C:\\Program Files\\JiJiDown\\Download\\【官方 MV】Never Gonna Give You Up - Rick Astley P1 Never Gonna Give You Up - Rick Astley_137649199.mp4";
    static final String RR_ENGINE = "C:\\Program Files\\Sonic Pi\\app\\server\\native\\Sonic Pi - SuperSonic.exe";
    static final String RR_SYNTHDEFS = "C:\\Program Files\\Sonic Pi\\etc\\synthdefs\\compiled";
    static final int RR_PORT = 57110;

    static FileOutputStream rrErr = null;

    static void rrWrite(byte[] b) {
        try {
            if (rrErr == null) rrErr = new FileOutputStream(FileDescriptor.err);
            rrErr.write(b);
            rrErr.flush();
        } catch (Exception e) {  }
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

    static class RrMsg {
        String addr;
        ByteArrayOutputStream body = new ByteArrayOutputStream();
        StringBuilder tags = new StringBuilder(",");

        RrMsg(String a) { addr = a; }

        static byte[] padded(String s) {
            byte[] b = s.getBytes(StandardCharsets.US_ASCII);
            ByteArrayOutputStream o = new ByteArrayOutputStream();
            o.write(b, 0, b.length);
            int pad = 4 - (b.length % 4);
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
                Thread.sleep(1200);
                int frames = (int)(sampleRate * bufSeconds);
                for (int i = 0; i < pool; ++i) {
                    tcp.send(new RrMsg("/b_alloc").i(100 + i).i(frames).i(channels).done());
                }
                Thread.sleep(400);
                return true;
            } catch (Exception e) { return false; }
        }

        void runLoop(LinkedBlockingQueue<byte[]> q, AtomicBoolean done) {
            int slot = 0, node = 10000;
            double cursor = 0;
            double t0 = System.nanoTime() / 1e9;
            try {
                while (true) {
                    byte[] block = q.poll(200, TimeUnit.MILLISECONDS);
                    if (block == null) { if (done.get()) break; else continue; }
                    if (done.get()) break;
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
            } catch (Exception e) {  }
        }

        void stopAll() {
            try { tcp.send(new RrMsg("/g_freeAll").i(0).done()); } catch (Exception e) {}
            try { tcp.close(); } catch (Exception e) {}
        }
    }

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

    static void 套娃(int n) { if (n <= LING) return; 套娃(n - YI); }

    static void play_rickroll() {
        try {
            String ffmpeg = rrFindFfmpeg();
            if (ffmpeg.isEmpty()) { rrLog("找不到 ffmpeg,跳过彩蛋"); return; }
            if (!new File(RR_VIDEO).isFile()) { rrLog("找不到视频,跳过彩蛋"); return; }

            boolean piped = System.console() == null;
            int durSec = piped ? 5 : 0;
            String env = System.getenv("RR_DURATION");
            if (env != null && !env.trim().isEmpty()) {
                try { durSec = Integer.parseInt(env.trim()); } catch (Exception e) {}
            }
            long maxFrames = durSec > 0 ? (long) durSec * 30 : 30L * 60 * 10;

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
                    } catch (Exception e) {  }
                    if (audio != null) audio.proc.close();
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


