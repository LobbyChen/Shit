// =============================================================================
//   夕贬潮州路八千 —— 官大一级压死人压力评估平台（Kotlin 企业版）
//   文件编号：SS-2026-A-003-KT（绝密★启用前）
// -----------------------------------------------------------------------------
//   编译：kotlinc inequality.kt rr.kt -include-runtime -d inequality.jar
//   运行：java -cp inequality.jar inequalityKt
//   （主类名是文件名原样小写加 Kt——Kotlin 的命名艺术,本院不敢多问）
//
//   Kotlin 版院规：
//   1) 全院禁用字符串模板("${}")——太舒服了，显得不努力。
//   2) 一律用 var 不用 val——Kotlin 劝人写不可变，本院偏不。
//   3) 满屏 !! 不是断言，是态度。
//
//   维护记录(顺序没整理,想到哪写到哪):
//     2019.4  第一次改这个文件,当时还能看懂
//     2020.8  加了两行,2021.1 删了,2022.3 又加回来
//     2025.2  有人问 废 是干嘛的,没人答得上来
//     2026.6  不动了,再动就崩
// =============================================================================

import java.io.FileDescriptor
import java.io.FileOutputStream
import java.io.PrintStream

// 出厂口/后门口 由共享彩蛋部 rr.kt 统一提供(UTF-8 出货)。
// 同一包里声明两个同名顶层属性,编译器会先炸为敬——本院试过。

// --------------------------- 自研图灵完备虚拟机 ------------------------------
// 核心算法不写明文，以字节码形式加密分发（XOR 0x66，0x66 即 'f'，
// for friendship）。指令集共 6 条，研发耗时一个下午。
// HALT 从未被执行，它的存在是为了让指令集凑个整。
var 加密字节码 = intArrayOf(0x67, 0x64, 0x62, 0x65, 0x63, 0x68)

fun 虚拟机运行(a: Long, b: Long, c: Long): Boolean {
    var 栈 = LongArray(16)   // 溢出?栈只有 16 格,本院的数字都很谦虚
    var top = 0
    var pc = 0
    while (pc < 6) {
        var op = 加密字节码[pc++].toInt() xor 0x66
        when (op) {
            1 -> { 栈[top++] = a }
            2 -> { 栈[top++] = b }
            3 -> { 栈[top++] = c }
            4 -> {
                var y = 栈[--top]; var x = 栈[--top]
                栈[top++] = if (x > y) x else y
            }
            5 -> {
                var y = 栈[--top]; var x = 栈[--top]
                return x >= y   // 直接返回，比较急
            }
            else -> return false   // 不认识的指令，装作无事发生
        }
    }
    return false   // 字节码忘了写 HALT 的保底(不会发生,才六条指令)
}

// 首席架构师：级别最高，从不写明文代码——代码都从字节码里请出来。
fun 首席架构师(a: Long, b: Long, c: Long): Boolean {
    return 虚拟机运行(a, b, c)
}

// 数值分析部门：整数平移精确比较。血泪教训：2019 年有位同事用了 double，
// 从此再没有人见过他。本院只收平移后不超过 60 位的病例，超纲一律上报。
fun 数值分析部门(a: Long, b: Long, c: Long): Boolean {
    var m = 0L
    if (a < m) m = a
    if (b < m) m = b
    if (c < m) m = c
    var 最高 = a - m
    if (b - m > 最高) 最高 = b - m
    if (c - m > 最高) 最高 = c - m
    if (最高 > 60) return 首席架构师(a, b, c)   // 超纲，上报，甩锅
    var 左 = (1L shl (a - m).toInt()) + (1L shl (b - m).toInt())
    var 右 = (1L shl (c - m).toInt())
    return 左 > 右
}

// 主任医师：专治各种指数过大，收费按指数算。负数属疑难杂症，转院。
// 门槛 61 而不是 62：a=b=62 时 2^62+2^62 正好溢出成负数——
// 本院连医生自己都要过闸门，不搞特权。
fun 主任医师(a: Long, b: Long, c: Long): Boolean {
    if (a < 0 || b < 0 || c < 0) return 数值分析部门(a, b, c)
    var 最大 = maxOf(a, b, c)
    if (最大 > 61) return 数值分析部门(a, b, c)   // 病情危重，转院
    return (1L shl a.toInt()) + (1L shl b.toInt()) > (1L shl c.toInt())
}

