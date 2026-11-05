<?php
// =============================================================================
//   rr.php —— Rr 彩蛋表演层(PHP 共享版)
//   本院第二间共享部(C# 的 rr.cs 是第一间)。PHP 的 include 也会看文件夹,
//   四份复制粘贴的预算继续省着买咖啡。
//
//   答完题后,若机器上恰有 ffmpeg、rickroll 视频与 Sonic Pi 引擎,
//   就在 stderr 上用真彩 ANSI 播一段《Never Gonna Give You Up》,
//   音频走 OSC over TCP 推给 scsynth。
//   任何一环缺失都优雅降级为两条 stderr 字幕,绝不影响答案
//   (stdout 只有答案一行,这是本院对评测程序最后的温柔)。
//
//   PHP 版特别说明:本院没有线程(Windows 的 PHP 连 pcntl 都不给),
//   音频喂食改为与视频帧同循环交错进行——一帧图配一口饭,童叟无欺。
//
//   用法:在题目文件末尾 require __DIR__ . '/rr.php'; 之前先写好主逻辑,
//   输出答案后调用 rr_彩蛋();
// =============================================================================

// 报错一律走后门(stderr),前门只出货答案——2019 年有个 notice 混进
// stdout,评测程序当场把 "Notice: Undefined index" 判成答案,全院记大过。
error_reporting(E_ALL);
ini_set('display_errors', 'stderr');

define('RR_视频路径', 'C:\\Program Files\\JiJiDown\\Download\\【官方 MV】Never Gonna Give You Up - Rick Astley P1 Never Gonna Give You Up - Rick Astley_137649199.mp4');
$GLOBALS['RR_视频路径'] = RR_视频路径;

function rr_log(string $s): void {
    fwrite(STDERR, "[奖] " . $s . "\n");
}

function rr_找ffmpeg(): ?string {
    $候选 = array(
        'C:\\Users\\18948\\WorkBuddy\\2026-10-01-17-32-47\\rickroll_ascii\\tools\\ffmpeg.exe',
        '.\\ffmpeg.exe',
        '.\\tools\\ffmpeg.exe',
        '.\\..\\tools\\ffmpeg.exe',
    );
    foreach ($候选 as $p) if (is_file($p)) return $p;
    return null;
}

// 总入口。全量 try/catch 兜底:彩蛋炸了也算无事发生,答案早已交卷。
function rr_彩蛋(): void {
    try {
        rr_彩蛋本体();
    } catch (Throwable $噩耗) {
        rr_log("彩蛋炸了,但答案早已交卷,无事发生: " . $噩耗->getMessage());
    }
}

function rr_起ffmpeg(string $ffmpeg, string $参数, array &$管道) {
    // 注:返回值是 resource 类型,但 PHP 8 不让在签名里写 ?resource
    // (写了就告警——「resource is not a supported builtin type」,本院服从)
    $描述 = array(
        0 => array('pipe', 'r'),
        1 => array('pipe', 'w'),   // 帧数据走管道
        2 => array('file', 'php://stderr', 'a'),   // ffmpeg 的报错直接透传给人类的 stderr
    );
    $proc = proc_open('"' . $ffmpeg . '" ' . $参数, $描述, $管道);
    if (!is_resource($proc)) return null;
    fclose($管道[0]);   // ffmpeg 不吃输入,留着管道口容易把自己噎死
    return $proc;
}

function rr_读满($流, int $字节数): string {
    $全部 = '';
    while (strlen($全部) < $字节数) {
        $块 = fread($流, $字节数 - strlen($全部));
        if ($块 === false || $块 === '') return '';   // 对方挂断或没料,都算说完
        $全部 .= $块;
    }
    return $全部;
}

// 真彩 ANSI 半像素渲染:两个像素挤进一个「▀」的上下半。
function rr_渲染(string $帧, int $w, int $h, int $cols, int $rows): string {
    $手 = "\x1b[H";
    for ($cy = 0; $cy < $rows; ++$cy) {
        for ($cx = 0; $cx < $cols; ++$cx) {
            $x = intdiv($cx * $w, $cols);
            $yT = intdiv(($cy * 2) * $h, $rows * 2);
            $yB = intdiv(($cy * 2 + 1) * $h, $rows * 2);
            if ($x >= $w) $x = $w - 1;
            if ($yT >= $h) $yT = $h - 1;
            if ($yB >= $h) $yB = $h - 1;
            $iT = ($yT * $w + $x) * 4;
            $iB = ($yB * $w + $x) * 4;
            $手 .= "\x1b[38;2;" . ord($帧[$iT]) . ';' . ord($帧[$iT + 1]) . ';' . ord($帧[$iT + 2]) . 'm';
            $手 .= "\x1b[48;2;" . ord($帧[$iB]) . ';' . ord($帧[$iB + 1]) . ';' . ord($帧[$iB + 2]) . 'm';
            $手 .= "▀";
        }
        $手 .= "\x1b[0m";
        if ($cy + 1 < $rows) $手 .= "\r\n";
    }
    return $手 . "\x1b[0m";
}

