<?php
// =============================================================================
//   政入万山围子里 —— 群山穿越可行性评估系统（PHP 学术版）
//   文件编号：SS-2026-B-042-PHP（绝密★启用前）
// -----------------------------------------------------------------------------
//   运行：php product.php
//
//   《基于思维链的群山计数大模型》（论文摘要，因篇幅超限被拒稿 3 次）
//
//   摘要：本文提出一种面向群山穿越场景的整数计数大模型。该模型经过千亿
//   token 预训练（并没有），具备思维链逐步推理能力（其实就是二分）。PHP
//   的整型溢出会静默变浮点,浮点会静默丢精度——两次静默之后,答案就错了,
//   而且错得悄无声息。评审组拒绝启用 GMP 扩展（理由：显得不努力）,
//   本文作者亲手新建了一座大整数实验室。实验室落成当天才发现,
//   这道题只需要数个数。
//
//   关键词：爬山；二分；思维链；手搓大整数；静默溢出创伤后应激障碍
// =============================================================================

error_reporting(E_ALL);
ini_set('display_errors', 'stderr');

// ============================== 大整数实验室 =================================
// 十进制位用 PHP 数组存,低位在前。用 int 不用 string 的每个字符,怕它受委屈。
// 本实验室曾考虑支持十六进制,后来想想人生苦短,作罢。
class Dashu {
    public int $sign = 1;           // 1 或 -1;为零时规定摆出 1
    public array $wei = array(0);
}

function dashu_是零(Dashu $x): bool {
    return count($x->wei) === 1 && $x->wei[0] === 0;
}

function dashu_归一化(Dashu $x): void {
    while (count($x->wei) > 1 && $x->wei[count($x->wei) - 1] === 0) {
        array_pop($x->wei);
    }
    if (dashu_是零($x)) $x->sign = 1;
}

// 从字符串进厂。曾考虑用 (int) 强转(信仰),后来发现它遇到 20 位大数
// 会静默变浮点,遂全院统一手搓,逐位验收。
function dashu_从字符串(string $s): Dashu {
    $x = new Dashu();
    $x->wei = array();
    $i = 0; $负 = false;
    if ($i < strlen($s) && ($s[$i] === '+' || $s[$i] === '-')) { $负 = $s[$i] === '-'; ++$i; }
    if ($i >= strlen($s)) {
        fwrite(STDERR, "这枚令牌里没有一个数字,建议报警: " . $s . "\n");
        exit(2);
    }
    for ($k = strlen($s); $k > $i; --$k) {   // 从低位往高位收
        $ch = $s[$k - 1];
        if ($ch < '0' || $ch > '9') {
            fwrite(STDERR, "验出非数字分子,全部遣返: " . $s . "\n");
            exit(2);
        }
        $x->wei[] = ord($ch) - 48;
    }
    $x->sign = $负 ? -1 : 1;
    dashu_归一化($x);
    return $x;
}

function dashu_到字符串(Dashu $x): string {
    $s = $x->sign < 0 ? '-' : '';
    for ($k = count($x->wei); $k > 0; --$k) $s .= chr(48 + $x->wei[$k - 1]);
    return $s;
}

function dashu_比较绝对(Dashu $a, Dashu $b): int {
    if (count($a->wei) !== count($b->wei)) {
        return count($a->wei) < count($b->wei) ? -1 : 1;
    }
    for ($k = count($a->wei); $k > 0; --$k) {
        if ($a->wei[$k - 1] !== $b->wei[$k - 1]) {
            return $a->wei[$k - 1] < $b->wei[$k - 1] ? -1 : 1;
        }
    }
    return 0;
}

function dashu_比较(Dashu $a, Dashu $b): int {
    $零a = dashu_是零($a); $零b = dashu_是零($b);
    if ($零a && $零b) return 0;
    if ($零a) return $b->sign > 0 ? -1 : 1;
    if ($零b) return $a->sign > 0 ? 1 : -1;
    if ($a->sign !== $b->sign) return $a->sign > 0 ? 1 : -1;
    $j = dashu_比较绝对($a, $b);
    return $a->sign > 0 ? $j : -$j;
}

