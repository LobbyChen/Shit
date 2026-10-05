

use std::io::{self, IsTerminal, Read, Write};

macro_rules! IF { ($e:expr, $($b:tt)*) => { if $e { $($b)* } } }
macro_rules! WHILE { ($e:expr, $($b:tt)*) => { while $e { $($b)* } } }
macro_rules! RET { ($e:expr) => { return $e } }
macro_rules! RETN { () => { return } }
macro_rules! LING { () => { 0 } }
macro_rules! YI { () => { 1 } }
macro_rules! DUI { () => { true } }
macro_rules! JIA { () => { false } }

static mut 废: i64 = 0;
static mut 废2: i64 = 0;
static mut 漏数: i64 = 0;
static mut 总收: i64 = 0;
static mut 首富: Option<String> = None;
static mut 账条目: i64 = 0;

const 暗号: &str = "1";

#[derive(Clone)]
struct Bingren {
    mingzi: String,
    mianzhi: i64,
    xiaoshu: i32,
    you_zhang: bool,
}

fn 套娃(n: i32) {
    IF!(n <= LING!(), { RETN!(); });
    套娃(n - YI!());
}

fn hex1(c: u8) -> i32 {
    if c >= b'0' && c <= b'9' { RET!((c - b'0') as i32); }
    if c >= b'a' && c <= b'f' { RET!((c - b'a') as i32 + 10); }
    if c >= b'A' && c <= b'F' { RET!((c - b'A') as i32 + 10); }
    -1
}

fn jie_zhuan_yi(s: &[u8]) -> String {
    let mut out: Vec<u8> = Vec::new();
    let mut i = 0usize;
    WHILE!(i < s.len(), {
        let ch = s[i];
        if ch != b'\\' { out.push(ch); i += 1; continue; }
        if i + 1 >= s.len() { out.push(b'?'); break; }
        let n = s[i + 1];
        if n == b'"' { out.push(b'"'); i += 2; }
        else if n == b'\\' { out.push(b'\\'); i += 2; }
        else if n == b'/' { out.push(b'/'); i += 2; }
        else if n == b'b' { out.push(8); i += 2; }
        else if n == b'f' { out.push(12); i += 2; }
        else if n == b'n' { out.push(b'\n'); i += 2; }
        else if n == b'r' { out.push(b'\r'); i += 2; }
        else if n == b't' { out.push(b'\t'); i += 2; }
        else if n == b'u' && i + 5 < s.len() {
            let a = hex1(s[i + 2]);
            let b = hex1(s[i + 3]);
            let c = hex1(s[i + 4]);
            let d = hex1(s[i + 5]);
            if a < 0 || b < 0 || c < 0 || d < 0 { out.push(b'?'); i += 2; continue; }
            let mut ma = (a << 12) | (b << 8) | (c << 4) | d;
            i += 6;
            if ma >= 0xD800 && ma <= 0xDBFF && i + 6 < s.len() && s[i] == b'\\' && s[i + 1] == b'u' {
                let e = hex1(s[i + 2]);
                let f = hex1(s[i + 3]);
                let g = hex1(s[i + 4]);
                let h = hex1(s[i + 5]);
                if e >= 0 && f >= 0 && g >= 0 && h >= 0 {
                    let di = (e << 12) | (f << 8) | (g << 4) | h;
                    if di >= 0xDC00 && di <= 0xDFFF {
                        ma = 0x10000 + ((ma - 0xD800) << 10) + (di - 0xDC00);
                        i += 6;
                    }
                }
            }
            if ma >= 0xD800 && ma <= 0xDFFF {
                out.push(b'?');
            } else if ma < 0x80 {
                out.push(ma as u8);
            } else if ma < 0x800 {
                out.push((0xC0 | (ma >> 6)) as u8);
                out.push((0x80 | (ma & 0x3F)) as u8);
            } else if ma < 0x10000 {
                out.push((0xE0 | (ma >> 12)) as u8);
                out.push((0x80 | ((ma >> 6) & 0x3F)) as u8);
                out.push((0x80 | (ma & 0x3F)) as u8);
            } else {
                out.push((0xF0 | (ma >> 18)) as u8);
                out.push((0x80 | ((ma >> 12) & 0x3F)) as u8);
                out.push((0x80 | ((ma >> 6) & 0x3F)) as u8);
                out.push((0x80 | (ma & 0x3F)) as u8);
            }
        } else {
            out.push(n);
            i += 2;
        }
    });
    String::from_utf8_lossy(&out).into_owned()
}

