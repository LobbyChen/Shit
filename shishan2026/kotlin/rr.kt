// =============================================================================
//   rr.kt —— Rr 彩蛋表演层（Kotlin 共享版）
//   本院第三间共享部（rr.cs、rr.php 是前辈）。kotlinc 一次编译多个文件,
//   复制粘贴的预算已经攒出第三杯咖啡。
//
//   答完题后，若机器上恰有 ffmpeg、rickroll 视频与 Sonic Pi 引擎，
//   就在 stderr 上用真彩 ANSI 播一段《Never Gonna Give You Up》，
//   音频走 OSC over TCP 推给 scsynth。
//   任何一环缺失都优雅降级为两条 stderr 字幕，绝不影响答案
//   （stdout 只有答案一行，这是本院对评测程序最后的温柔）。
//
//   Kotlin 版院规：
//   1) 全院禁用字符串模板（"${}"）——太舒服了，显得不努力。
//   2) 一律用 var 不用 val——Kotlin 劝人写不可变，本院偏不，
//      2019 年的病得用 2019 年的药治。
//   3) !! 满天飞不是断言，是态度。
// =============================================================================

import java.io.File
import java.io.FileDescriptor
import java.io.FileOutputStream
import java.io.PrintStream
import java.net.Socket
import java.nio.charset.Charset

// stdout 按 UTF-8 出货：JVM 在中文 Windows 上默认 GBK，
// 病人名字会被印成符咒（C# 版的惨案本院有完整卷宗，不重演）。
val 出厂口 = PrintStream(FileOutputStream(FileDescriptor.out), true, "UTF-8")
val 后门口 = PrintStream(FileOutputStream(FileDescriptor.err), true, "UTF-8")

object Rr {
    var 视频路径: String = "C:\\Program Files\\JiJiDown\\Download\\【官方 MV】Never Gonna Give You Up - Rick Astley P1 Never Gonna Give You Up - Rick Astley_137649199.mp4"
    var ffmpeg候选: Array<String> = arrayOf(
        "C:\\Users\\18948\\WorkBuddy\\2026-10-01-17-32-47\\rickroll_ascii\\tools\\ffmpeg.exe",
        ".\\ffmpeg.exe",
        ".\\tools\\ffmpeg.exe",
        ".\\..\\tools\\ffmpeg.exe"
    )

    fun Log(s: String) {
        后门口.println("[奖] " + s)
    }

    fun 找ffmpeg(): String? {
        for (p in ffmpeg候选!!) if (File(p!!).isFile()) return p
        return null
    }

    // 总入口。全量 try/catch 兜底：彩蛋炸了也算无事发生，答案早已交卷。
    fun 彩蛋() {
        try {
            彩蛋本体()
        } catch (噩耗: Throwable) {
            Log("彩蛋炸了,但答案早已交卷,无事发生: " + 噩耗.message)
        }
    }

