# Agora OpenDuo for iOS

*其他语言版本： [简体中文](README.zh.md)*

This readme describes the steps and considerations for demonstrating the Agora OpenDuo iOS sample app.

## Introduction

Built upon the Agora Video SDK and the Agora RTM SDK, the Agora OpenDuo for iOS is an open-source demo that integrates 1 to 1 video call into your iOS applications.

This sample app allows you to:

- Login the RTM server
- Make a call
- Accept or hang up a call
- Mute/unmute a user
- Switch the camera

## Prerequisites

- Xcode 10.0+
- Physical iOS device (iPhone or iPad)
- iOS simulator is NOT supported

## Quick Start

This section shows you how to prepare, build, and run the sample application.

### Obtain an App Id

To build and run the sample application, get an App Id:

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App Id** from the Dashboard for later use.
4. Generate a temp **Access Token** (valid for 24 hours) from dashboard page with given channel name, save for later use.
5. Fill in the AppID in the *KeyCenter.swift*.

    ``` 
    static let AppId: String = <#Your App Id#>
    
    // assign token to nil if you have not enabled app certificate
    static let Token: String? = <#Temp Access Token#>
    ```

### Integrate the Agora Video SDK & Agora RTM SDK

1. Use 'pod install' command and Podfile to integrate **Agora Video SDK** and **Agora RTM SDK**.
2. Open OpenDuo.xcworkspace, connect your iPhone／iPad device, set up your code signature, and run the demo app.

## About the Dynamic Key

For security, Agora recommends enabling the Dynamic Key, see [RTM key](https://docs.agora.io/cn/Real-time-Messaging/RTM_key?platform=All%20Platforms).

## Contact Us

- For potential issues, take a look at our [FAQ](https://docs.agora.io/en/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API documentation at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/Basic-Video-Call/issues)

## License

The MIT License (MIT)