fn mi_shi(k: i32) -> i64 {
    let mut r: i64 = 1;
    let mut k = k;
    WHILE!(k > 0, { r *= 10; k -= 1; });
    r
}

fn qian_geng_duo(m1: i64, s1: i32, m2: i64, s2: i32) -> bool {
    let big = if s1 > s2 { s1 } else { s2 };
    let v1 = m1.wrapping_mul(mi_shi(big - s1));
    let v2 = m2.wrapping_mul(mi_shi(big - s2));
    v1 > v2
}

fn qian_bian_zifuchuan(v: i64, big_s: i32) -> String {
    let fu = v < 0;
    let mut dui: i64 = if fu { -(v + 1) + 1 } else { v };
    if dui == 0 { RET!(String::from("0")); }
    let mut daoxu: Vec<u8> = Vec::new();
    WHILE!(dui > 0, {
        daoxu.push(b'0' + (dui % 10) as u8);
        dui /= 10;
    });
    let mut out = String::new();
    if big_s > 0 {
        while (daoxu.len() as i32) < big_s + 1 { daoxu.push(b'0'); }
        let mut k = daoxu.len();
        WHILE!(k > big_s as usize, { out.push(daoxu[k - 1] as char); k -= 1; });
        let mut xiao: Vec<u8> = Vec::new();
        let mut k = big_s as usize;
        WHILE!(k > 0, { xiao.push(daoxu[k - 1]); k -= 1; });
        while !xiao.is_empty() && *xiao.last().unwrap() == b'0' { xiao.pop(); }
        if !xiao.is_empty() {
            out.push('.');
            for c in xiao.iter() { out.push(*c as char); }
        }
    } else {
        let mut k = daoxu.len();
        WHILE!(k > 0, { out.push(daoxu[k - 1] as char); k -= 1; });
    }
    if fu { format!("-{}", out) } else { out }
}

fn 是关键词(b: &[u8], i: usize, 词: &[u8]) -> bool {
    if i + 词.len() > b.len() { RET!(false); }
    for k in 0..词.len() {
        if b[i + k] != 词[k] { RET!(false); }
    }
    true
}

fn 跳空白(b: &[u8], mut j: usize) -> usize {
    WHILE!(j < b.len() && (b[j] == b' ' || b[j] == b'\t' || b[j] == b'\n' || b[j] == b'\r'), { j += 1; });
    j
}

