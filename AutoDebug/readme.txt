- 首先将目录下文件放置在 d:\dumps (默认），其他路径需要自己修改下注册表字段

修改操作系统AeDebug参数

64位路径：\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows NT\CurrentVersion\AeDebug

auto="1"
Debugger="xxx"   --- 修改为正确的recrash 路径

二 执行脚本RegCrash.reg

三 异常的dump均会保存在 d:\dumps 路径下

四 已知问题
   1、 32位操作系统不了解注册表AeDebug的路径在哪
   2、 只支持32位进程的异常捕获，如果需要捕获64位的dump，请替换ntsd为64位版本
   3、 每捕获一次，会残留一个ntsd进程。
   4、 不支持进程名过滤，会捕获所有的异常进程
   
五 未来会解决已知问题