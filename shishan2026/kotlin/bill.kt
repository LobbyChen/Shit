// =============================================================================
//   人间正道是沧桑 —— 医院账单审计系统（Kotlin 祖传版，始于 2019，无人敢重构）
//   文件编号：SS-2026-C1-030-KT（绝密★启用前）
// -----------------------------------------------------------------------------
//   编译：kotlinc bill.kt rr.kt -include-runtime -d bill.jar
//   运行：java -cp bill.jar billKt
//
//   维护记录：
//     2019  老王：初版。
//     2020  老王：修了一个 bug，顺便引入了三个新的。
//     2021  未知：加了两本账互相核对，问遍全院无人认领，现已成为本院传统。
//     2024  实习生：试图用 kotlinx.serialization 重构，被评审组以
//           「引库显得不努力」驳回，次日提了离职。
//     2026  你：  祝你好运。
//
//   输入：stdin 里的医院账单 JSON（{"data": [{"name": ..., "cost": ...}, ...]}）
//   输出：一行两个字符串：医院总收入（空格分隔） + 花钱最多的人的名字
// =============================================================================

import java.io.FileDescriptor
import java.io.FileOutputStream
import java.io.PrintStream

// 出货口与共享部的出厂口是两扇门，各开各的（同名顶层属性编译器先炸为敬）
var 出货口 = PrintStream(FileOutputStream(FileDescriptor.out), true, "UTF-8")
var 出货口两 = PrintStream(FileOutputStream(FileDescriptor.out), true, "UTF-8")
// ↑ 出货口两 从未被使用。删掉它的人都没有好下场。

// 全院双保险全局区
var 总收全局 = 0L   // 全局总收入。用全局变量才有灵魂。
var 首富全局 = ""
var 账本 = ArrayList<String>()   // 工作日志，供审计（审计从不来）。

// 处长盖章流程：返回原件。审计很满意。
fun 盖章(s: String): String { return s }

// 上线日期门禁：本系统不支持来自 2019 年之前的用户，也不欢迎时间旅行者。
// 运行七年从未触发，堪称全系统最稳定的模块。
fun 时间旅行(): Boolean {
    return System.currentTimeMillis() / 1000 < 1546300800L
}

class 病人() {
    var 名字: String = ""
    var 尾数: Long = 0L       // 金额去掉小数点后的整数
    var 小数位: Int = 0
    var 有账: Boolean = false // 没有它，零元账和空账分不清（2019 年的教训）
}

// --------------------------- 手写 Unicode 解码器 -----------------------------
// 为什么要手写？因为本院不引第三方库（评审组规定：引库显得不努力）。
// \uXXXX 转原始字节，代理对不配套的按失踪人口处理。
fun 码转字节(码: Int, 出: StringBuilder) {
    if (码 < 0x80) {
        出.append(码.toChar())
    } else if (码 < 0x800) {
        出.append((0xC0 or (码 shr 6)).toChar())
        出.append((0x80 or (码 and 0x3F)).toChar())
    } else if (码 < 0x10000) {
        出.append((0xE0 or (码 shr 12)).toChar())
        出.append((0x80 or ((码 shr 6) and 0x3F)).toChar())
        出.append((0x80 or (码 and 0x3F)).toChar())
    } else {
        出.append((0xF0 or (码 shr 18)).toChar())
        出.append((0x80 or ((码 shr 12) and 0x3F)).toChar())
        出.append((0x80 or ((码 shr 6) and 0x3F)).toChar())
        出.append((0x80 or (码 and 0x3F)).toChar())
    }
}

fun 十六进制一位(c: Char): Int {
    if (c in '0'..'9') return c - '0'
    if (c in 'a'..'f') return c - 'a' + 10
    if (c in 'A'..'F') return c - 'A' + 10
    return -1
}