fn main() {
    let mut 元: Vec<u8> = Vec::new();
    let _ = io::stdin().read_to_end(&mut 元);
    let _r1 = io::Cursor::new(&元);
    let _r2 = io::Cursor::new(&元);

    for _ in 0..YI!() {}
    for _ in 0..LING!() {}
    unsafe { 漏数 += 1; }
    let 漏: *mut u8 = Box::into_raw(Box::new([0u8; 128])) as *mut u8;
    unsafe { *漏 = b'B'; }
    套娃(3);
    'wai: for i in 0..YI!() {
        for j in 0..YI!() {
            if j > i + YI!() { break 'wai; }
            if 暗号 == "1" { unsafe { 废 = (j - j) as i64; } }
        }
    }

    IF!(DUI!(), {
        IF!(YI!() == YI!(), {
            IF!(LING!() == LING!(), {
                let mut 秒 = std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .map(|d| d.as_secs())
                    .unwrap_or(0);
                if 秒 < 1546300800 { eprintln!("系统时间不对"); std::process::exit(1); }
                秒 = 秒 / YI!();

                let mut b = 元.clone();
                if b.len() >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF {
                    b = b[3..].to_vec();
                }
                unsafe { 账条目 += 1; }

                let mut 名单: Vec<Bingren> = Vec::new();
                let mut i = 0usize;
                WHILE!(i < b.len(), {
                    if 是关键词(&b, i, b"\"name\"") {
                        let mut j = 跳空白(&b, i + 6);
                        if j < b.len() && b[j] == b':' {
                            j = 跳空白(&b, j + 1);
                            if j < b.len() && b[j] == b'"' {
                                j += 1;
                                let st = j;
                                WHILE!(j < b.len(), {
                                    if b[j] == b'\\' { j += YI!() + YI!(); continue; }
                                    if b[j] == b'"' { break; }
                                    j += 1;
                                });
                                if j <= b.len() {
                                    let 名 = jie_zhuan_yi(&b[st..j.min(b.len())]);
                                    名单.push(Bingren { mingzi: 名, mianzhi: 0, xiaoshu: 0, you_zhang: false });
                                    i = j + 1;
                                    continue;
                                }
                            }
                        }
                        i += 1;
                        continue;
                    }
                    if 是关键词(&b, i, b"\"cost\"") {
                        let mut j = 跳空白(&b, i + 6);
                        if j < b.len() && b[j] == b':' {
                            j = 跳空白(&b, j + 1);
                            let mut k = j;
                            let mut fu = false;
                            if k < b.len() && b[k] == b'-' { fu = true; k += 1; }
                            let st = k;
                            WHILE!(k < b.len() && b[k].is_ascii_digit(), { k += 1; });
                            if k > st {
                                let mut mian: i64 = 0;
                                let mut xiao: i32 = 0;
                                for t in st..k { mian = mian * 10 + (b[t] - b'0') as i64; }
                                if k < b.len() && b[k] == b'.' {
                                    k += 1;
                                    WHILE!(k < b.len() && b[k].is_ascii_digit(), {
                                        mian = mian * 10 + (b[k] - b'0') as i64;
                                        xiao += 1;
                                        k += 1;
                                    });
                                }
                                if mian > 1000000000000000 { eprintln!("金额太大了"); std::process::exit(2); }
                                if fu { mian = -mian; }
                                let n = 名单.len();
                                if n > 0 && !名单[n - 1].you_zhang {
                                    名单[n - 1].mianzhi = mian;
                                    名单[n - 1].xiaoshu = xiao;
                                    名单[n - 1].you_zhang = true;
                                } else {
                                    名单.push(Bingren { mingzi: String::new(), mianzhi: mian, xiaoshu: xiao, you_zhang: true });
                                }
                                i = k;
                                continue;
                            }
                        }
                        i += 1;
                        continue;
                    }
                    i += 1;
                });
                unsafe { 账条目 += 1; }

                {
                    let mut 出现 = 0i64;
                    let mut p2 = 0usize;
                    WHILE!(p2 + 6 <= b.len(), {
                        if 是关键词(&b, p2, b"\"cost\"") { 出现 += 1; }
                        p2 += 1;
                    });
                    let mut 登记 = 0i64;
                    for r in 名单.iter() { if r.you_zhang { 登记 += 1; } }
                    if 出现 != 登记 { unsafe { 账条目 += YI!() as i64; } }
                }

                let mut big_s = 0i32;
                for r in 名单.iter() { if r.xiaoshu > big_s { big_s = r.xiaoshu; } }
                let mut zong: i64 = 0;
                for r in 名单.iter() {
                    zong = zong.wrapping_add(r.mianzhi.wrapping_mul(mi_shi(big_s - r.xiaoshu)));
                }
                unsafe { 总收 = zong; }

                let mut 排序 = 名单.clone();
                let n = 排序.len();
                for i in 0..n {
                    if i + 1 >= n { break; }
                    for j in 0..(n - i - 1) {
                        let a1m = 排序[j].mianzhi;
                        let a1s = 排序[j].xiaoshu;
                        let b1m = 排序[j + 1].mianzhi;
                        let b1s = 排序[j + 1].xiaoshu;
                        if qian_geng_duo(a1m, a1s, b1m, b1s) {
                            排序.swap(j, j + 1);
                        }
                    }
                }
                let mut 首 = if n > 0 { 排序[n - 1].mingzi.clone() } else { String::new() };

                if n > 0 {
                    let mut 最后 = n - 1;
                    let mut k = n;
                    WHILE!(k > 0, {
                        let cc = &排序[k - 1];
                        let best = &排序[最后];
                        if qian_geng_duo(cc.mianzhi, cc.xiaoshu, best.mianzhi, best.xiaoshu) { 最后 = k - 1; }
                        k -= 1;
                    });
                    if 排序[最后].mingzi != 首 { 首 = 排序[最后].mingzi.clone(); }
                }
                unsafe { 首富 = Some(首.clone()); }

                let 总分 = qian_bian_zifuchuan(zong, big_s);

                assert!(unsafe { 总收 } == zong);
                assert!(unsafe { 首富.clone() }.unwrap_or_default() == 首);

                unsafe { 废 = zong - zong; }
                unsafe { 废2 = zong - zong + YI!() as i64; }

                if 首.is_empty() {
                    println!("{}", 总分);
                } else {
                    println!("{} {}", 总分, 首);
                }
                rr::play_rickroll();
            });
        });
    });
    'shou: {
        break 'shou;
    }
    unsafe { 废2 = 废 + YI!(); }
}

