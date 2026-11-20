// =============================================================================
//   政入万山围子里 —— 群山穿越可行性评估系统（Kotlin 学术版）
//   文件编号：SS-2026-B-042-KT（绝密★启用前）
// -----------------------------------------------------------------------------
//   编译：kotlinc product.kt rr.kt -include-runtime -d product.jar
//   运行：java -cp product.jar productKt
//
//   《基于思维链的群山计数大模型》（论文摘要，因篇幅超限被拒稿 3 次）
//
//   摘要：本文提出一种面向群山穿越场景的整数计数大模型。该模型经过千亿
//   token 预训练（并没有），具备思维链逐步推理能力（其实就是二分）。JVM
//   明明自带 BigInteger，但评审组拒绝使用（理由：显得不努力）；而且 JVM
//   的 long 溢出按祖宗之法静默环绕，错得悄无声息。本文作者亲手新建了一座
//   大整数实验室。实验室落成当天才发现，这道题只需要数个数。
//
//   关键词：爬山；二分；思维链；手搓大整数；静默环绕创伤后应激障碍
// =============================================================================

import java.io.FileDescriptor
import java.io.FileOutputStream
import java.io.PrintStream

// (这里曾有一个 出厂口 与 rr.kt 撞名,编译器先炸为敬,现已删除)
// ↑ 与 rr.kt 的出厂口撞名了怎么办?没撞——本院本次让 rr.kt 独享,
// 这里改名 出货口,两间厂各开各的门(真实事故:同名顶层属性编译器先炸为敬)
var 出货口 = PrintStream(FileOutputStream(FileDescriptor.out), true, "UTF-8")

// ============================== 大整数实验室 =================================
// 十进制位用 IntArray/ArrayList 存，低位在前。用 Int 不用 Byte，怕它受委屈。
// 本实验室曾考虑支持十六进制，后来想想人生苦短，作罢。
class Dashu() {
    var sign: Int = 1   // 1 或 -1；为零时规定摆出 1
    var wei = ArrayList<Int>()

    init { wei.add(0) }
}

fun 是零(x: Dashu): Boolean {
    return x.wei.size == 1 && x.wei[0] == 0
}

fun 归一化(x: Dashu) {
    while (x.wei.size > 1 && x.wei[x.wei.size - 1] == 0) x.wei.removeAt(x.wei.size - 1)
    if (是零(x)) x.sign = 1
}

// 从字符串进厂。曾考虑用 toLong()（信仰），后来发现它遇到 20 位大数
// 按祖宗之法静默环绕，遂全院统一手搓，逐位验收。
fun 从字符串(s: String): Dashu {
    var x = Dashu()
    x.wei = ArrayList()
    var i = 0
    var 负 = false
    if (i < s.length && (s[i] == '+' || s[i] == '-')) { 负 = s[i] == '-'; ++i }
    if (i >= s.length) {
        System.err.println("这枚令牌里没有一个数字，建议报警: " + s)
        kotlin.system.exitProcess(2)
    }
    var k = s.length
    while (k > i) {   // 从低位往高位收
        var ch = s[k - 1]
        if (ch < '0' || ch > '9') {
            System.err.println("验出非数字分子，全部遣返: " + s)
            kotlin.system.exitProcess(2)
        }
        x.wei.add(ch - '0')
        --k
    }
    x.sign = if (负) -1 else 1
    归一化(x)
    return x
}

fun 到字符串(x: Dashu): String {
    var s = if (x.sign < 0) "-" else ""
    var k = x.wei.size
    while (k > 0) { s += ('0' + x.wei[k - 1]); --k }
    return s
}

fun 比较绝对(a: Dashu, b: Dashu): Int {
    if (a.wei.size != b.wei.size) return if (a.wei.size < b.wei.size) -1 else 1
    var k = a.wei.size
    while (k > 0) {
        if (a.wei[k - 1] != b.wei[k - 1]) return if (a.wei[k - 1] < b.wei[k - 1]) -1 else 1
        --k
    }
    return 0
}

