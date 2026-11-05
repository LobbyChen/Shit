# -*- coding: utf-8 -*-
# =============================================================================
#   一键自检：把屎山挨个编译、挨个跑、逐用例对答案。
#   用法：python 自检.py
#   - 优先用 _toolchain/cl_env.bat（MSVC），其次找 g++ / clang++；
#   - C++ 版是主测对象；python旧版备份/ 里的 Python 版若存在，也顺带回归；
#   - C2 连不上假地址（那本来就是假地址），所以起一个冒牌服务器冒充灵界，
#     生成临时副本指过去，测完即删。
# =============================================================================
import os
import sys
import shutil
import threading
import http.server
import socketserver
import subprocess

这里 = os.path.dirname(os.path.abspath(__file__))
PY = sys.executable
环境 = dict(os.environ, PYTHONIOENCODING="utf-8", RR_DURATION="1")
# ↑ RR_DURATION=1:不给的话,万一评测机上真有 ffmpeg 和视频,
#   每个用例都要先看 5 秒彩蛋,41 个用例就是三分半,评委等到睡着。
构建 = os.path.join(这里, "_build")

通过数 = 0
失败数 = 0


def 跑命令(命令, 标准输入="", 超时=120):
    try:
        结果 = subprocess.run(命令, input=标准输入, capture_output=True, text=True,
                              encoding="utf-8", errors="replace", timeout=超时, env=环境)
    except subprocess.TimeoutExpired:
        return "", -1, "超时 %d 秒，程序可能陷入冥想" % 超时
    return 结果.stdout.strip(), 结果.returncode, 结果.stderr


def 报结果(标签, 实际, 期望, 附加=""):
    global 通过数, 失败数
    if 实际 == 期望:
        通过数 += 1
        print("[PASS] %-10s %s" % (标签, 附加))
    else:
        失败数 += 1
        print("[FAIL] %-10s 期望=%r 实际=%r %s" % (标签, 期望, 实际, 附加))


# ------------------------------- 编译器探测 ----------------------------------
def 找编译器():
    蝙蝠 = os.path.join(这里, "_toolchain", "cl_env.bat")
    if os.path.exists(蝙蝠):
        return "msvc", 蝙蝠
    for 名 in ("g++", "clang++"):
        路径 = shutil.which(名)
        if 路径:
            return 名, None
    return None, None


编译器, 蝙蝠 = 找编译器()


def 编译(cpp, exe):
    if 编译器 == "msvc":
        cmd = ["cmd", "/c", 蝙蝠, "/Fe:" + exe, "/Fo:" + 构建 + os.sep]
    else:
        cmd = [编译器, "-O2", "-std=c++11", "-w", "-o", exe]
    cmd.append(cpp)
    # MSVC 的报错是 GBK 的，不写 errors=replace 会把解码线程噎死（真的噎死过）
    try:
        r = subprocess.run(cmd, cwd=这里, capture_output=True, text=True,
                           encoding="utf-8", errors="replace", timeout=300)
    except subprocess.TimeoutExpired:
        return False, "编译超时 300 秒，链接器可能去灵界了"
    if r.returncode != 0:
        return False, (r.stderr or r.stdout or "")[-800:]
    return True, ""


os.makedirs(构建, exist_ok=True)


def 做runner(exe路径):
    return lambda 输入: 跑命令([exe路径], 输入)


def 做pyrunner(py路径):
    return lambda 输入: 跑命令([PY, py路径], 输入)