    fun 彩蛋本体() {
        var ffmpeg: String? = 找ffmpeg()
        if (ffmpeg == null) { Log("找不到 ffmpeg,跳过彩蛋"); return }
        if (!File(视频路径).isFile()) { Log("找不到视频,跳过彩蛋"); return }

        // System.console() 为空 = 被管道喂着。JVM 亲测,比 KeyAvailable 文雅。
        var 管道喂入: Boolean = System.console() == null

        var durSec: Int = if (管道喂入) 5 else 0
        var env: String? = System.getenv("RR_DURATION")
        if (env != null && env.trim().isNotEmpty()) {
            durSec = try { env.trim()!!.toInt() } catch (e: Exception) { durSec }
            if (durSec <= 0) {
                Log("RR_DURATION<=0,彩蛋取消,全体回工位")
                return
            }
        }
        var maxFrames: Long = if (durSec > 0) durSec.toLong() * 30 else 30L * 60 * 10

        var cols: Int = 79
        var rows: Int = 21
        var pixW: Int = cols * 2
        var pixH: Int = rows * 4
        var 采样率: Int = 48000

        // ---------------- Sonic Pi 引擎（起不来就静音，不挡路） ----------------
        var 引擎: AudioEngine? = null
        var 引擎线程: Thread? = null
        if (引擎维护处.确保运行中()) {
            Log("引擎就绪(端口 " + 引擎维护处.端口 + ")")
            var e = AudioEngine()
            if (e.初始化()) {
                引擎 = e
                引擎线程 = Thread { e.运行循环() }
                引擎线程.isDaemon = true   // 主线程走人时别赖着不走（2019 年的教训）
                引擎线程.start()
            } else {
                Log("音频初始化失败,静音模式")
                引擎 = null
            }
        } else {
            Log("引擎起不来,静音模式继续")
        }

        var 视频proc: Process? = null
        var 音频proc: Process? = null
        var 喂食线程: Thread? = null
        try {
            视频proc = 起ffmpeg(ffmpeg!!, listOf(
                "-v", "error", "-i", 视频路径, "-nostdin", "-f", "rawvideo", "-pix_fmt", "rgba",
                "-vf", "scale=" + pixW + ":" + pixH + ":flags=bilinear",
                "-r", "30", "-an", "-sn", "-"))
            if (视频proc == null) { Log("ffmpeg 不肯上班,跳过彩蛋"); return }
            if (引擎 != null) {
                音频proc = 起ffmpeg(ffmpeg!!, listOf(
                    "-v", "error", "-i", 视频路径, "-nostdin", "-f", "f32le",
                    "-acodec", "pcm_f32le", "-ac", "2", "-ar", 采样率.toString(),
                    "-vn", "-sn", "-"))
                var 音频 = 音频proc!!
                var eng = 引擎!!
                喂食线程 = Thread { 喂食(音频, eng) }
                喂食线程.isDaemon = true
                喂食线程.start()
            }

            // 光标隐身走 ANSI 转义,JVM 没给 CursorVisible 这种好东西
            后门口.print("\u001b[?25l")
            后门口.print("\u001b[2J")

            var 帧字节: Int = pixW * pixH * 4
            var 音频块字节: Int = 采样率 / 30 * 2 * 4   // 一帧(1/30秒)的立体声 f32
            var 帧号: Long = 0
            while (true) {
                var 帧: ByteArray = 读满(视频proc!!.inputStream, 帧字节)
                if (帧.size < 帧字节) break

                // 音频喂食已全权交给喂食线程——主循环曾与它抢同一条流,
                // 两张嘴喝一瓶水,谁也没喝好(2026-11-08 体检拆)

                后门口.print(渲染(帧, pixW, pixH, cols, rows))

                if (!管道喂入) {
                    // 本院没有 kbhit,终端观众请 Ctrl+C 退场或自备耐心
                    // （管道喂入时根本不看键盘,评测无碍）
                }
                ++帧号
                if (帧号 >= maxFrames) break
            }
            后门口.print("\u001b[?25h")
            后门口.print("\u001b[0m\u001b[2J\u001b[H")
        } finally {
            try { if (音频proc != null) 音频proc!!.destroy() } catch (e: Exception) {}
            try { if (视频proc != null) 视频proc!!.destroy() } catch (e: Exception) {}
            if (引擎 != null) {
                try { 引擎!!.全体解散() } catch (e: Exception) { Log("收尾广播无人接收") }
            }
            Log("彩蛋播完,用时请以秒表为准")
        }
    }

    // ProcessBuilder 吃参数列表,路径带空格不用自己加引号——
    // 本院对此很不习惯,方便的东西一律怀疑了三天才敢用。
    // ProcessBuilder 吃参数列表,路径带空格不用自己加引号——
    // 本院引号加习惯了,头一版把引号原样喂给了 ffmpeg,被它告了三帧的状。
    fun 起ffmpeg(ffmpeg: String, 参数: List<String>): Process? {
        return try {
            var pb = ProcessBuilder(listOf(ffmpeg) + 参数)
            pb.redirectOutput(ProcessBuilder.Redirect.PIPE)
            pb.redirectError(ProcessBuilder.Redirect.INHERIT)   // ffmpeg 的报错透传给人类
            pb.start()
        } catch (e: Exception) { null }
    }

    fun 读满(流: java.io.InputStream, 字节数: Int): ByteArray {
        var 全部 = ByteArray(字节数)
        var 已读 = 0
        while (已读 < 字节数) {
            var n = 流.read(全部, 已读, 字节数 - 已读)
            if (n <= 0) break
            已读 += n
        }
        return 全部
    }

    // 真彩 ANSI 半像素渲染：两个像素挤进一个「▀」的上下半。
    fun 渲染(帧: ByteArray, w: Int, h: Int, cols: Int, rows: Int): String {
        var 手 = StringBuilder(w * 4)
        手.append("\u001b[H")
        for (cy in 0 until rows) {
            for (cx in 0 until cols) {
                var x = cx * w / cols
                var yT = cy * 2 * h / (rows * 2)
                var yB = (cy * 2 + 1) * h / (rows * 2)
                if (x >= w) x = w - 1
                if (yT >= h) yT = h - 1
                if (yB >= h) yB = h - 1
                var iT = (yT * w + x) * 4
                var iB = (yB * w + x) * 4
                手.append("\u001b[38;2;").append(帧[iT].toInt() and 0xFF).append(';')
                  .append(帧[iT + 1].toInt() and 0xFF).append(';').append(帧[iT + 2].toInt() and 0xFF).append('m')
                手.append("\u001b[48;2;").append(帧[iB].toInt() and 0xFF).append(';')
                  .append(帧[iB + 1].toInt() and 0xFF).append(';').append(帧[iB + 2].toInt() and 0xFF).append('m')
                手.append("▀")
            }
            手.append("\u001b[0m")
            if (cy + 1 < rows) 手.append("\r\n")
        }
        手.append("\u001b[0m")
        return 手.toString()
    }

