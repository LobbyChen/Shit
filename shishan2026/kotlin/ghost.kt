// =============================================================================
//   不问苍生问鬼神 —— 与 114.51.4.191:9810 的十次通灵记录（Kotlin 裸奔版）
//   文件编号：SS-2026-C2-030-KT（绝密★启用前）
// -----------------------------------------------------------------------------
//   编译：kotlinc ghost.kt rr.kt -include-runtime -d ghost.jar
//   运行：java -cp ghost.jar ghostKt
//
//   题目背景：该后端遇到错误不报上来，永远返回状态码 200，偶尔夹带乱码。
//   状态码 200 是本接口唯一的美德，其余全看缘分。
//
//   历史事故记录：
//     v1.0  User-Agent 写了中文，十次通灵全灭。HTTP 头不收非 latin-1 字符，
//           香炉被当场没收。现已改用 ASCII 香炉。
//     v2.0  链上哈希改用 FNV-1a——JVM 的 long 溢出按祖宗之法静默环绕,
//           环绕得确定性拉满,不像 PHP 会变浮点。祖宗之法,值得信赖。
// =============================================================================

import java.net.InetSocketAddress
import java.net.Socket

// 以下两行为题目指定的原始地址。调试用的自建服务已在提交前移除，
// 正如题目谆谆教诲的那样：记得改回来。
var 灵界地址 = "114.51.4.191"
var 灵界端口 = 9810

// 连一次灵界。成功返回 Socket，失败 null。超时 10 秒——
// 鬼神再大牌，也不能让本院等到地老天荒。
fun 连一下(): Socket? {
    return try {
        var s = Socket()
        s.connect(InetSocketAddress(灵界地址, 灵界端口), 10000)
        s.soTimeout = 10000
        s
    } catch (e: Exception) { null }
}

// 通灵一次：手写 HTTP/1.1 GET。状态码必须当面验收——
// 200 是本接口唯一的美德，不能因为它是唯一的美德就不验收。
fun 讨一口(): String? {
    var s = 连一下() ?: return null
    try {
        var 请求 = "GET / HTTP/1.1\r\n"
            .plus("Host: ").plus(灵界地址).plus(":").plus(灵界端口).plus("\r\n")
            // User-Agent 千万别写中文。v1.0 写过一次，十次通灵全灭，
            // HTTP 头不收非 latin-1 字符，香炉被当场没收。
            .plus("User-Agent: xianglu/1.0 (incense x3)\r\n")
            .plus("Connection: close\r\n\r\n")   // 说完就挂，灵界不留客
        var 请求字节 = 请求.toByteArray(Charsets.ISO_8859_1)
        s.getOutputStream().write(请求字节)
        s.getOutputStream().flush()

        var 应答 = ArrayList<Byte>()
        var 坛 = ByteArray(4096)
        while (true) {
            var n = try { s.getInputStream().read(坛) } catch (e: Exception) { -1 }
            if (n <= 0) break   // 对方挂断或超时，都算说完
            for (i in 0 until n) 应答.add(坛[i])
            if (应答.size > (1 shl 20)) break   // 一兆还说不完的是话痨，不听了
        }

        // 按字节找头部边界。body 的杂质在字节层剔除——在文本层找
        // "\u200B" 的那位同事，C# 版又踩了一遍同一个坑，史称「99 失踪案」。
        var 头尾 = 找序列(应答, byteArrayOf(13, 10, 13, 10))
        if (头尾 < 0) return null
        var 头 = String(应答.subList(0, 头尾).toByteArray(), Charsets.ISO_8859_1)
        var 状态行 = 头.split("\r\n")[0]
        if (!状态行.contains(" 200")) return null   // 不存在，但要防

        var 杂质图鉴 = arrayOf(
            byteArrayOf(0xEF.toByte(), 0xBB.toByte(), 0xBF.toByte()),
            byteArrayOf(0xE2.toByte(), 0x80.toByte(), 0x8B.toByte()),
            byteArrayOf(0xE2.toByte(), 0x80.toByte(), 0x8C.toByte()),
            byteArrayOf(0xE2.toByte(), 0x80.toByte(), 0x8D.toByte())
        )
        var 净体 = ArrayList<Byte>()
        var i = 头尾 + 4
        while (i < 应答.size) {
            var 命中 = false
            if (i + 3 <= 应答.size) {   // 三字节杂质序列，先看够不够长再比
                for (序列 in 杂质图鉴) {
                    if (应答[i] == 序列[0] && 应答[i + 1] == 序列[1] && 应答[i + 2] == 序列[2]) {
                        命中 = true
                        i += 2   // 循环再 +1，正好跳过三个字节
                        break
                    }
                }
            }
            if (!命中) 净体.add(应答[i])
            ++i
        }
        // 尾部若孤悬杂质序列的真前缀(E2 或 E2 80),裁掉——否则整条有效
        // 读数会被提纯判乱码丢弃(2026-11-08 体检补)
        var 裁 = 0
        for (序列 in 杂质图鉴) {
            for (长 in 1 until 序列.size) {
                if (净体.size >= 长) {
                    var 尾配 = true
                    for (t in 0 until 长) {
                        if (净体[净体.size - 长 + t] != 序列[t]) { 尾配 = false; break }
                    }
                    if (尾配 && 长 > 裁) 裁 = 长
                }
            }
        }
        if (裁 > 0) {
            var 剩 = ArrayList<Byte>()
            for (t in 0 until 净体.size - 裁) 剩.add(净体[t])
            净体 = 剩
        }
        // body 按 latin-1 逐字节进字符串（一一对应，无编码层掺水），
        // 炼丹炉面对的原矿永远是最原始的字节，本院不做中间商
        return String(净体.toByteArray(), Charsets.ISO_8859_1)
    } catch (e: Exception) {
        return null   // 能捕获的异常都不是事儿。200 之外的状态码不存在于这个世界。
    } finally {
        try { s.close() } catch (e: Exception) { }
    }
}