# ------------------------------- 用例清单 ------------------------------------
A用例 = [
    ("1 2 3", "Bad"), ("2 2 2", "Good"), ("3 1 3", "Good"),
    ("5 5 6", "Bad"), ("5 5 5", "Good"), ("0 0 1", "Bad"), ("0 0 0", "Good"),
    ("-1 3 3", "Good"), ("-10 0 -5", "Good"), ("-3 -1 0", "Bad"),
    ("100 100 101", "Bad"), ("1000 5 999", "Good"), ("2 3 3", "Good"),
    ("+2 3 3", "Good"), ("03 4 3", "Good"), ("64 64 65", "Bad"),
    ("65 3 65", "Good"), ("100 200 150", "Good"),
]
B用例 = [
    ("2 3 4 100", "75"), ("2 3 4 13", "Not Found"), ("2 3 4 12", "Not Found"),
    ("1 5 3", "Not Found"), ("5 10", "4"), ("1 0 1", "Not Found"),
    ("1 0 2", "1"), ("2 -3 4 100", "123"), ("1 0 1000000000", "999999999"),
    ("60 60 60 1000000000000000000", "999999999999783999"),
    ("3 1 2 3 100", "81"),
    ("9 999999999 999999999 999999999 999999999 1", "Not Found"),
    ("-9 999999999 999999999 999999999 999999999 9000000000000000000",
     "8999999964000000062999999964000000008"),
    ("2 99999999999999999999 1", "Not Found"), ("+2 3 4 100", "75"),
]
C1用例 = [
    ('{"data": [\n  {"name": "dario", "cost": 114.51},\n'
     '  {"name": "trump", "cost": 191.98}\n]}', "306.49 trump"),
    ('{"data":[{"name":"a","cost":5.5},{"name":"b","cost":5.5},'
     '{"name":"c","cost":1}]}', "12 b"),
    ('{"data":[{"name":"x","cost":-5},{"name":"y","cost":10}]}', "5 y"),
    ('{"data":[{"name":"\\u533b\\u751f","cost":3}]}', "3 医生"),
    ('\ufeff{"data":[{"name":"b","cost":2}]}', "2 b"),   # BOM 前缀,转义写明,免得源码里养隐形生物
    ('{"data":[]}', "0"),
    ('{"data":[{"name":"z","cost":0.05},{"name":"w","cost":0.10}]}', "0.15 w"),
]


def 测一组(题目, 文件标签, runner, 用例):
    for 输入, 期望 in 用例:
        实际, 码, 错 = runner(输入)
        附加 = "" if 码 == 0 else "(退出码 %d, stderr: %s)" % (码, 错[-120:].replace("\n", " "))
        报结果("%s/%s" % (文件标签, 题目), 实际, 期望, 附加)


# ------------------------------- C2 冒牌鬼神 ---------------------------------
应答清单 = ["42", "3.5", "garbage!@#", "  7  ", "1e2", "abc",
            "13", "nan", "-4", "​99", "备用"]
应答队列 = list(应答清单)


class 冒牌鬼神(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        try:
            正文 = 应答队列.pop(0)
        except IndexError:
            正文 = "0"
        数据 = 正文.encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "text/plain; charset=utf-8")
        self.send_header("Content-Length", str(len(数据)))
        self.end_headers()
        self.wfile.write(数据)

    def log_message(self, *args):
        pass  # 冒牌货也要低调


