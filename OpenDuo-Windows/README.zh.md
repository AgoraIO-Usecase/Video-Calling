# Open Live Windows

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成Agora视频SDK，实现多人视频连麦直播。

在这个示例项目中包含了以下功能：

- 呼叫指定id的用户；
- 取消呼叫；
- 接受呼叫；
- 拒绝呼叫；
- 接受呼叫之后进行视频通话；
- RTC频道号=主叫ID+被叫ID

本开源项目使用 **C++** 语言

## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 App ID。将 APP_ID宏定义内容改为刚才申请的 App ID


* #define APP_ID _T("Your App ID")

然后在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载 **视频通话/视频互动直播 SDK和实时消息 SDK**，解压后将其中的 **sdk** 复制到本项目目录下（并覆盖原有旧目录）。
最后使用 QT5.6 打开 OpenLive.pro，编译整个解决方案即可运行

* 如果不在代码里填写appid，也可以在运行目录下放置一个AgoraConfigOpenDuo.ini文件，文件内容：

    [BaseInfo]
    AppId=xxxx

**注意：**如果代码里指定了appid就不会使用ini文件的appid。


## 运行环境
* VC++ 2013(或更高版本)
* QT5.6(或更高版本)
* Windows 7(或更高版本)

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的bug，欢迎提交 [issue](https://github.com/AgoraIO/Basic-Video-Call/issues)

## 代码许可

The MIT License (MIT).