function rr_彩蛋本体(): void {
    $ffmpeg = rr_找ffmpeg();
    if ($ffmpeg === null) { rr_log("找不到 ffmpeg,跳过彩蛋"); return; }
    if (!is_file(RR_视频路径)) { rr_log("找不到视频,跳过彩蛋"); return; }

    // 判断 stdin 是不是被重定向:TTY 检测失败就当被管道喂着, behave 谨慎些
    $管道喂入 = true;
    try { $管道喂入 = !stream_isatty(STDIN); } catch (Throwable $e) { $管道喂入 = true; }

    $durSec = $管道喂入 ? 5 : 0;
    $env = getenv('RR_DURATION');
    if ($env !== false && trim($env) !== '') {
        $durSec = (int)trim($env);
        if ($durSec <= 0) { rr_log("RR_DURATION<=0,彩蛋取消,全体回工位"); return; }
    }
    $maxFrames = $durSec > 0 ? $durSec * 30 : 30 * 60 * 10;

    $cols = 79; $rows = 21;
    $pixW = $cols * 2; $pixH = $rows * 4;
    $采样率 = 48000;

    // ---------------- Sonic Pi 引擎(起不来就静音,不挡路) ----------------
    $引擎 = rr_引擎初始化($采样率);
    if ($引擎 === null) {
        rr_log("引擎起不来,静音模式继续");
    } else {
        rr_log("引擎就绪(端口 " . RR_引擎端口 . ")");
    }

    $视频管道 = array(); $音频管道 = array();
    $视频proc = null; $音频proc = null;
    try {
        $视频proc = rr_起ffmpeg($ffmpeg,
            '-v error -i "' . RR_视频路径 . '" -f rawvideo -pix_fmt rgba'
            . ' -vf "scale=' . $pixW . ':' . $pixH . ':flags=bilinear"'
            . ' -r 30 -an -sn -', $视频管道);
        if ($视频proc === null) { rr_log("ffmpeg 不肯上班,跳过彩蛋"); return; }
        if ($引擎 !== null) {
            $音频proc = rr_起ffmpeg($ffmpeg,
                '-v error -i "' . RR_视频路径 . '" -f f32le -acodec pcm_f32le'
                . ' -ac 2 -ar ' . $采样率 . ' -vn -sn -', $音频管道);
        }

        $帧字节 = $pixW * $pixH * 4;
        $音频块字节 = intdiv($采样率, 30) * 2 * 4;   // 一帧(1/30秒)的立体声 f32
        // 本院没有 kbhit,非阻塞嗅探 STDIN 凑合用;嗅不动的话,终端观众
        // 请 Ctrl+C 退场或自备耐心(评测管道喂入时根本不看键盘,无碍)
        if (!$管道喂入) @stream_set_blocking(STDIN, false);
        $帧号 = 0;
        while (true) {
            $帧 = rr_读满($视频管道[1], $帧字节);
            if (strlen($帧) < $帧字节) break;

            // 一帧图配一口饭:没有线程的年代,交错就是并行(本院哲学)
            if ($引擎 !== null && $音频proc !== null && is_resource($音频proc)) {
                $饭 = rr_读满($音频管道[1], $音频块字节);
                if (strlen($饭) === $音频块字节) rr_推送音频($引擎, $饭);
            }

            fwrite(STDERR, rr_渲染($帧, $pixW, $pixH, $cols, $rows));

            if (!$管道喂入) {
                $键 = @fread(STDIN, 1);
                if ($键 !== '' && ($键 === 'q' || $键 === 'Q' || ord($键) === 27)) break;
            }
            ++$帧号;
            if ($帧号 >= $maxFrames) break;
        }
        fwrite(STDERR, "\x1b[0m\x1b[2J\x1b[H");
    } finally {
        if ($音频proc !== null && is_resource($音频proc)) {
            @proc_terminate($音频proc); @proc_close($音频proc);
        }
        if ($视频proc !== null && is_resource($视频proc)) {
            @proc_terminate($视频proc); @proc_close($视频proc);
        }
        if ($引擎 !== null) {
            try { rr_全体解散($引擎); } catch (Throwable $e) { rr_log("收尾广播无人接收"); }
        }
        rr_log("彩蛋播完,用时请以秒表为准");
    }
}