def 测C2(文件标签, 原文件, 是python):
    应答队列[:] = list(应答清单)   # 上一轮可能把应答喝光了，重新斟满
    服务器 = socketserver.TCPServer(("127.0.0.1", 0), 冒牌鬼神)
    端口 = 服务器.server_address[1]
    threading.Thread(target=服务器.serve_forever, daemon=True).start()
    try:
        with open(原文件, encoding="utf-8") as f:
            源码 = f.read()
        if 是python:
            临时 = os.path.join(构建, "_临时_ghost.py")
            with open(临时, "w", encoding="utf-8") as f:
                f.write(源码.replace("114.51.4.191", "127.0.0.1")
                             .replace(":9810", ":%d" % 端口))
            runner = 做pyrunner(临时)
        else:
            临时cpp = os.path.join(构建, "_临时_ghost.cpp")
            临时exe = os.path.join(构建, "_临时_ghost.exe")
            with open(临时cpp, "w", encoding="utf-8") as f:
                f.write(源码.replace('"114.51.4.191"', '"127.0.0.1"')
                             .replace("lingjie_port = 9810",
                                      "lingjie_port = %d" % 端口))
            好了, 为什么 = 编译(临时cpp, 临时exe)
            if not 好了:
                报结果("%s/C2" % 文件标签, "编译失败", "260.5", 为什么)
                报结果("%s/C2失联" % 文件标签, "编译失败", "0")
                return
            runner = 做runner(临时exe)
        # 期望：42 + 3.5 + 7 + 100 + 13 + (-4) + 99 = 260.5
        # （garbage/abc/nan 均为乱码不计；"  7  " 去空白后有效；零宽剥掉后 99 有效）
        实际, 码, 错 = runner("")
        报结果("%s/C2" % 文件标签, 实际, "260.5",
               "" if 码 == 0 else "(stderr: %s)" % 错[-150:].replace("\n", " "))
    finally:
        服务器.shutdown()
        for 残骸 in ("_临时_ghost.py", "_临时_ghost.cpp", "_临时_ghost.exe"):
            路径 = os.path.join(构建, 残骸)
            if os.path.exists(路径):
                os.remove(路径)


def 测C2失联(文件标签, 原文件, 是python):
    print("[信息] C2失联：这台机器对无人监听的端口是静默丢包，"
          "每次 connect 要熬约 21 秒 SYN 超时，三境界合计约 63 秒，请勿惊慌")
    # 占位拿一个端口号随即放掉，模拟「鬼神不在家」
    服务器 = socketserver.TCPServer(("127.0.0.1", 0), 冒牌鬼神)
    服务器.server_close()   # 放掉端口，连它等于对着空气施法
    with open(原文件, encoding="utf-8") as f:
        源码 = f.read()
    if 是python:
        临时 = os.path.join(构建, "_临时_ghost.py")
        with open(临时, "w", encoding="utf-8") as f:
            f.write(源码.replace("114.51.4.191", "127.0.0.1").replace(":9810", ":9"))
        runner = 做pyrunner(临时)
    else:
        临时cpp = os.path.join(构建, "_临时_ghost.cpp")
        临时exe = os.path.join(构建, "_临时_ghost.exe")
        with open(临时cpp, "w", encoding="utf-8") as f:
            f.write(源码.replace('"114.51.4.191"', '"127.0.0.1"')
                         .replace("lingjie_port = 9810", "lingjie_port = 9"))
        好了, _ = 编译(临时cpp, 临时exe)
        if not 好了:
            return
        runner = 做runner(临时exe)
    实际, _, _ = runner("")
    报结果("%s/C2失联" % 文件标签, 实际, "0")
    for 残骸 in ("_临时_ghost.py", "_临时_ghost.cpp", "_临时_ghost.exe", "_临时_ghost.obj"):
        路径 = os.path.join(构建, 残骸)
        if os.path.exists(路径):
            os.remove(路径)


# ================================ 开始检查 ====================================
cpp文件 = {名: os.path.join(这里, 名 + ".cpp")
           for 名 in ("inequality", "product", "bill", "ghost")}
py目录 = os.path.join(这里, "python旧版备份")

print("== 编译器：%s ==" % ("未找到（C++ 用例将跳过）" if not 编译器 else 编译器))

if 编译器:
    exe表 = {}
    编译全过 = True
    for 名, 路径 in cpp文件.items():
        if not os.path.exists(路径):
            continue
        exe = os.path.join(构建, 名 + ".exe")
        好了, 为什么 = 编译(路径, exe)
        if 好了:
            exe表[名] = exe
            print("== 编译 %s.cpp 通过 ==" % 名)
        else:
            编译全过 = False
            print("== 编译 %s.cpp 失败 ==\n%s" % (名, 为什么))
    if not 编译全过:
        print("编译都过不了，屎山塌成零件了。")
        sys.exit(1)
    if exe表:
        测一组("A", "cpp", 做runner(exe表["inequality"]), A用例)
        测一组("B", "cpp", 做runner(exe表["product"]), B用例)
        测一组("C1", "cpp", 做runner(exe表["bill"]), C1用例)
        测C2("cpp", cpp文件["ghost"], False)
        测C2失联("cpp", cpp文件["ghost"], False)

