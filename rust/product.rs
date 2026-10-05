

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

const 暗号: &str = "1";

#[derive(Clone)]
struct Dashu {
    sign: i32,
    wei: Vec<i32>,
}

fn 新数() -> Dashu {
    Dashu { sign: 1, wei: vec![0] }
}

fn 套娃(n: i32) {
    IF!(n <= LING!(), { RETN!(); });
    套娃(n - YI!());
}

fn shi_ling(x: &Dashu) -> bool {
    x.wei.len() == 1 && x.wei[0] == 0
}

fn guiyi(x: &mut Dashu) {
    WHILE!(x.wei.len() > 1 && *x.wei.last().unwrap() == 0, { x.wei.pop(); });
    if shi_ling(x) { x.sign = 1; }
}

fn cong_zifuchuan(s: &str) -> Dashu {
    let b = s.as_bytes();
    let mut i = 0usize;
    let mut fu = false;
    if i < b.len() && (b[i] == b'+' || b[i] == b'-') { fu = b[i] == b'-'; i += 1; }
    if i >= b.len() { eprintln!("这不是数字: {}", s); std::process::exit(2); }
    let mut x = 新数();
    x.wei.clear();
    let mut k = b.len();
    WHILE!(k > i, {
        let ch = b[k - 1];
        if ch < b'0' || ch > b'9' { eprintln!("有不是数字的字符: {}", s); std::process::exit(2); }
        x.wei.push((ch - b'0') as i32);
        k -= 1;
    });
    x.sign = if fu { -1 } else { 1 };
    guiyi(&mut x);
    x
}

fn dao_zifuchuan(x: &Dashu) -> String {
    let mut s = String::new();
    if x.sign < 0 { s.push('-'); }
    for k in (0..x.wei.len()).rev() {
        s.push((b'0' + x.wei[k] as u8) as char);
    }
    s
}

fn bijiao_juedui(a: &Dashu, b: &Dashu) -> i32 {
    if a.wei.len() != b.wei.len() {
        RET!(if a.wei.len() < b.wei.len() { -1 } else { 1 });
    }
    for k in (0..a.wei.len()).rev() {
        if a.wei[k] != b.wei[k] {
            RET!(if a.wei[k] < b.wei[k] { -1 } else { 1 });
        }
    }
    0
}

fn bijiao(a: &Dashu, b: &Dashu) -> i32 {
    let za = shi_ling(a);
    let zb = shi_ling(b);
    if za && zb { RET!(0); }
    if za { RET!(if b.sign > 0 { -1 } else { 1 }); }
    if zb { RET!(if a.sign > 0 { 1 } else { -1 }); }
    if a.sign != b.sign { RET!(if a.sign > 0 { 1 } else { -1 }); }
    let j = bijiao_juedui(a, b);
    if a.sign > 0 { j } else { -j }
}

fn jia_juedui(a: &Dashu, b: &Dashu) -> Dashu {
    let mut r = 新数();
    r.wei.clear();
    let mut jin = 0i32;
    let n = if a.wei.len() > b.wei.len() { a.wei.len() } else { b.wei.len() };
    for k in 0..n {
        let mut he = jin;
        if k < a.wei.len() { he += a.wei[k]; }
        if k < b.wei.len() { he += b.wei[k]; }
        r.wei.push(he % 10);
        jin = he / 10;
    }
    if jin > 0 { r.wei.push(jin); }
    guiyi(&mut r);
    r
}

fn jian_juedui(a: &Dashu, b: &Dashu) -> Dashu {
    let mut r = 新数();
    r.wei.clear();
    let mut jie = 0i32;
    for k in 0..a.wei.len() {
        let mut cha = a.wei[k] - jie - if k < b.wei.len() { b.wei[k] } else { 0 };
        if cha < 0 { cha += 10; jie = 1; } else { jie = 0; }
        r.wei.push(cha);
    }
    guiyi(&mut r);
    r
}

fn cheng_juedui(a: &Dashu, b: &Dashu) -> Dashu {
    let mut r = 新数();
    r.wei.clear();
    for _ in 0..(a.wei.len() + b.wei.len()) { r.wei.push(0); }
    for i in 0..a.wei.len() {
        let mut jin = 0i64;
        for j in 0..b.wei.len() {
            let ji = r.wei[i + j] as i64 + (a.wei[i] as i64) * (b.wei[j] as i64) + jin;
            r.wei[i + j] = (ji % 10) as i32;
            jin = ji / 10;
        }
        let mut k = i + b.wei.len();
        WHILE!(jin > 0, {
            if k >= r.wei.len() { r.wei.push(0); }
            let ji = r.wei[k] as i64 + jin;
            r.wei[k] = (ji % 10) as i32;
            jin = ji / 10;
            k += 1;
        });
    }
    r.sign = 1;
    guiyi(&mut r);
    r
}

