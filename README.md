MMDump
======

**MMD Answer Dumping Binary**

**Compile with the Android NDK. Run with root.**

###About

MMDump listens on the first available network and looks for packets containing json-formatted answers for MeMeDa. When found, it prints the answers in number format into the stdout, or send to the automation program.

###Libraries

* **Libpcap** for network capturing. [Homepage](http://www.tcpdump.org)
* **lodepng** for screenshot analyzing. [Homepage](http://lodev.org/lodepng/)
* **busybox** for some native calls [Homepage](www.busybox.net)
* **screencap** for screen capturing [Source](https://android.googlesource.com/platform/frameworks/base/+/jb-release/cmds/screencap/screencap.cpp)
* **input** for emulating input

###Build
Place the repo into the `jni` folder of your project.  
Run `ndk-build`  

###Usage
**这是关于此程序的Android应用的用法,关于mmdump命令的用法,请使用`-h`**

* `监听`: 自动分析数据包寻找答案,收到答案后通过Toast消息显示出来,如:`Ans:1342231`
* `自动执行`: 抓取数据包后,根据答案模拟输入自动刷分
* `停止`: 结束后台进程

**注意**  
* 程序需要root权限,如果由于root授权延迟,应用重启等原因导致卡死打不开,请清除数据后再开
* 请一定保持应用后台运行,如果可能,加入系统保护列表/清理白名单
* 为保证稳定,建议限制后台进程,尤其不要进行大型网络访问(如下载)
* 自动执行依赖像素点检测和触摸模拟,所以自动运行时尽量不要操作设备,建议接在电源上,保持屏幕常亮,并关闭SIM卡,QQ微信或其他任何可能产生通知或覆盖屏幕的内容
* 自动执行不稳定,对于广告虽然有优化但仍不稳定,如果广告页面被点开或其他情况请结束下载或返回,停止进程并重开,再重刷
* 自动运行正常的通知有:`TCP Listening started on xxx`(初始化), `Started on: xxx`(开始游戏), `Outputting Random Answers`(进入混淆模式), `Starting new session`(新建游戏), `call ended`(线程结束,选择话题后出现)
* 其他任何消息均为错误信息,如`Packet Matching Failure`,遇到此情况请结束重开 (当前版本实现不稳定,还未达可以完全不管不出错的地步,所以还是要看着)

任何形式的反馈请贴在这个Github仓库的Issue中,我回尽快处理/回复,谢谢!
