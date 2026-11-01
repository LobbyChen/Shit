@echo off
rem cl 已经在 PATH 上就直接用(外部已设好 INCLUDE/LIB),不然才走 vcvars。
rem vcvars 会调 reg.exe,在受限环境里会被安全策略整棵杀掉,能跳就跳。
where cl >nul 2>&1
if errorlevel 1 (
  call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
)
cl /nologo /W0 /utf-8 /EHsc /O2 %*