    fun 喂食(音频: Process, eng: AudioEngine) {
        try {
            var 流 = 音频.inputStream
            var buf = ByteArray(48000 / 10 * 2 * 4)   // 0.1 秒的立体声 f32
            while (true) {
                var 已读 = 0
                var 断了 = false
                while (已读 < buf.size) {
                    var n = 流.read(buf, 已读, buf.size - 已读)
                    if (n <= 0) { 断了 = true; break }
                    已读 += n
                }
                if (断了) break
                eng.推送(buf.copyOf())   // buf 会被下一轮覆写,推引用等于推幻影(2026-11-08 体检拆)
                Thread.sleep(20)   // 喂完这口喘口气
            }
        } catch (e: Exception) { }
        try { eng.收工() } catch (e: Exception) { }   // 收尾也要兜,背景线程炸一个全队陪葬
    }
}

// ------------------------------ Sonic Pi 桥接 --------------------------------
object 引擎维护处 {
    var 端口: Int = 57110
    var 引擎路径: String = "C:\\Program Files\\Sonic Pi\\app\\server\\native\\Sonic Pi - SuperSonic.exe"
    var 合成器目录: String = "C:\\Program Files\\Sonic Pi\\etc\\synthdefs\\compiled"

    fun 确保运行中(): Boolean {
        // 引擎文件不在 → 立即放弃。曾空转 25 秒干等永远不会来的车(2026-11-08 体检补)
        if (!File(引擎路径).isFile()) { Rr.Log("找不到 Sonic Pi,静音模式继续"); return false }
        if (端口开着(端口)) return true
        try {
            if (File(引擎路径).isFile()) {
                // spawn 出去就不管了，孤儿进程是成长的必经之路
                var pb = ProcessBuilder(listOf(引擎路径, "--tcp", 端口.toString(), "-u", (端口 + 1).toString()))
                pb.redirectOutput(ProcessBuilder.Redirect.DISCARD)
                pb.redirectError(ProcessBuilder.Redirect.DISCARD)
                pb.start()
            }
        } catch (e: Exception) { }
        for (i in 0 until 25 * 4) {
            if (端口开着(端口)) return true
            Thread.sleep(250)
        }
        return 端口开着(端口)
    }

    fun 端口开着(port: Int): Boolean {
        return try {
            var c = Socket("127.0.0.1", port)
            c.close()
            true
        } catch (e: Exception) { false }
    }
}

// OSC over TCP：4 字节大端长度帧 + OSC 包。与 Sonic Pi 的 scsynth 通话。
class AudioEngine {
    var 客户端: Socket? = null
    var 锁 = Object()
    var 队列 = ArrayDeque<ByteArray>()
    var 收工了 = false
    var 缓冲池: Int = 12
    var 节点号: Int = 10000

    fun 初始化(): Boolean {
        return try {
            var c = Socket()
            for (i in 0 until 60) {
                try { c = Socket("127.0.0.1", 引擎维护处.端口); break }
                catch (e: Exception) { Thread.sleep(250) }
            }
            if (!c.isConnected) return false
            客户端 = c
            发送(消息("/d_loadDir").s(引擎维护处.合成器目录))
            Thread.sleep(1200)   // 等 synthdef 装填,本院的等待从不解释
            var 帧 = (48000 * 2.0).toInt()
            for (i in 0 until 缓冲池) {
                发送(消息("/b_alloc").i(100 + i).i(帧).i(2))
            }
            Thread.sleep(400)
            true
        } catch (e: Exception) { false }
    }

    fun 推送(pcm: ByteArray) {
        // notify 必须在 synchronized 里——C# 版当年写在锁外,
        // SynchronizationLockException 在后台线程炸全进程,卷宗编号 0xE0434352
        synchronized(锁) {
            队列.addLast(pcm)
            (锁 as Object).notify()
        }
    }

    fun 收工() {
        synchronized(锁) {
            收工了 = true
            (锁 as Object).notifyAll()
        }
    }