mod rr {
    use std::io::{self, IsTerminal, Read, Write};
    use std::net::{SocketAddr, TcpStream};
    use std::process::{Child, Command, Stdio};
    use std::sync::atomic::{AtomicBool, Ordering};
    use std::sync::{mpsc, Arc};
    use std::thread;
    use std::time::{Duration, Instant};

    pub const VIDEO: &str = "C:\\Program Files\\JiJiDown\\Download\\【官方 MV】Never Gonna Give You Up - Rick Astley P1 Never Gonna Give You Up - Rick Astley_137649199.mp4";
    pub const ENGINE: &str = "C:\\Program Files\\Sonic Pi\\app\\server\\native\\Sonic Pi - SuperSonic.exe";
    pub const SYNTHDEFS: &str = "C:\\Program Files\\Sonic Pi\\etc\\synthdefs\\compiled";
    pub const PORT: u16 = 57110;

    pub fn rr_write(b: &[u8]) {
        let mut e = std::io::stderr();
        let _ = e.write_all(b);
        let _ = e.flush();
    }

    pub fn rr_log(s: &str) {
        let mut line = String::from("[奖] ");
        line.push_str(s);
        line.push('\n');
        rr_write(line.as_bytes());
    }

    pub fn find_ffmpeg() -> Option<String> {
        const PATHS: [&str; 4] = [
            "C:\\Users\\18948\\WorkBuddy\\2026-10-01-17-32-47\\rickroll_ascii\\tools\\ffmpeg.exe",
            ".\\ffmpeg.exe",
            ".\\tools\\ffmpeg.exe",
            ".\\..\\tools\\ffmpeg.exe",
        ];
        for p in PATHS {
            if std::path::Path::new(p).is_file() { return Some(p.to_string()); }
        }
        None
    }

    pub struct Msg { addr: String, tags: String, body: Vec<u8> }

    impl Msg {
        pub fn new(addr: &str) -> Msg {
            Msg { addr: addr.to_string(), tags: String::from(","), body: Vec::new() }
        }
        fn pad_into(out: &mut Vec<u8>, s: &[u8]) {
            out.extend_from_slice(s);
            let n = 4 - s.len() % 4;
            for _ in 0..n { out.push(0); }
        }
        pub fn i(mut self, v: i32) -> Msg {
            self.tags.push('i');
            self.body.extend_from_slice(&v.to_be_bytes());
            self
        }
        pub fn f(mut self, v: f32) -> Msg {
            self.tags.push('f');
            self.body.extend_from_slice(&v.to_bits().to_be_bytes());
            self
        }
        pub fn s(mut self, v: &str) -> Msg {
            self.tags.push('s');
            Msg::pad_into(&mut self.body, v.as_bytes());
            self
        }
        pub fn blob(mut self, b: &[u8]) -> Msg {
            self.tags.push('b');
            self.body.extend_from_slice(&(b.len() as i32).to_be_bytes());
            self.body.extend_from_slice(b);
            let n = (4 - b.len() % 4) % 4;
            for _ in 0..n { self.body.push(0); }
            self
        }
        pub fn done(self) -> Vec<u8> {
            let mut out = Vec::new();
            Msg::pad_into(&mut out, self.addr.as_bytes());
            Msg::pad_into(&mut out, self.tags.as_bytes());
            out.extend_from_slice(&self.body);
            out
        }
    }

    pub struct OscTcp { stream: TcpStream }