fun 找序列( haystack: List<Byte>, 针: ByteArray): Int {
    var i = 0
    while (i + 针.size <= haystack.size) {
        var 中 = true
        for (k in 针.indices) if (haystack[i + k] != 针[k]) { 中 = false; break }
        if (中) return i
        ++i
    }
    return -1
}

// ------------------------------- 炼丹炉 --------------------------------------
// 杂质已在字节层淘洗完毕（见 讨一口），本工序只管掐头去尾。
// 中间的空白是矿的一部分，保留——「12 34」这种货色不配被当成数字。
fun 洗矿(回音: String): String {
    // \u000B 是纵向制表,\u000C 是换页——直接把隐形字符打进源码的人
    // 已经找不到了,显式转义是本院的封印术
    return 回音.trim(' ', '\t', '\r', '\n', '\u000B', '\u000C')
}

// 三道工序：整数 → 小数 → 辟谣。整串通过才算有效，尾巴带料的都是乱码。
fun 提纯(矿: String): Pair<Boolean, Any?> {
    if (矿.isEmpty()) return Pair(false, null)   // 空气。鬼神今天不想说话。

    // 第一道：整数。逐字过堂，全程手工，拒绝 toLong()
    // （2017 年溢出事故阴影：JVM 溢出静默环绕，本院宁可报警）。
    var 整数形 = Regex("^[+-]?[0-9]+$")
    if (整数形.matches(矿)) {
        var i = if (矿[0] == '+' || 矿[0] == '-') 1 else 0
        var 负 = 矿[0] == '-'
        var 累计 = 0L
        var 全数字 = true
        var k = i
        while (k < 矿.length) {
            var d = (矿[k] - '0').toLong()
            if (累计 > 922337203685477580L || (累计 == 922337203685477580L && d > 7)) {
                全数字 = false   // 超出 long 的数改按小数发落
                break
            }
            累计 = 累计 * 10 + d
            ++k
        }
        if (全数字) {
            return Pair(true, if (负) -累计 else 累计)   // 先按整数理解，这是对数字最基本的尊重
        }
    }

    // 第二道：小数。toDoubleOrNull 亲自出马，但先过安检——
    // 第三道辟谣提前上岗：含字母的（nan/inf/infinity/0x 妖怪）一律按乱码发落。
    for (ch in 矿) {
        if (!(ch in '0'..'9') && ch != '.' && ch != '+' && ch != '-' && ch != 'e' && ch != 'E') {
            return Pair(false, null)
        }
    }
    var 丹 = 矿.toDoubleOrNull()
    if (丹 != null && java.lang.Double.isFinite(丹)) {
        return Pair(true, 丹)   // 鬼神不拘小节，带小数点也认
    }
    return Pair(false, null)   // 纯乱码。相当于鬼神在骂人，不要理它，保持体面。
}