if os.path.isdir(py目录):
    测一组("A", "py", 做pyrunner(os.path.join(py目录, "inequality.py")), A用例)
    测一组("B", "py", 做pyrunner(os.path.join(py目录, "product.py")), B用例)
    测一组("C1", "py", 做pyrunner(os.path.join(py目录, "bill.py")), C1用例)
    测C2("py", os.path.join(py目录, "ghost.py"), True)
    测C2失联("py", os.path.join(py目录, "ghost.py"), True)

# ------------------------------- C# 版 ---------------------------------------
csc路径 = r"C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe"
cs目录 = os.path.join(这里, "csharp")

if os.path.exists(csc路径) and os.path.isdir(cs目录):
    def 编译cs(cs源, exe):
        # csc 是 2019 年的,吃 UTF-8 要 /codepage:65001,跟 MSVC 的 /utf-8 一个脾气
        try:
            r = subprocess.run(
                [csc路径, "/nologo", "/optimize+", "/codepage:65001",
                 "/nowarn:0219,0168,0414", "/out:" + exe,
                 cs源, os.path.join(cs目录, "rr.cs")],
                cwd=cs目录, capture_output=True, text=True,
                encoding="utf-8", errors="replace", timeout=300)
        except subprocess.TimeoutExpired:
            return False, "csc 编译超时 300 秒,编译器可能去灵界了"
        if r.returncode != 0:
            return False, (r.stderr or r.stdout or "")[-600:]
        return True, ""

    def 编译cs幽灵(目标端口, exe名):
        # C2 无法连真实地址(假地址),临时副本指向冒牌服务器,测完即删
        with open(cs源表["ghost"], encoding="utf-8") as f:
            源码 = f.read()
        临时cs = os.path.join(构建, "_临时_ghost.cs")
        with open(临时cs, "w", encoding="utf-8") as f:
            f.write(源码.replace('"114.51.4.191"', '"127.0.0.1"')
                         .replace("const int 灵界端口 = 9810",
                                  "const int 灵界端口 = %d" % 目标端口))
        return 编译cs(临时cs, exe名)

    cs源表 = {名: os.path.join(cs目录, 名 + ".cs")
              for 名 in ("inequality", "product", "bill", "ghost")}
    cs全过 = True
    for 名, 源 in cs源表.items():
        exe = os.path.join(构建, "cs_" + 名 + ".exe")
        好了, 为什么 = 编译cs(源, exe)
        if 好了:
            print("== 编译 %s.cs (C#) 通过 ==" % 名)
        else:
            cs全过 = False
            print("== 编译 %s.cs (C#) 失败 ==\n%s" % (名, 为什么))
    if not cs全过:
        print("C# 编译塌了,屎山塌成零件了。")
        sys.exit(1)
    测一组("A", "cs", 做runner(os.path.join(构建, "cs_inequality.exe")), A用例)
    测一组("B", "cs", 做runner(os.path.join(构建, "cs_product.exe")), B用例)
    测一组("C1", "cs", 做runner(os.path.join(构建, "cs_bill.exe")), C1用例)

    应答队列[:] = list(应答清单)   # 上几轮可能把应答喝光了,重新斟满
    cs幽灵exe = os.path.join(构建, "cs_临时_ghost.exe")
    服务器 = socketserver.TCPServer(("127.0.0.1", 0), 冒牌鬼神)
    threading.Thread(target=服务器.serve_forever, daemon=True).start()
    try:
        好了, 为什么 = 编译cs幽灵(服务器.server_address[1], cs幽灵exe)
        if not 好了:
            报结果("cs/C2", "编译失败", "260.5", 为什么)
        else:
            实际, 码, 错 = 做runner(cs幽灵exe)("")
            报结果("cs/C2", 实际, "260.5",
                   "" if 码 == 0 else "(stderr: %s)" % 错[-150:].replace("\n", " "))
    finally:
        服务器.shutdown()
        服务器.server_close()

    print("[信息] cs/C2失联：无人监听端口在本机要熬约 63 秒 SYN 超时,请勿惊慌")
    好了, _ = 编译cs幽灵(9, cs幽灵exe)
    if 好了:
        实际, _, _ = 做runner(cs幽灵exe)("")
        报结果("cs/C2失联", 实际, "0")
    for 残骸 in ("_临时_ghost.cs", "cs_临时_ghost.exe"):
        路径 = os.path.join(构建, 残骸)
        if os.path.exists(路径):
            os.remove(路径)

