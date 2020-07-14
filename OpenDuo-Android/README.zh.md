# OpenLive for Android

[English](README.md) | 中文

**为了安全，建议正式环境中启用 [动态密钥鉴权](https://docs.agora.io/cn/Real-time-Messaging/RTM_key?platform=All%20Platforms) 机制**

这个开源示例项目演示了如何快速集成 Agora 视频 SDK 和实时消息 SDK，实现1对1视频通话。

在这个示例项目中包含了以下功能：

- 登录实时消息 RTM 服务器；
- 拨号和呼叫；
- 接听和挂断；
- 静音和解除静音；
- 切换前置摄像头和后置摄像头；

## 环境准备

- Android Studio 3.3 +
- 真实 Android 设备 (Nexus 5X 或者其它设备)
- 部分模拟器会存在功能缺失或者性能问题，所以推荐使用真机

## 运行示例程序
这个段落主要讲解了如何编译和运行实例程序。

### 创建Agora账号并获取AppId
在编译和启动实例程序前，您需要首先获取一个可用的App ID:

1. 在agora.io创建一个开发者账号

2. 前往后台页面，点击左部导航栏的 项目 > 项目列表 菜单

3. 复制后台的 App ID 并备注，稍后启动应用时会用到它

4. 在项目页面生成临时 Access Token (24小时内有效)并备注，注意生成的Token只能适用于对应的频道名。

5. 将 AppID 填写进 "app/src/main/res/values/strings_config.xml"

```
    <!-- PLEASE KEEP THIS App ID IN SAFE PLACE -->
    <!-- Get your own App ID at https://dashboard.agora.io/ -->
    <!-- After you entered the App ID, remove <##> outside of Your App ID -->
    <!-- <string name="private_app_id" translatable="false"><#YOUR APP ID#></string>-->
    <string name="private_app_id" translatable="false"><#YOUR APP ID#></string>
    <!-- Obtain a temp Access Token at https://dashboard.agora.io -->
    <!-- You will need to deploy your own token server for production release -->
    <!-- Leave this value empty if Security keys/Token is not enabled for your project -->
    <string name="agora_access_token" translatable="false"><#YOUR ACCESS TOKEN#></string>
```

### 集成 Agora 视频 SDK

Agora视频SDK会在Android Studio打开后自动通过gradle同步安装。

### 启动应用程序
连上设备，编译并运行。
也可以使用 `Gradle` 直接编译运行。

## 联系我们

- 如果你遇到了困难，可以先参阅 [常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考 [官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考 [官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看 [社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO-Usecase/Video-Calling/issues)

## 代码许可
The MIT License (MIT).