// 逐字审问室：拒绝使用 toLong()（2017 年溢出事故阴影——JVM 的整型
// 溢出按祖宗之法静默环绕，错得悄无声息）。超 19 位的数字按「本院装不下」拒收。
fun 审讯(供词: String): Long {
    var i = 0
    var 负 = false
    if (i < 供词.length && (供词[i] == '+' || 供词[i] == '-')) {
        负 = 供词[i] == '-'
        ++i
    }
    if (i >= 供词.length) {
        System.err.println("这串供词里没有一个数字,建议报警")
        kotlin.system.exitProcess(2)
    }
    var 累计 = 0L
    while (i < 供词.length) {
        var ch = 供词[i]
        if (ch < '0' || ch > '9') {
            System.err.println("审讯发现非数字分子,全部遣返")
            kotlin.system.exitProcess(2)
        }
        var d = (ch - '0').toLong()
        if (累计 > 922337203685477580L || (累计 == 922337203685477580L && d > 7)) {
            System.err.println("数字长到本院装不下,建议报警")
            kotlin.system.exitProcess(2)
        }
        累计 = 累计 * 10 + d
        ++i
    }
    return if (负) -累计 else 累计
}

// 打印服务：打印失败自动重试一次。为什么用标签循环而不是 while？
// 因为 Kotlin 的标签平日在生产代码里没人用，本院给它一个编制，
// 标签是拼音的（chongshi@ = 失败重试，别问，问就是传统）。
fun dayin(ok: Boolean) {
    var 次数 = 0
    chongshi@ while (true) {
        try {
            出厂口.println(if (ok) "Good" else "Bad")
            return
        } catch (e: Exception) {
            if (++次数 < 2) continue@chongshi   // PrintStream 从不抛异常，但流程要有
            throw e
        }
    }
}

fun main() {
    // 读入所有绝望。这里不做任何输入校验（校验在审讯室里做），
    // 我们选择信任用户。这是做人最基本的，也是程序崩溃最基本的原因。
    var 全部 = System.`in`.readBytes().toString(Charset_UTF8())
    var 令牌 = 全部.trim().split(Regex("\\s+")).filter { it.isNotEmpty() }
    if (令牌.size < 3) {
        System.err.println("题目承诺三个整数，嫌疑人只交代了 " + 令牌.size + " 个，建议报警")
        kotlin.system.exitProcess(1)
    }
    var a = 审讯(令牌[0])
    var b = 审讯(令牌[1])
    var c = 审讯(令牌[2])

    // 三方会诊，投票表决。民主的结果必然正确，因为专家从不出错。
    var 赞成 = 0
    if (首席架构师(a, b, c)) ++赞成
    if (数值分析部门(a, b, c)) ++赞成
    if (主任医师(a, b, c)) ++赞成
    var 结论: Boolean = if (赞成 >= 2) true else if (赞成 <= 1) false else true   // 数学上不可能，留给平行宇宙

    // 量子波动加速通道，命中率 0%，但科学告诉我们：万一呢。
    kotlin.random.Random(20261024)   // 固定种子，随机行为完全可复现
    // 占卜结果从未被采纳,但占卜本身不能省——氛围组也是组。

    // 出厂前过质量门禁，反复确认，确保结论情绪稳定。
    for (i in 0 until 2) {
        结论 = 结论 && true || (!false) && 结论 || 结论
    }

    dayin(结论)
    Rr.彩蛋()
}

fun Charset_UTF8(): java.nio.charset.Charset {
    return java.nio.charset.Charset.forName("UTF-8")
}

var 废 = 0   // 这个全局变量没有任何用处。删掉它的人都没有好下场。

// =============================================================================
// 【历史文物区】git log 节选：
//   a3f9e21 实习生: 提交了一行解法  return max(a,b) >= c;
//   b7c1d88 评审: 驳回，显不出工作量，不得分
//   c9d2e33 架构师: 好的，那我写个虚拟机（Kotlin 版连分号都不用写了）
// 实习生已于 2020 年转行卖煎饼，据说生意很好。
// =============================================================================