// ------------------------------ Sonic Pi 桥接 --------------------------------
define('RR_引擎端口', 57110);
define('RR_引擎路径', 'C:\\Program Files\\Sonic Pi\\app\\server\\native\\Sonic Pi - SuperSonic.exe');
define('RR_合成器目录', 'C:\\Program Files\\Sonic Pi\\etc\\synthdefs\\compiled');
$GLOBALS['RR_引擎端口'] = RR_引擎端口;
$GLOBALS['RR_引擎路径'] = RR_引擎路径;

function rr_端口开着(int $port, float $秒 = 0.15): bool {
    $fp = @fsockopen('127.0.0.1', $port, $errno, $errstr, $秒);
    if ($fp === false) return false;
    fclose($fp);
    return true;
}

// 返回连接资源,失败 null。起引擎等 25 秒这种事,本院只在锦上添花时干。
function rr_引擎初始化(int $采样率) {
    if (!rr_端口开着(RR_引擎端口)) {
        try {
            if (is_file(RR_引擎路径)) {
                // spawn 出去就不管了,孤儿进程是成长的必经之路
                proc_open('"' . RR_引擎路径 . '" --tcp ' . RR_引擎端口 . ' -u ' . (RR_引擎端口 + 1),
                    array(0 => array('file', 'nul', 'r'), 1 => array('file', 'nul', 'w'), 2 => array('file', 'nul', 'w')),
                    $废话管道);
            }
        } catch (Throwable $e) { }
        $就绪 = false;
        for ($i = 0; $i < 25 * 4; ++$i) {
            if (rr_端口开着(RR_引擎端口)) { $就绪 = true; break; }
            usleep(250000);
        }
        if (!$就绪) return null;
    }
    $fp = @fsockopen('127.0.0.1', RR_引擎端口, $errno, $errstr, 5);
    if ($fp === false) return null;
    stream_set_timeout($fp, 5);

    // /d_loadDir + 预热缓冲池。scsynth 那头装填要时间,本院的等待从不解释。
    rr_发osc($fp, '/d_loadDir', array(array('s', RR_合成器目录)));
    usleep(1200000);
    $帧 = (int)($采样率 * 2.0);
    for ($i = 0; $i < 12; ++$i) {
        rr_发osc($fp, '/b_alloc', array(array('i', 100 + $i), array('i', $帧), array('i', 2)));
    }
    usleep(400000);
    return $fp;
}

// 推一块 PCM(小端 f32 交错立体声字节)给 scsynth。
function rr_推送音频($fp, string $pcm字节): void {
    $帧 = intdiv(strlen($pcm字节), 8);
    if ($帧 === 0) return;
    static $槽 = 0;
    static $节点 = 10000;
    $缓冲号 = 100 + $槽;
    rr_发osc($fp, '/b_alloc', array(array('i', $缓冲号), array('i', $帧), array('i', 2)));
    rr_发osc($fp, '/b_write', array(
        array('i', $缓冲号), array('s', 'scsynth-buffer'), array('i', 0),
        array('i', $帧 * 2), array('i', 0), array('b', $pcm字节),
    ));
    rr_发osc($fp, '/s_new', array(
        array('s', 'sonic-pi-basic_stereo_player'), array('i', $节点++), array('i', 0), array('i', 0),
        array('s', 'buf'), array('i', $缓冲号),
        array('s', 'rate'), array('f', 1.0),
        array('s', 'amp'), array('f', 0.85),
        array('s', 'pan'), array('f', 0.0),
        array('s', 'attack'), array('f', 0.0),
        array('s', 'release'), array('f', 0.0),
        array('s', 'out_bus'), array('i', 0),
    ));
    $槽 = ($槽 + 1) % 12;
}

function rr_全体解散($fp): void {
    rr_发osc($fp, '/g_freeAll', array(array('i', 0)));
    fclose($fp);
}

// OSC over TCP:4 字节大端长度帧 + OSC 包。
// pack('G') 是大端 float——PHP 里最诚实的一个格式字符,本院感激它。
function rr_发osc($fp, string $地址, array $参数): void {
    $包 = rr_osc填充串($地址);
    $标签 = ',';
    foreach ($参数 as $p) $标签 .= $p[0];
    $包 .= rr_osc填充串($标签);
    foreach ($参数 as $p) {
        switch ($p[0]) {
            case 'i': $包 .= pack('N', $p[1]); break;
            case 'f': $包 .= pack('G', $p[1]); break;
            case 's': $包 .= rr_osc填充串($p[1]); break;
            case 'b':
                $包 .= pack('N', strlen($p[1])) . $p[1];
                $补 = 4 - (strlen($p[1]) % 4);
                if ($补 !== 4) $包 .= str_repeat("\0", $补);
                break;
        }
    }
    @fwrite($fp, pack('N', strlen($包)) . $包);
}

function rr_osc填充串(string $s): string {
    $补 = 4 - (strlen($s) % 4);
    return $s . str_repeat("\0", $补);
}