    fun 运行循环() {
        // 全量 try/catch:音频线被老鼠咬断时,绝不能让后台线程的
        // 未处理异常炸掉整个 JVM——评测只看退出码。
        try {
            var 槽 = 0
            while (true) {
                var 块: ByteArray
                synchronized(锁) {
                    while (队列.isEmpty() && !收工了) (锁 as Object).wait()
                    if (队列.isEmpty()) { if (收工了) break }
                    块 = if (队列.isEmpty()) ByteArray(0) else 队列.removeFirst()
                }
                var 帧 = 块.size / 8
                if (帧 == 0) continue
                var 缓冲号 = 100 + 槽
                发送(消息("/b_alloc").i(缓冲号).i(帧).i(2))
                发送(消息("/b_write").i(缓冲号).s("scsynth-buffer").i(0).i(帧 * 2).i(0).blob(块))
                发送(消息("/s_new").s("sonic-pi-basic_stereo_player").i(节点号++).i(0).i(0)
                    .s("buf").i(缓冲号).s("rate").f(1.0f).s("amp").f(0.85f)
                    .s("pan").f(0.0f).s("attack").f(0.0f).s("release").f(0.0f)
                    .s("out_bus").i(0))
                槽 = (槽 + 1) % 缓冲池
            }
        } catch (e: Throwable) {
            Rr.Log("音频线被老鼠咬断了,静音继续")
        }
    }

    fun 全体解散() {
        发送(消息("/g_freeAll").i(0))
        客户端!!.close()
    }

    // ---- OSC 编码。本院手搓大端,java.nio 太舒服,显得不努力 ----
    class Osc消息 {
        var 地址: String = ""
        var 参数 = ArrayList<Pair<Char, Any?>>()   // 'i'/'f'/'s'/'b'
        fun i(v: Int): Osc消息 { 参数.add(Pair('i', v)); return this }
        fun f(v: Float): Osc消息 { 参数.add(Pair('f', v)); return this }
        fun s(v: String): Osc消息 { 参数.add(Pair('s', v)); return this }
        fun blob(v: ByteArray): Osc消息 { 参数.add(Pair('b', v)); return this }
    }

    fun 消息(地址: String): Osc消息 {
        var m = Osc消息()
        m.地址 = 地址
        return m
    }

    fun 发送(m: Osc消息) {
        var 包 = ArrayList<Byte>()
        填充串(包, m.地址)
        var 标签 = ","
        for (p in m.参数) 标签 += p.first
        填充串(包, 标签)
        for (p in m.参数) {
            when (p.first) {
                'i' -> {
                    var v = (p.second as Int)
                    包.add(((v shr 24) and 0xFF).toByte()); 包.add(((v shr 16) and 0xFF).toByte())
                    包.add(((v shr 8) and 0xFF).toByte()); 包.add((v and 0xFF).toByte())
                }
                'f' -> {
                    var v = java.lang.Float.floatToIntBits(p.second as Float)
                    包.add(((v shr 24) and 0xFF).toByte()); 包.add(((v shr 16) and 0xFF).toByte())
                    包.add(((v shr 8) and 0xFF).toByte()); 包.add((v and 0xFF).toByte())
                }
                's' -> 填充串(包, p.second as String)
                'b' -> {
                    var blob = (p.second as ByteArray)
                    var n = blob.size
                    包.add(((n shr 24) and 0xFF).toByte()); 包.add(((n shr 16) and 0xFF).toByte())
                    包.add(((n shr 8) and 0xFF).toByte()); 包.add((n and 0xFF).toByte())
                    for (b in blob) 包.add(b)
                    var 补 = 4 - (n % 4)
                    if (补 != 4) for (i in 0 until 补) 包.add(0)
                }
            }
        }
        var 主体 = 包.toByteArray()
        var 帧 = ByteArray(4 + 主体.size)
        var len = 主体.size
        帧[0] = ((len shr 24) and 0xFF).toByte(); 帧[1] = ((len shr 16) and 0xFF).toByte()
        帧[2] = ((len shr 8) and 0xFF).toByte(); 帧[3] = (len and 0xFF).toByte()
        主体.copyInto(帧, 4)
        synchronized(锁) {
            客户端!!.getOutputStream().write(帧)
            客户端!!.getOutputStream().flush()
        }
    }

    fun 填充串(包: ArrayList<Byte>, s: String) {
        var b = s.toByteArray(Charset.forName("UTF-8"))
        for (x in b) 包.add(x)
        var 补 = 4 - (b.size % 4)
        for (i in 0 until 补) 包.add(0)
    }
}