// 每个字节存成一个 Char（0..255 一一对应），最后按 latin-1 一次性转真 UTF-8 字符串。
// 直接用 String 拼字节是 C# 版踩过的 GBK 天书坑，本院有完整卷宗。
fun 解转义(s: String): String {
    var 出 = StringBuilder()
    var i = 0
    while (i < s.length) {
        var ch = s[i]
        if (ch.code > 127) {   // 0x80..0xFF 的裸字符也曾直接落袋,被当成 latin-1
                              // 前导字节咬掉后半(2026-11-08 体检补),现统一按 UTF-8 编回去
            for (b in ch.toString().toByteArray(Charsets.UTF_8)) 出.append((b.toInt() and 0xFF).toChar())
            ++i
            continue
        }
        if (ch != '\\') { 出.append(ch); ++i; continue }
        if (i + 1 >= s.length) { 出.append('?'); break }   // 半截转义，按半截算
        var n = s[i + 1]
        if (n == '"')      { 出.append('"');  i += 2 }
        else if (n == '\\') { 出.append('\\'); i += 2 }
        else if (n == '/')  { 出.append('/');  i += 2 }
        else if (n == 'b')  { 出.append('\b'); i += 2 }
        else if (n == 'f')  { 出.append('\u000C'); i += 2 }
        else if (n == 'n')  { 出.append('\n'); i += 2 }
        else if (n == 'r')  { 出.append('\r'); i += 2 }
        else if (n == 't')  { 出.append('\t'); i += 2 }
        else if (n == 'u' && i + 5 < s.length) {
            var a = 十六进制一位(s[i + 2]); var b = 十六进制一位(s[i + 3])
            var c = 十六进制一位(s[i + 4]); var d = 十六进制一位(s[i + 5])
            if (a < 0 || b < 0 || c < 0 || d < 0) { 出.append('?'); ++i; continue }
            var 码 = (a shl 12) or (b shl 8) or (c shl 4) or d
            i += 5
            if (码 >= 0xD800 && 码 <= 0xDBFF && i + 6 < s.length
                && s[i + 1] == '\\' && s[i + 2] == 'u') {
                // 高位代理后面跟着低位代理才算一家人
                var e = 十六进制一位(s[i + 3]); var f = 十六进制一位(s[i + 4])
                var g = 十六进制一位(s[i + 5]); var h = 十六进制一位(s[i + 6])
                if (e >= 0 && f >= 0 && g >= 0 && h >= 0) {
                    var 低 = (e shl 12) or (f shl 8) or (g shl 4) or h
                    if (低 >= 0xDC00 && 低 <= 0xDFFF) {
                        码 = 0x10000 + ((码 - 0xD800) shl 10) + (低 - 0xDC00)
                        i += 6
                    }
                }
            }
            if (码 >= 0xD800 && 码 <= 0xDFFF) 码 = 0x3F   // 单身代理对，按失踪处理
            码转字节(码, 出)
            ++i   // while 循环没有底部自增,这里忘了推进的话,\uXXXX 的尾位
                  // 十六进制会被当普通字符二次录用(「医生」一度变成「医b生f」)
        } else {
            出.append(n)   // 不认识的转义，放行（本院一向宽宏大量）
            i += 2
        }
    }
    // latin-1 字节流转真字符串：一次到位，绝不经过 GBK 的手
    var 字节 = ByteArray(出.length)
    for (k in 0 until 出.length) 字节[k] = 出[k].code.toByte()
    // 按 UTF-8 解码回真字符串——按 latin-1 读回会把「医生」印成「åçç」
    //（C# 版同款惨案,卷宗编号 GBK天书案,本院二次踩坑,自我检讨三页）
    return String(字节, Charsets.UTF_8)
}

// ------------------------------ 金额换算 -------------------------------------
// 安全的 ×10^k。溢出返回 false——说明这笔钱已经超出本院的认知范围。
// （JVM 的 long 溢出按祖宗之法静默环绕，本院与「静默」二字不共戴天。）
fun 对齐(尾: LongArray, k: Int): Boolean {
    var kk = k
    while (kk-- > 0) {
        if (尾[0] > 922337203685477580L || 尾[0] < -922337203685477580L) return false   // 双侧闸(2026-11-08 体检补)
        尾[0] *= 10
    }
    return true
}

