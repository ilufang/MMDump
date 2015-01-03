MMDump-Win
==========

MMD Answer Dumping Executable for Windows
Windows平台么么答答案抓包

###关于
为了拯救苦逼的iOS用户们，只得出此下策，通过Windows转接WiFi实现数据拦截。  
当然，最方便不过就是用Android热点，再运行Android版~
不过这样有电脑能开WiFi就可以刷***任意平台***的么么答了~

###运行
1. 请安装WinPcap运行库[下载地址](http://www.winpcap.org/install/default.htm)
2. 使用网线连接后，在Windows上打开WiFi热点(什么随身WiFi应该没问题，只要数据从Windows过即可)，手机连接到该热点
3. 运行MMDump-Win.exe,你应该看到一个黑色的命令行窗口。  
如果出现乱码，请设置命令行为中文(英文系统对不起,如果真有这种情况请自行重编译)
4. 选择网络设备，就是看起来最顺眼的那个，输入数字按回车
5. 等一会儿或者随便上上网，看到窗口出现FIRST packet!!说明这个设备是正确的(只有一个没得选的忽略这步和下一步)
6. 如果网页加载完命令行无反应，请手动将每一个设备试一遍，直到有响应
7. 运行手机上的么么答，随便开一个百科话题测试一下，如果命令行上出现答案就成功了！
8. 打开天梯，yooooooooooo~

###编译
1. 请下载WinPcap开发者包WpdPack[下载地址](http://www.winpcap.org/devel.htm)
2. 此repo中的源码使用Dev-C++编写(虽然不是特别喜欢这个,但Windows各种神坑,尤其是Visual Studio的运行环境!!!)
3. 解压WpdPack，复制include和lib文件夹至编译器对应目录下(\Dev-Cpp\MinGW64\不同版本可能不一样，VC的自己查吧，或者添加外部或者外部引用都可以)
4. 工程选项中添加libwpcap.lib库
5. 构建工程，若自己构建请注意编译mmdump.cpp和dataparser.cpp文件，连接libwpcap.lib

###Troubleshooting
* MMDump可能需要管理员权限
* 一定安装并自起WinPcap
* 我苦逼没法测,大家找到问题告诉我啊~~