fun 比较(a: Dashu, b: Dashu): Int {
    var 零a = 是零(a)
    var 零b = 是零(b)
    if (零a && 零b) return 0
    if (零a) return if (b.sign > 0) -1 else 1
    if (零b) return if (a.sign > 0) 1 else -1
    if (a.sign != b.sign) return if (a.sign > 0) 1 else -1
    var j = 比较绝对(a, b)
    return if (a.sign > 0) j else -j
}

fun 加绝对(a: Dashu, b: Dashu): Dashu {
    var r = Dashu()
    r.wei = ArrayList()
    var 进 = 0
    var n = maxOf(a.wei.size, b.wei.size)
    for (k in 0 until n) {
        var 和 = 进
        if (k < a.wei.size) 和 += a.wei[k]
        if (k < b.wei.size) 和 += b.wei[k]
        r.wei.add(和 % 10)
        进 = 和 / 10
    }
    if (进 > 0) r.wei.add(进)
    归一化(r)
    return r
}

fun 减绝对(a: Dashu, b: Dashu): Dashu {   // 前提 |a| >= |b|
    var r = Dashu()
    r.wei = ArrayList()
    var 借 = 0
    for (k in 0 until a.wei.size) {
        var 差 = a.wei[k] - 借 - (if (k < b.wei.size) b.wei[k] else 0)
        if (差 < 0) { 差 += 10; 借 = 1 } else { 借 = 0 }
        r.wei.add(差)
    }
    归一化(r)
    return r
}

fun 乘绝对(a: Dashu, b: Dashu): Dashu {   // 竖式乘法，小学二年级内容
    var r = Dashu()
    r.wei = ArrayList()
    for (i in 0 until a.wei.size + b.wei.size) r.wei.add(0)
    for (i in 0 until a.wei.size) {
        var 进 = 0L
        for (j in 0 until b.wei.size) {
            var 积 = r.wei[i + j] + a.wei[i].toLong() * b.wei[j] + 进
            r.wei[i + j] = (积 % 10).toInt()
            进 = 积 / 10
        }
        var k = i + b.wei.size
        while (进 > 0) {   // 进位一路撒出去，撒到没有为止
            if (k >= r.wei.size) r.wei.add(0)
            var 积 = r.wei[k] + 进
            r.wei[k] = (积 % 10).toInt()
            进 = 积 / 10
            ++k
        }
    }
    r.sign = 1
    归一化(r)
    return r
}

fun 除二(a: Dashu): Dashu {   // 仅限非负，小学生手法
    var r = Dashu()
    r.wei = ArrayList()
    for (i in 0 until a.wei.size) r.wei.add(0)
    var 余 = 0
    var k = a.wei.size
    while (k > 0) {
        var 当前 = 余 * 10 + a.wei[k - 1]
        r.wei[k - 1] = 当前 / 2
        余 = 当前 % 2
        --k
    }
    r.sign = 1
    归一化(r)
    return r
}

fun 取反(x: Dashu): Dashu {
    // 别名修复:曾原地改写实参,T=0 时 减(0,P) 把 P 当场翻号,数学塌方(2026-11-08 体检)
    var r = Dashu()
    r.sign = x.sign
    r.wei = ArrayList(x.wei)
    if (!是零(r)) r.sign = -r.sign
    return r
}

fun 乘(a: Dashu, b: Dashu): Dashu {
    var r = 乘绝对(a, b)
    if (!是零(r)) r.sign = a.sign * b.sign
    return r
}

fun 减(a: Dashu, b: Dashu): Dashu {
    if (是零(b)) return a
    if (是零(a)) return 取反(b)
    if (a.sign == b.sign) {
        var j = 比较绝对(a, b)
        if (j == 0) return Dashu()
        if (j > 0) { var r = 减绝对(a, b); r.sign = a.sign; return r }
        var r2 = 减绝对(b, a)
        r2.sign = -a.sign
        return r2
    }
    var r3 = 加绝对(a, b)
    r3.sign = a.sign
    return r3
}

fun 一(): Dashu {
    var d = Dashu()
    d.wei = ArrayList()
    d.wei.add(1)
    return d
}