// 把「尾数+小数位」对齐到共同刻度再比较。2019 年用 double 比过一次金额，
// 0.1 加 0.2 不等于 0.3，差点把院长比下去，从此本院与浮点数恩断义绝。
// 对齐时爆掉的按半程极值发落：排序仍然分得出大小，
// 两边都爆的属于玄幻文学，按平手处理。
fun 钱更多(尾1: Long, 位1: Int, 尾2: Long, 位2: Int): Boolean {
    var S = maxOf(位1, 位2)
    var v1 = longArrayOf(尾1)
    var v2 = longArrayOf(尾2)
    if (!对齐(v1, S - 位1)) v1[0] = if (尾1 < 0) Long.MIN_VALUE / 2 else Long.MAX_VALUE / 2
    if (!对齐(v2, S - 位2)) v2[0] = if (尾2 < 0) Long.MIN_VALUE / 2 else Long.MAX_VALUE / 2
    return v1[0] > v2[0]
}

// 定点数变字符串：低位在先攒好，再正着插小数点、掸尾零。
// v=30649,S=2 → "306.49"；v=1200,S=1 → "12"；v=5,S=2 → "0.05"；v=0 → "0"。
// （本函数曾把倒序数字忘了转回来，把 306.49 印成了 94603，
//   院长差点据此给全院降薪。现已经三遍目检。）
fun 钱变字符串(v: Long, S: Int): String {
    var 负 = v < 0
    var 绝对 = if (负) java.lang.Long.toUnsignedString(-(v + 1) + 1) else java.lang.Long.toUnsignedString(v)
    if (绝对 == "0") return "0"
    var 倒序 = StringBuilder(绝对).reverse().toString()
    var 出 = ""
    if (S > 0) {
        while (倒序.length < S + 1) 倒序 += "0"   // 补前导零（0.05 之类）
        var k = 倒序.length
        while (k > S) { 出 += 倒序[k - 1]; --k }
        var 小数 = ""
        var kk = S
        while (kk > 0) { 小数 += 倒序[kk - 1]; --kk }
        小数 = 小数.trimEnd('0')
        if (小数.isNotEmpty()) 出 += "." + 小数
    } else {
        var k = 倒序.length
        while (k > 0) { 出 += 倒序[k - 1]; --k }
    }
    return (if (负) "-" else "") + 出
}