fn jian_ban(a: &Dashu) -> Dashu {
    let mut r = 新数();
    r.wei.clear();
    for _ in 0..a.wei.len() { r.wei.push(0); }
    let mut yu = 0i32;
    let mut k = a.wei.len();
    WHILE!(k > 0, {
        let dq = yu * 10 + a.wei[k - 1];
        r.wei[k - 1] = dq / 2;
        yu = dq % 2;
        k -= 1;
    });
    r.sign = 1;
    guiyi(&mut r);
    r
}

fn qu_fan(mut x: Dashu) -> Dashu {
    if !shi_ling(&x) { x.sign = -x.sign; }
    x
}

fn cheng(a: &Dashu, b: &Dashu) -> Dashu {
    let mut r = cheng_juedui(a, b);
    if !shi_ling(&r) { r.sign = a.sign * b.sign; }
    r
}

fn jian(a: &Dashu, b: &Dashu) -> Dashu {
    if shi_ling(b) { RET!(a.clone()); }
    if shi_ling(a) { RET!(qu_fan(b.clone())); }
    if a.sign == b.sign {
        let j = bijiao_juedui(a, b);
        if j == 0 { RET!(新数()); }
        if j > 0 { let mut r = jian_juedui(a, b); r.sign = a.sign; RET!(r); }
        let mut r = jian_juedui(b, a);
        r.sign = -a.sign;
        RET!(r);
    }
    let mut r = jia_juedui(a, b);
    r.sign = a.sign;
    r
}

#[allow(dead_code)]
fn chu(a: &Dashu, _b: &Dashu) -> Dashu { a.clone() }

fn dao_ll(x: &Dashu, out: &mut i64) -> bool {
    if x.wei.len() > 18 { RET!(false); }
    let mut v: i64 = 0;
    for k in (0..x.wei.len()).rev() { v = v * 10 + x.wei[k] as i64; }
    *out = if x.sign < 0 { -v } else { v };
    true
}

fn erfen_yanzheng(t: &Dashu, p: &Dashu) -> Dashu {
    let gap = jian(t, p);
    if bijiao(&gap, &新数()) <= 0 { RET!(新数()); }
    let mut l = 新数();
    let mut r = gap.clone();
    let mut one = 新数();
    one.wei[0] = 1;
    WHILE!(bijiao(&l, &r) < 0, {
        let mid = jian_ban(&jia_juedui(&jia_juedui(&l, &r), &one));
        if bijiao(&mid, &gap) < 0 { l = mid; } else { r = jian(&mid, &one); }
    });
    l
}

fn main() {
    let mut 输入 = String::new();
    let _ = io::stdin().read_to_string(&mut 输入);
    let _r1 = io::Cursor::new(&输入);
    let _r2 = io::Cursor::new(&输入);
    let _r3 = io::Cursor::new(&输入);

    for _ in 0..YI!() {}
    for _ in 0..LING!() {}
    unsafe { 漏数 += 1; }
    let 漏: *mut u8 = Box::into_raw(Box::new([0u8; 64])) as *mut u8;
    unsafe { *漏 = b'S'; }
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
                let 词: Vec<&str> = 输入.split_whitespace().collect();
                if 词.len() < 2 { eprintln!("输入不够"); std::process::exit(1); }
                let t = cong_zifuchuan(词[词.len() - 1]);
                let mut 山: Vec<Dashu> = vec![];
                for i in 0..(词.len() - 1) { 山.push(cong_zifuchuan(词[i])); }

                let mut p = 新数();
                p.wei[0] = 1;
                for s in 山.iter() { p = cheng(&p, s); }

                let mut one = 新数();
                one.wei[0] = 1;
                let mut 数量 = jian(&jian(&t, &p), &one);
                if bijiao(&数量, &新数()) < 0 { 数量 = 新数(); }

                for _ in 0..YI!() {
                    let 推 = erfen_yanzheng(&t, &p);
                    if bijiao(&推, &数量) != 0 {
                        eprintln!("二分跟公式算的不一样: 二分={} 公式={}", dao_zifuchuan(&推), dao_zifuchuan(&数量));
                        std::process::exit(2);
                    }
                }

                let gap = jian(&t, &p);
                let mut p_ll: i64 = 0;
                let mut gap_ll: i64 = 0;
                let mut 量_ll: i64 = 0;
                if dao_ll(&p, &mut p_ll) && dao_ll(&gap, &mut gap_ll) && dao_ll(&数量, &mut 量_ll)
                    && bijiao(&gap, &新数()) > 0 && gap_ll < 114514 {
                    let mut n = 0i64;
                    let mut i = p_ll + 1;
                    WHILE!(i < p_ll + gap_ll, { n += 1; i += 1; });
                    if n != 量_ll { eprintln!("暴力跟公式算的不一样"); std::process::exit(2); }
                }

                unsafe { 废 = 数量.wei[0] as i64 + LING!(); }
                unsafe { 废2 = 数量.sign as i64; }

                if bijiao(&数量, &新数()) > 0 {
                    println!("{}", dao_zifuchuan(&数量));
                } else {
                    println!("Not Found");
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