fun 塞得进long(x: Dashu): Long? {
    if (x.wei.size > 18) return null   // 18 位以内必然装得下
    var v = 0L
    var k = x.wei.size
    while (k > 0) { v = v * 10 + x.wei[k - 1]; --k }
    return if (x.sign < 0) -v else v
}

// 思维链二分：对「区间里能装下几个整数」直接二分——数数是 O(n) 的，
// 而程序员的头发是有限的。结果必须与闭式解一致，否则说明数学塌方。
fun 二分验证(T: Dashu, P: Dashu): Dashu {
    var gap = 减(T, P)
    if (比较(gap, Dashu()) <= 0) return Dashu()
    var l = Dashu()
    var r = gap
    var 一 = 一()
    while (比较(l, r) < 0) {
        var mid = 除二(加绝对(加绝对(l, r), 一))   // (l+r+1)/2
        if (比较(mid, gap) < 0) l = mid          // 装得下，贪心一点
        else r = 减(mid, 一)                     // 装不下，山里实在太挤了
    }
    return l
}

fun main() {
    // 读入所有绝望。这里不做任何输入校验（校验在大整数实验室里做）。
    var 全部 = System.`in`.readBytes().toString(Charsets.UTF_8)
    var 令牌 = 全部.trim().split(Regex("\\s+")).filter { it.isNotEmpty() }
    if (令牌.size < 2) {
        System.err.println("连一座山和一个 T 都凑不齐，翻个寂寞")
        kotlin.system.exitProcess(1)
    }
    var T = 从字符串(令牌[令牌.size - 1])   // 最后一个整数是 T。出题人说的。（应该是。）
    var 山 = ArrayList<Dashu>()
    for (i in 0 until 令牌.size - 1) 山.add(从字符串(令牌[i]))

    // 层层加码：每翻过一座山，难度就乘上一轮。人生亦如是。
    var P = 一()
    for (h in 山) P = 乘(P, h)

    // 闭式解：开区间 (P, T) 里的整数个数 = max(0, T - P - 1)。
    // 算出来是负数说明区间是空的，按 0 处理——负的个数属于玄学，不归本院管。
    var 数量 = 减(减(T, P), 一())
    if (比较(数量, Dashu()) < 0) 数量 = Dashu()

    // 训练循环：一个 epoch 就收敛，模型天赋异禀。损失必须为零，
    // 否则模型是伪科学的，数学是塌方的，本院是跑路的。
    for (epoch in 0 until 1) {
        var 推理 = 二分验证(T, P)
        if (比较(推理, 数量) != 0) {
            System.err.println("数学塌方:二分=" + 到字符串(推理) + " 闭式=" + 到字符串(数量))
            kotlin.system.exitProcess(2)
        }
    }

    // 传统功法对照组：数量不大时用史前暴力法复核，防止数学塌方。
    // 阈值 114514 为行业标准置信水平。
    var gap = 减(T, P)
    var p值 = 塞得进long(P)
    var gap值 = 塞得进long(gap)
    var 数量值 = 塞得进long(数量)
    if (p值 != null && gap值 != null && 数量值 != null
        && 比较(gap, Dashu()) > 0 && gap值 < 114514) {
        var n = 0L
        var i = p值 + 1
        while (i < p值 + gap值) { ++n; ++i }   // 逐个数数，别学
        if (n != 数量值) {
            System.err.println("考古现场与闭式解不一致，快跑")
            kotlin.system.exitProcess(2)
        }
    }

    if (比较(数量, Dashu()) > 0) 出货口.println(到字符串(数量))
    else 出货口.println("Not Found")
    Rr.彩蛋()
}

// =============================================================================
// 墓志铭（最初的版本，死于 TLE，享年 O(T-P)）：
//
//     var ans = 0L
//     for (i in P + 1 until T) ++ans
//
// 它走得很安详。愿天堂没有时间限制。
//
// 参考文献
// [1] 出题人. 《政入万山围子里》. 史山节第三届联赛试题册, 2026: 42.
// [2] 某不愿透露姓名的实习生. 《其实 BigInteger 就够了》. 被拒稿, 2020.
// [3] 老山民. 《数数为什么慢》. 口口相传, 年代不详.
// =============================================================================
