SerialWizard 串口调试助手
====
SerialWizard是一个使用Qt & C++ 开发的跨平台串口调试工具

![](https://github.com/changfeng1050/SerialWizard/raw/master/images/screenshot_overview.png)

## Features


- 跨平台(Windows,Linux)
- 串口名、波特率、数据位、停止位、校验位设置,自动识别可用串口
- 基本的数据接收、发送（ascii和hex格式、接收时间戳、接收数据换行显示）
- 当前串口打开状态，发送和接收字节数
- 控件布局自动适应大小
- 定时按照规定时间间隔自动发送（按照数据帧、行、固定字节数）
- 循环发送
- 支持串口、TCP（作为服务器端）通道传输数据，可以将串口数据转发到TCP或者TCP数据转发到串口（暂时未实现）,方便在没有串口的设备（如手机）或者是没有网络功能的设备（如单片机）上调试数据
- 计算数据校验码的功能（校验和、异或值）
- 自动保存配置
- 数据保存的功能
- 发送数据添加结尾 \r\n、 \r、 \n
- 拖放文件到程序中自动更新内容到发送输入框
- 处理输入文本,如自动整理空格、删除前缀、后缀、匹配字符、删除/保留匹配行、自动换行

## 开发环境

本源码可以使用[Clion](https://www.jetbrains.com/clion/)和[Qt Creator](https://github.com/qt-creator/qt-creator)开发

- Qt 5.14.2
- C++ 20

### Clion

使用Clion开发需要注意事项

-  Clion工具链设置 File -> Settiongs -> Build,Execution,Deployment -> Toolchians -> Enviromant, 选择 MinGW，选择MinGw的路径，也可以选择Qt自带的版本的路径，如 C:\Qt\Qt5.14.2\Tools\mingw730_64
-  CMakeList.txt中配置Qt的相关设置

### Qt Creator

使用 Qt Creator开发需要注意事项

-  在SerialWizard.pro配置Qt的相关设置
-  在SerialWizard.pro添加项目的源码文件

## 计划

-  TCP数据转发到串口
-  按照GBK、UTF8发送、接收数据
-  多语言支持

## 关于

开发这个串口助手主要是为了方便自己开发调试，如果这个工具能满足你的需求，start一个呗，如果有什么需求，也可以提出来哦 (｡˘•ε•˘｡), 我的邮箱 changfeng1050@hotmail.com.
