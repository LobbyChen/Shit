# -*- coding: utf-8 -*-
# =============================================================================
#   打包脚本：把要提交的文件压成 codes.zip。
#   用法：python 打包.py
#
#   题目要求 A、B 必做，C1 / C2 二选一。本脚本默认把四份全打包进去
#   （方便你自己替换试玩），如果想只交一份 C，把下面清单里不交的那个
#   删掉再运行即可。
# =============================================================================
import os
import zipfile

清单 = [
    "inequality.cpp",  # A 必做
    "product.cpp",     # B 必做
    "bill.cpp",        # C1（与 ghost.cpp 二选一）
    "ghost.cpp",       # C2（与 bill.cpp 二选一）
    "README.md",
]

这里 = os.path.dirname(os.path.abspath(__file__))
输出 = os.path.join(这里, "codes.zip")

with zipfile.ZipFile(输出, "w", zipfile.ZIP_DEFLATED) as 包:
    for 名字 in 清单:
        包.write(os.path.join(这里, 名字), 名字)

print("已生成 %s，共 %d 个文件：%s" % (输出, len(清单), "、".join(清单)))
print("提醒：C1（bill.cpp）与 C2（ghost.cpp）按赛规二选一，"
      "若只想交一份，请编辑本文件顶部的清单后重新运行。")