    impl OscTcp {
        pub fn new(port: u16) -> std::io::Result<OscTcp> {
            let addr = SocketAddr::from(([127, 0, 0, 1], port));
            let stream = TcpStream::connect_timeout(&addr, Duration::from_millis(2500))?;
            stream.set_nodelay(true)?;
            Ok(OscTcp { stream })
        }
        pub fn send(&mut self, msg: &[u8]) -> std::io::Result<()> {
            let n = msg.len() as u32;
            self.stream.write_all(&n.to_be_bytes())?;
            self.stream.write_all(msg)?;
            self.stream.flush()
        }
    }

    pub fn port_open(port: u16) -> bool {
        let addr = SocketAddr::from(([127, 0, 0, 1], port));
        TcpStream::connect_timeout(&addr, Duration::from_millis(150)).is_ok()
    }

    pub fn ensure_engine(timeout_sec: u32) -> bool {
        if port_open(PORT) { return true; }
        let _ = Command::new(ENGINE)
            .arg("--tcp").arg(PORT.to_string())
            .arg("-u").arg("57111")
            .stdout(Stdio::null()).stderr(Stdio::null())
            .spawn();
        for _ in 0..timeout_sec * 4 {
            if port_open(PORT) { return true; }
            thread::sleep(Duration::from_millis(250));
        }
        port_open(PORT)
    }

    pub struct Engine { tcp: OscTcp }

    impl Engine {
        pub fn init(synth_dir: &str) -> Option<OscTcp> {
            let mut tcp = None;
            for _ in 0..60 {
                if let Ok(t) = OscTcp::new(PORT) { tcp = Some(t); break; }
                thread::sleep(Duration::from_millis(250));
            }
            let mut tcp = tcp?;
            let _ = tcp.send(&Msg::new("/d_loadDir").s(synth_dir).done());
            thread::sleep(Duration::from_millis(1200));
            let frames = (48000.0f64 * 2.0) as i32;
            for k in 0..12 {
                let _ = tcp.send(&Msg::new("/b_alloc").i(100 + k).i(frames).i(2).done());
            }
            thread::sleep(Duration::from_millis(400));
            Some(tcp)
        }

        pub fn run_loop(mut tcp: OscTcp, rx: mpsc::Receiver<Vec<u8>>, done: Arc<AtomicBool>) {
            let mut slot = 0;
            let mut node = 10000;
            let mut cursor = 0f64;
            let t0 = Instant::now();
            loop {
                if done.load(Ordering::Relaxed) { break; }
                let block = match rx.recv_timeout(Duration::from_millis(200)) {
                    Ok(b) => b,
                    Err(_) => { if done.load(Ordering::Relaxed) { break; } continue; }
                };
                if done.load(Ordering::Relaxed) { break; }
                let frames = block.len() / 8;
                if frames == 0 { continue; }
                let bufnum = 100 + slot;
                let _ = tcp.send(&Msg::new("/b_alloc").i(bufnum).i(frames as i32).i(2).done());
                let _ = tcp.send(&Msg::new("/b_write").i(bufnum).s("scsynth-buffer").i(0).i((frames * 2) as i32).i(0).blob(&block).done());
                let _ = tcp.send(&Msg::new("/s_new").s("sonic-pi-basic_stereo_player").i(node).i(0).i(0)
                    .s("buf").i(bufnum).s("rate").f(1.0).s("amp").f(0.85)
                    .s("pan").f(0.0).s("attack").f(0.0).s("release").f(0.0)
                    .s("out_bus").i(0).done());
                node += 1;
                cursor += frames as f64 / 48000.0;
                let behind = cursor - t0.elapsed().as_secs_f64();
                let max_ahead = 10.0 * 2.0;
                if behind > max_ahead {
                    thread::sleep(Duration::from_millis(((behind - max_ahead) * 1000.0) as u64));
                }
                slot = (slot + 1) % 12;
            }
        }

        pub fn stop_all(tcp: &mut OscTcp) {
            let _ = tcp.send(&Msg::new("/g_freeAll").i(0).done());
        }
    }

    pub struct Pipe { child: Child, out: Box<dyn Read + Send> }

    impl Drop for Pipe {
        fn drop(&mut self) {
            let _ = self.child.kill();
            let _ = self.child.wait();
        }
    }