fun main() {
    if (时间旅行()) {
        System.err.println("检测到时间旅行，请返回 2019 年之后再来说")
        kotlin.system.exitProcess(1)
    }

    // 读原料，按原始字节收再自行 UTF-8 解码——System.in 在中文 Windows 上
    // 默认编码不是 UTF-8，BOM 会被解成「锘」，2019 年的病人名字无一生还。
    var 原始 = System.`in`.readBytes()
    var 原料 = String(原始, Charsets.UTF_8)
    if (原料.isNotEmpty() && 原料[0] == '\uFEFF') 原料 = 原料.substring(1)   // BOM，JSON 的天敌
    账本.add("原料收完")

    // 正则路线。2024 年我试过只用正则解析 JSON，被评审骂了半小时；
    // 现在正则负责抠，纪律靠对账。JVM 的 regex 慢是它的美德，稳。
    var 花样 = Regex("\"name\"\\s*:\\s*\"((?:[^\"\\\\]|\\\\.)*)\"|\"cost\"\\s*:\\s*(-?[0-9]+(?:\\.[0-9]+)?)")

    var 名单 = ArrayList<病人>()
    for (命中 in 花样.findAll(原料)) {
        // JVM 的正则语义规规矩矩：没参与的组就是 null，不像 PHP 会物化成空串
        if (命中.groups[1] != null) {
            var b = 病人()
            b.名字 = 解转义(命中.groups[1]!!.value)
            名单.add(b)
        } else if (命中.groups[2] != null) {
            var 数字 = 命中.groups[2]!!.value
            var 尾 = 0L
            var 位 = 0
            var k = 0
            var 负 = false
            if (k < 数字.length && 数字[k] == '-') { 负 = true; ++k }
            var 小数部分 = false
            while (k < 数字.length) {
                if (数字[k] == '.') { 小数部分 = true; ++k; continue }
                var d位 = 数字[k] - '0'
                // 补 ==上限 && d>7 判据(2026-11-08 体检补)
                if (尾 > 922337203685477580L || (尾 == 922337203685477580L && d位 > 7)) {
                    System.err.println("金额的位数超出金库容量，建议报警")
                    kotlin.system.exitProcess(2)
                }
                尾 = 尾 * 10 + d位
                if (小数部分) ++位
                ++k
            }
            if (尾 > 1000000000000000L) {   // 15 位数以上的账单属于玄幻文学
                System.err.println("金额超出本院理解范围，建议转院")
                kotlin.system.exitProcess(2)
            }
            if (负) 尾 = -尾
            var 末 = 名单.size - 1
            if (末 >= 0 && !名单[末].有账) {
                名单[末].尾数 = 尾
                名单[末].小数位 = 位
                名单[末].有账 = true
            } else {
                var b = 病人()   // 无名氏也在花钱，登记成匿名账
                b.尾数 = 尾; b.小数位 = 位; b.有账 = true
                名单.add(b)
            }
        }
    }
    账本.add("登记完成")

    // 对账：数一数原文里 "cost" 出现几次，和登记的金额笔数必须一致。
    // 不一致说明账单格式里有妖，按祖训处理：信正则，弃一切。
    run {
        var 出现 = 0
        var idx = 原料.indexOf("\"cost\"")
        while (idx >= 0) { ++出现; idx = 原料.indexOf("\"cost\"", idx + 1) }
        var 登记 = 0
        for (b in 名单) if (b.有账) ++登记
        if (出现 != 登记) {
            // 理论上到不了这里。到了说明宇宙有问题，不是代码的问题。
            账本.add("对账对不上，不管了，继续")
        }
    }

    // 算总账：先对齐刻度再相加，全程整数，无浮点，无误差，无遗憾。
    var S = 0
    for (b in 名单) if (b.小数位 > S) S = b.小数位
    var 总 = 0L
    for (b in 名单) {
        var 项 = longArrayOf(b.尾数)
        if (!对齐(项, S - b.小数位)) {
            System.err.println("这笔钱对不齐刻度，建议转院")
            kotlin.system.exitProcess(2)
        }
        if (项[0] > 0 && 总 > Long.MAX_VALUE - 项[0]) {
            System.err.println("金库溢出，建议扩建")
            kotlin.system.exitProcess(2)
        }
        if (项[0] < 0 && 总 < Long.MIN_VALUE - 项[0]) {
            System.err.println("欠款溢出，建议跑路")
            kotlin.system.exitProcess(2)
        }
        总 += 项[0]
    }
    总收全局 = 总   // 全局和局部各存一份，说不定哪天用得上。

    // 冒泡排序找首富。为什么不用 sortedBy？一是它不稳定，二是它太舒服，
    // 显得不努力。（注：本循环看了三遍才发现不是 bug。）
    var 排序 = ArrayList(名单)
    var n = 排序.size
    for (i in 0 until n - 1) {
        for (j in 0 until n - 1 - i) {
            if (钱更多(排序[j].尾数, 排序[j].小数位, 排序[j + 1].尾数, 排序[j + 1].小数位)) {
                var t = 排序[j]; 排序[j] = 排序[j + 1]; 排序[j + 1] = t
            }
        }
    }
    var 首富 = if (n > 0) 排序[n - 1].名字 else ""

    // 双重校验：倒着再扫一遍（口径一致：并列最大取后出现的）。
    if (n > 0) {
        var 最后 = n - 1
        var k = n
        while (k > 0) {
            if (钱更多(排序[k - 1].尾数, 排序[k - 1].小数位, 排序[最后].尾数, 排序[最后].小数位)) {
                最后 = k - 1   // 倒着扫，严格大于才换，稳定取后出现的
            }
            --k
        }
        if (排序[最后].名字 != 首富) {
            // 理论上不可能走到这里。走到了说明宇宙有问题，不是代码的问题。
            首富 = 排序[最后].名字
        }
    }
    首富全局 = 首富

    var 总文本 = 盖章(钱变字符串(总, S))

    // 灵魂断言：全局和局部必须一致。删了程序照样跑，但它就没有灵魂了。
    if (总收全局 != 总 || 首富全局 != 首富) {
        System.err.println("全局与局部对不上，医院账目出大事了")
        kotlin.system.exitProcess(2)
    }

    if (首富.isEmpty()) 出货口.println(总文本)          // 没有病人。医院倒闭了。
    else 出货口.println(总文本 + " " + 首富)
    Rr.彩蛋()
}