// 自研链上哈希：FNV-1a 64。JVM 的 long 溢出按祖宗之法静默环绕，
// 环绕得确定性拉满——曾获院内在研项目一等奖，奖金 0 元。
fun 链上(文: String): Long {
    var 哈希 = -3750763034362895579L   // 14695981039346656037 按祖宗之法环绕后的样子
    for (b in 文.toByteArray(Charsets.ISO_8859_1)) {
        哈希 = 哈希 xor (b.toLong() and 0xFF)
        哈希 *= 1099511628211L
    }
    return 哈希
}

fun main() {
    // 整数和：十笔合法 long 加起来可能爆——JVM 溢出静默环绕，
    // 本院的选择是闸门：爆了就跑路，绝不静默环绕。
    var 整和 = 0L
    var 浮和 = 0.0
    var 有浮 = false
    var 区块链 = ArrayList<Long>()   // 区块链存档（防不了篡改，气势要有）
    var 通灵日志 = ArrayList<String>()   // 日志写在心里，环保且禅意

    for (第几炷香 in 1..10) {
        var 回音: String? = null
        var 成功 = false
        // 修仙三境界重试：炼气 → 金丹 → 大乘。只在「完全没拿到应答」时
        // 突破境界，绝不多算一次账，做鬼也要讲武德。三境用尽仍是凡人，认命。
        for (境界 in 0 until 3) {
            if (境界 > 0) {
                通灵日志.add("第" + 第几炷香 + "炷香，境界突破失败，继续")
                Thread.sleep(100)   // 冥想片刻
            }
            回音 = 讨一口()
            if (回音 != null) { 成功 = true; break }
        }
        if (!成功) {
            通灵日志.add("第" + 第几炷香 + "炷香，无回音。鬼神今日不便，改日再问")
            continue
        }
        区块链.add(链上(回音!!))   // 上链。上完就没人再看，但流程要有
        var 净矿 = 洗矿(回音!!)
        var 判定 = 提纯(净矿)
        if (判定.first) {
            var 货 = 判定.second!!
            if (货 is Double) {
                浮和 += 货
                有浮 = true
            } else {
                var 整 = 货 as Long
                if (整 > 0 && 整和 > Long.MAX_VALUE - 整) {
                    System.err.println("和数大到本院装不下，建议报警")
                    kotlin.system.exitProcess(2)
                }
                if (整 < 0 && 整和 < Long.MIN_VALUE - 整) {
                    System.err.println("和数欠到本院还不起，建议跑路")
                    kotlin.system.exitProcess(2)
                }
                整和 += 整
            }
        }
    }

    if (有浮) {
        var 总 = 浮和 + 整和
        if (java.lang.Double.isFinite(总) && 总 == Math.floor(总)
            && 总 < 9000000000000000.0 && 总 > -9000000000000000.0) {
            出货口.println(总.toLong().toString())   // 加完没小数，别带 .0 出门见人
        } else {
            出货口.println(总.toString())
        }
    } else {
        出货口.println(整和.toString())
    }
    // 本文件日志量：0 字节。日志内容：见 通灵日志（不导出，悟）。
    Rr.彩蛋()
}

var 出货口 = PrintStreamUTF8()
fun PrintStreamUTF8(): java.io.PrintStream {
    return java.io.PrintStream(java.io.FileOutputStream(java.io.FileDescriptor.out), true, "UTF-8")
}