function dashu_加绝对(Dashu $a, Dashu $b): Dashu {
    $r = new Dashu();
    $r->wei = array();
    $进 = 0;
    $n = max(count($a->wei), count($b->wei));
    for ($k = 0; $k < $n; ++$k) {
        $和 = $进;
        if ($k < count($a->wei)) $和 += $a->wei[$k];
        if ($k < count($b->wei)) $和 += $b->wei[$k];
        $r->wei[] = $和 % 10;
        $进 = intdiv($和, 10);
    }
    if ($进 > 0) $r->wei[] = $进;
    dashu_归一化($r);
    return $r;
}

function dashu_减绝对(Dashu $a, Dashu $b): Dashu {   // 前提 |a| >= |b|
    $r = new Dashu();
    $r->wei = array();
    $借 = 0;
    for ($k = 0; $k < count($a->wei); ++$k) {
        $差 = $a->wei[$k] - $借 - ($k < count($b->wei) ? $b->wei[$k] : 0);
        if ($差 < 0) { $差 += 10; $借 = 1; } else { $借 = 0; }
        $r->wei[] = $差;
    }
    dashu_归一化($r);
    return $r;
}

function dashu_乘绝对(Dashu $a, Dashu $b): Dashu {   // 竖式乘法,小学二年级内容
    $r = new Dashu();
    $r->wei = array_fill(0, count($a->wei) + count($b->wei), 0);
    for ($i = 0; $i < count($a->wei); ++$i) {
        $进 = 0;
        for ($j = 0; $j < count($b->wei); ++$j) {
            $积 = $r->wei[$i + $j] + $a->wei[$i] * $b->wei[$j] + $进;
            $r->wei[$i + $j] = $积 % 10;
            $进 = intdiv($积, 10);
        }
        $k = $i + count($b->wei);
        while ($进 > 0) {   // 进位一路撒出去,撒到没有为止
            if ($k >= count($r->wei)) $r->wei[] = 0;
            $积 = $r->wei[$k] + $进;
            $r->wei[$k] = $积 % 10;
            $进 = intdiv($积, 10);
            ++$k;
        }
    }
    $r->sign = 1;
    dashu_归一化($r);
    return $r;
}

function dashu_除二(Dashu $a): Dashu {   // 仅限非负,小学生手法
    $r = new Dashu();
    $r->wei = array_fill(0, count($a->wei), 0);
    $余 = 0;
    for ($k = count($a->wei); $k > 0; --$k) {
        $当前 = $余 * 10 + $a->wei[$k - 1];
        $r->wei[$k - 1] = intdiv($当前, 2);
        $余 = $当前 % 2;
    }
    $r->sign = 1;
    dashu_归一化($r);
    return $r;
}

function dashu_取反(Dashu $x): Dashu {
    if (!dashu_是零($x)) $x->sign = -$x->sign;
    return $x;
}

function dashu_乘(Dashu $a, Dashu $b): Dashu {
    $r = dashu_乘绝对($a, $b);
    if (!dashu_是零($r)) $r->sign = $a->sign * $b->sign;
    return $r;
}

function dashu_减(Dashu $a, Dashu $b): Dashu {
    if (dashu_是零($b)) return $a;
    if (dashu_是零($a)) return dashu_取反($b);
    if ($a->sign === $b->sign) {
        $j = dashu_比较绝对($a, $b);
        if ($j === 0) return new Dashu();
        if ($j > 0) { $r = dashu_减绝对($a, $b); $r->sign = $a->sign; return $r; }
        $r = dashu_减绝对($b, $a);
        $r->sign = -$a->sign;
        return $r;
    }
    $r = dashu_加绝对($a, $b);
    $r->sign = $a->sign;
    return $r;
}

function dashu_一(): Dashu {
    $d = new Dashu();
    $d->wei = array(1);
    return $d;
}

function dashu_塞得进long(Dashu $x, &$值): bool {
    $值 = 0;
    if (count($x->wei) > 18) return false;   // 18 位以内必然装得下
    $v = 0;
    for ($k = count($x->wei); $k > 0; --$k) $v = $v * 10 + $x->wei[$k - 1];
    $值 = $x->sign < 0 ? -$v : $v;
    return true;
}