# ------------------------------- PHP 版 ---------------------------------------
php路径 = os.path.join(这里, "_toolchain", "php8", "php.exe")
php目录 = os.path.join(这里, "php")

if os.path.exists(php路径) and os.path.isdir(php目录):
    # PHP 不用编译,本院对此评价:"连编译都懒得编译,很好,很有精神。"
    def 做phprunner(php文件):
        return lambda 输入: 跑命令([php路径, php文件], 输入)

    php文件表 = {名: os.path.join(php目录, 名 + ".php")
                 for 名 in ("inequality", "product", "bill", "ghost")}
    缺货 = [p for p in php文件表.values() if not os.path.exists(p)]
    if 缺货:
        print("PHP 源码缺货: %s" % 缺货)
        sys.exit(1)

    def 做php幽灵(目标端口, 临时php):
        # C2 无法连真实地址(假地址),临时副本指向冒牌服务器,测完即删
        with open(php文件表["ghost"], encoding="utf-8") as f:
            源码 = f.read()
        with open(临时php, "w", encoding="utf-8") as f:
            f.write(源码.replace("'114.51.4.191'", "'127.0.0.1'")
                         .replace("灵界端口 = 9810", "灵界端口 = %d" % 目标端口))

    测一组("A", "php", 做phprunner(php文件表["inequality"]), A用例)
    测一组("B", "php", 做phprunner(php文件表["product"]), B用例)
    测一组("C1", "php", 做phprunner(php文件表["bill"]), C1用例)

    应答队列[:] = list(应答清单)   # 上几轮可能把应答喝光了,重新斟满
    服务器 = socketserver.TCPServer(("127.0.0.1", 0), 冒牌鬼神)
    threading.Thread(target=服务器.serve_forever, daemon=True).start()
    try:
        临时php = os.path.join(构建, "_临时_ghost.php")
        做php幽灵(服务器.server_address[1], 临时php)
        实际, 码, 错 = 做phprunner(临时php)("")
        报结果("php/C2", 实际, "260.5",
               "" if 码 == 0 else "(退出码 %d, stderr: %s)" % (码, 错[-150:].replace("\n", " ")))
    finally:
        服务器.shutdown()
        服务器.server_close()

    print("[信息] php/C2失联：无人监听端口在本机要熬约 63 秒 SYN 超时,请勿惊慌")
    做php幽灵(9, 临时php)
    实际, _, _ = 做phprunner(临时php)("")
    报结果("php/C2失联", 实际, "0")
    for 残骸 in ("_临时_ghost.php",):
        路径 = os.path.join(构建, 残骸)
        if os.path.exists(路径):
            os.remove(路径)

print()
print("========== 自检完成：通过 %d，失败 %d ==========" % (通过数, 失败数))
if 失败数:
    print("有失败项！屎山塌方，禁止提交。")
    sys.exit(1)
print("全绿。屎山结构稳固，可以运行 打包.py 生成 codes.zip 提交。")