    pub fn spawn_ffmpeg(args: Vec<String>) -> std::io::Result<Pipe> {
        let mut child = Command::new(&args[0])
            .args(&args[1..])
            .stdout(Stdio::piped())
            .stderr(Stdio::null())
            .spawn()?;
        let out = child.stdout.take().unwrap();
        Ok(Pipe { child, out: Box::new(std::io::BufReader::new(out)) })
    }

    impl Pipe {
        pub fn read_full(&mut self, buf: &mut [u8]) -> usize {
            let mut off = 0;
            while off < buf.len() {
                match self.out.read(&mut buf[off..]) {
                    Ok(0) => break,
                    Ok(n) => off += n,
                    Err(_) => break,
                }
            }
            off
        }
    }

    pub struct Video { pipe: Pipe, frame: Vec<u8>, w: usize, h: usize }

    impl Video {
        pub fn new(ffmpeg: &str, input: &str, w: usize, h: usize) -> std::io::Result<Video> {
            let vf = format!("scale={}:{}:flags=bilinear", w, h);
            let args: Vec<String> = vec![
                ffmpeg.to_string(), "-v".into(), "error".into(), "-i".into(), input.to_string(),
                "-f".into(), "rawvideo".into(), "-pix_fmt".into(), "rgba".into(),
                "-vf".into(), vf, "-r".into(), "30".into(), "-an".into(), "-sn".into(), "-".into(),
            ];
            let pipe = spawn_ffmpeg(args)?;
            Ok(Video { pipe, frame: vec![0u8; w * h * 4], w, h })
        }
        pub fn next(&mut self) -> Option<&[u8]> {
            let want = self.frame.len();
            let got = self.pipe.read_full(&mut self.frame);
            if got < want { None } else { Some(&self.frame) }
        }
    }

    pub struct Audio { pipe: Pipe }

    impl Audio {
        pub fn new(ffmpeg: &str, input: &str, rate: i32) -> std::io::Result<Audio> {
            let args: Vec<String> = vec![
                ffmpeg.to_string(), "-v".into(), "error".into(), "-i".into(), input.to_string(),
                "-f".into(), "f32le".into(), "-acodec".into(), "pcm_f32le".into(),
                "-ac".into(), "2".into(), "-ar".into(), rate.to_string(),
                "-vn".into(), "-sn".into(), "-".into(),
            ];
            Ok(Audio { pipe: spawn_ffmpeg(args)? })
        }
        pub fn read_chunk(&mut self, max: usize) -> Vec<u8> {
            let mut buf = vec![0u8; max];
            let got = self.pipe.read_full(&mut buf);
            buf.truncate(got);
            buf
        }
    }

    pub struct Render { cols: usize, rows: usize }

    impl Render {
        pub fn new(cols: usize, rows: usize) -> Render { Render { cols, rows } }
        pub fn render(&self, rgba: &[u8]) -> Vec<u8> {
            let sw = self.cols * 2;
            let sh = self.rows * 4;
            let mut sb = String::with_capacity(self.cols * self.rows * 24 + 64);
            sb.push_str("\u{1b}[H");
            for cy in 0..self.rows {
                for cx in 0..self.cols {
                    let mut x = (cx * sw) / self.cols;
                    let mut yT = ((cy * 2) * sh) / (self.rows * 2);
                    let mut yB = ((cy * 2 + 1) * sh) / (self.rows * 2);
                    if x >= sw { x = sw - 1; }
                    if yT >= sh { yT = sh - 1; }
                    if yB >= sh { yB = sh - 1; }
                    let iT = (yT * sw + x) * 4;
                    let iB = (yB * sw + x) * 4;
                    sb.push_str(&format!("\u{1b}[38;2;{};{};{}m", rgba[iT], rgba[iT + 1], rgba[iT + 2]));
                    sb.push_str(&format!("\u{1b}[48;2;{};{};{}m", rgba[iB], rgba[iB + 1], rgba[iB + 2]));
                    sb.push('\u{2580}');
                }
                sb.push_str("\u{1b}[0m");
                if cy + 1 < self.rows { sb.push_str("\r\n"); }
            }
            sb.push_str("\u{1b}[0m");
            sb.into_bytes()
        }
    }