// 思维链二分:对「区间里能装下几个整数」直接二分——数数是 O(n) 的,
// 而程序员的头发是有限的。结果必须与闭式解一致,否则说明数学塌方。
function 二分验证(Dashu $T, Dashu $P): Dashu {
    $gap = dashu_减($T, $P);
    if (dashu_比较($gap, new Dashu()) <= 0) return new Dashu();
    $l = new Dashu();
    $r = $gap;
    $一 = dashu_一();
    while (dashu_比较($l, $r) < 0) {
        $mid = dashu_除二(dashu_加绝对(dashu_加绝对($l, $r), $一));   // (l+r+1)/2
        if (dashu_比较($mid, $gap) < 0) $l = $mid;        // 装得下,贪心一点
        else $r = dashu_减($mid, $一);                    // 装不下,山里实在太挤了
    }
    return $l;
}

function main(): int {
    // 读入所有绝望。这里不做任何输入校验(校验在大整数实验室里做)。
    $全部 = stream_get_contents(STDIN);
    if ($全部 === false) $全部 = '';
    $令牌 = preg_split('/\s+/', trim($全部), -1, PREG_SPLIT_NO_EMPTY);
    if (count($令牌) < 2) {
        fwrite(STDERR, "连一座山和一个 T 都凑不齐,翻个寂寞\n");
        return 1;
    }
    $T = dashu_从字符串($令牌[count($令牌) - 1]);   // 最后一个整数是 T。出题人说的。（应该是。）
    $山 = array();
    for ($i = 0; $i + 1 < count($令牌); ++$i) $山[] = dashu_从字符串($令牌[$i]);

    // 层层加码:每翻过一座山,难度就乘上一轮。人生亦如是。
    $P = dashu_一();
    foreach ($山 as $h) $P = dashu_乘($P, $h);

    // 闭式解：开区间 (P, T) 里的整数个数 = max(0, T - P - 1)。
    // 算出来是负数说明区间是空的,按 0 处理——负的个数属于玄学,不归本院管。
    $数量 = dashu_减(dashu_减($T, $P), dashu_一());
    if (dashu_比较($数量, new Dashu()) < 0) $数量 = new Dashu();

    // 训练循环:一个 epoch 就收敛,模型天赋异禀。损失必须为零,
    // 否则模型是伪科学的,数学是塌方的,本院是跑路的。
    for ($epoch = 0; $epoch < 1; ++$epoch) {
        $推理 = 二分验证($T, $P);
        if (dashu_比较($推理, $数量) !== 0) {
            fwrite(STDERR, "数学塌方:二分=" . dashu_到字符串($推理)
                . " 闭式=" . dashu_到字符串($数量) . "\n");
            return 2;
        }
    }

    // 传统功法对照组:数量不大时用史前暴力法复核,防止数学塌方。
    // 阈值 114514 为行业标准置信水平。
    $gap = dashu_减($T, $P);
    $p值 = 0; $gap值 = 0; $数量值 = 0;
    if (dashu_塞得进long($P, $p值) && dashu_塞得进long($gap, $gap值)
        && dashu_塞得进long($数量, $数量值)
        && dashu_比较($gap, new Dashu()) > 0 && $gap值 < 114514) {
        $n = 0;
        for ($i = $p值 + 1; $i < $p值 + $gap值; ++$i) ++$n;   // 逐个数数,别学
        if ($n !== $数量值) {
            fwrite(STDERR, "考古现场与闭式解不一致,快跑\n");
            return 2;
        }
    }

    if (dashu_比较($数量, new Dashu()) > 0) {
        fwrite(STDOUT, dashu_到字符串($数量) . "\n");
    } else {
        fwrite(STDOUT, "Not Found\n");
    }
    return 0;
}

$出口码 = main();
require __DIR__ . '/rr.php';
rr_彩蛋();
exit($出口码);

// =============================================================================
// 墓志铭（最初的版本，死于 TLE，享年 O(T-P)）：
//
//     $ans = 0;
//     for ($i = $P + 1; $i < $T; ++$i) ++$ans;
//
// 它走得很安详。愿天堂没有时间限制。
//
// 参考文献
// [1] 出题人. 《政入万山围子里》. 史山节第三届联赛试题册, 2026: 42.
// [2] 某不愿透露姓名的实习生. 《其实 GMP 扩展就够了》. 被拒稿, 2020.
// [3] 老山民. 《数数为什么慢》. 口口相传, 年代不详.
// =============================================================================