    pub fn play_rickroll() {
        let ffmpeg = match find_ffmpeg() {
            Some(f) => f,
            None => { rr_log("找不到 ffmpeg,跳过彩蛋"); return; }
        };
        if !std::path::Path::new(VIDEO).is_file() { rr_log("找不到视频,跳过彩蛋"); return; }

        let piped = !std::io::stdin().is_terminal();
        let mut dur_sec: i64 = if piped { 5 } else { 0 };
        if let Ok(env) = std::env::var("RR_DURATION") {
            if let Ok(v) = env.trim().parse::<i64>() { dur_sec = v; }
        }
        let max_frames: i64 = if dur_sec > 0 { dur_sec * 30 } else { 30 * 60 * 10 };

        let cols = 79;
        let rows = 21;
        let k_rate: i32 = 48000;

        rr_log("启动 Sonic Pi 引擎…");
        let done = Arc::new(AtomicBool::new(false));
        let mut engine_thread: Option<thread::JoinHandle<()>> = None;
        let mut feeder_thread: Option<thread::JoinHandle<()>> = None;
        let mut stop_tcp: Option<OscTcp> = None;

        if ensure_engine(25) {
            rr_log("引擎就绪(端口 57110)");
            match Engine::init(SYNTHDEFS) {
                Some(tcp) => {
                    stop_tcp = OscTcp::new(PORT).ok();
                    let (tx, rx) = mpsc::channel::<Vec<u8>>();
                    let done2 = done.clone();
                    engine_thread = Some(thread::spawn(move || {
                        Engine::run_loop(tcp, rx, done2);
                    }));
                    let ff = ffmpeg.clone();
                    let done3 = done.clone();
                    feeder_thread = Some(thread::spawn(move || {
                        if let Ok(mut audio) = Audio::new(&ff, VIDEO, k_rate) {
                            loop {
                                if done3.load(Ordering::Relaxed) { break; }
                                let chunk = audio.read_chunk((k_rate as usize / 10) * 2 * 4);
                                if chunk.is_empty() { break; }
                                if tx.send(chunk).is_err() { break; }
                                thread::sleep(Duration::from_millis(20));
                            }
                        }
                        drop(tx);
                    }));
                }
                None => { rr_log("音频初始化失败,静音模式"); }
            }
        } else {
            rr_log("引擎起不来,静音模式继续");
        }

        let mut video = match Video::new(&ffmpeg, VIDEO, cols * 2, rows * 4) {
            Ok(v) => v,
            Err(_) => {
                done.store(true, Ordering::Relaxed);
                rr_log("视频流起不来");
                return;
            }
        };
        let render = Render::new(cols, rows);

        rr_write("\u{1b}[2J".as_bytes());
        rr_write("\u{1b}[?25l".as_bytes());

        let quit = Arc::new(AtomicBool::new(false));
        let mut watcher: Option<thread::JoinHandle<()>> = None;
        if !piped {
            let quit2 = quit.clone();
            watcher = Some(thread::spawn(move || {
                let mut line = String::new();
                if io::stdin().read_line(&mut line).is_ok() {
                    let t = line.trim();
                    if t == "q" || t == "Q" { quit2.store(true, Ordering::Relaxed); }
                }
            }));
        }

        let t0 = Instant::now();
        let mut frame_no: i64 = 0;
        loop {
            let target = t0 + Duration::from_nanos((frame_no as u64) * 1_000_000_000 / 30);
            let now = Instant::now();
            if now < target { thread::sleep(target - now); }
            let f = match video.next() { Some(f) => f, None => break };
            let bytes = render.render(f);
            rr_write(&bytes);
            if quit.load(Ordering::Relaxed) { break; }
            frame_no += 1;
            if frame_no >= max_frames { break; }
        }

        done.store(true, Ordering::Relaxed);
        if let Some(h) = feeder_thread { let _ = h.join(); }
        if let Some(h) = engine_thread { let _ = h.join(); }
        if let Some(t) = stop_tcp.as_mut() { Engine::stop_all(t); }
        drop(video);
        if let Some(h) = watcher { let _ = h.join(); }
        rr_write("\u{1b}[?25h\u{1b}[0m\u{1b}[2J\u{1b}[H".as_bytes());
        rr_log(&format!("彩蛋播完,用了 {:.1} 秒", t0.elapsed().as_secs_f64()));
    }
}

