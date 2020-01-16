#!/usr/bin/python
# -*- coding: UTF-8 -*-
import re
import os

def main():
    RTC_SDK_URL = ""
    RTM_SDK_URL = ""
    
    if "RTC_SDK_URL" in os.environ:
        RTC_SDK_URL = os.environ["RTC_SDK_URL"]
    
    if "RTM_SDK_URL" in os.environ:
        RTC_SDK_URL = os.environ["RTM_SDK_URL"]
    
    TARGET_RTC_LIBS_ZIP = "agora_rtc_sdk.zip"
    TARGET_RTC_INTERNAL_FOLDER = "agora_rtc_sdk"
    
    TARGET_RTM_LIBS_ZIP = "agora_rtm_sdk.zip"
    TARGET_RTM_INTERNAL_FOLDER = "agora_rtm_sdk"
    
    RTC_ZIP_STRUCTURE_FOLDER = "Agora_Native_SDK_for_iOS_FULL/libs"
    RTM_ZIP_STRUCTURE_FOLDER = "Agora_RTM_SDK_for_iOS/libs"
    
    CRY_FRAMEWORK_NAME = "AgoraRtcCryptoLoader.framework"
    RTC_FRAMEWORK_NAME = "AgoraRtcKit.framework"
    RTM_FRAMEWORK_NAME = "AgoraRtmKit.framework"
    
    APP_NAME = "OpenDuo"

    rm = "rm -r " + "*_sdk"
    os.system(rm)
    
    rm = "rm -r " + "*.zip"
    os.system(rm)
    
    rm = "rm -r " + APP_NAME + "/" + "*.framework"
    os.system(rm)
    
    # RTC SDK Download
    wget = "wget -q " + RTC_SDK_URL + " -O " + TARGET_RTC_LIBS_ZIP
    os.system(wget)
    
    unzip = "unzip -q " + TARGET_RTC_LIBS_ZIP + " -d " + TARGET_RTC_INTERNAL_FOLDER
    os.system(unzip)
    
    # RTM SDK Download
    wget = "wget -q " + RTM_SDK_URL + " -O " + TARGET_RTM_LIBS_ZIP
    os.system(wget)
    
    unzip = "unzip -q " + TARGET_RTM_LIBS_ZIP + " -d " + TARGET_RTM_INTERNAL_FOLDER
    os.system(unzip)
    
    # mv sdks
    mv = "mv -f " + TARGET_RTC_INTERNAL_FOLDER + "/" + RTC_ZIP_STRUCTURE_FOLDER + "/" + "*.framework" + " \"" + APP_NAME +"\""
    os.system(mv)

    mv = "mv -f " + TARGET_RTM_INTERNAL_FOLDER + "/" + RTM_ZIP_STRUCTURE_FOLDER + "/" + "*.framework" + " \"" + APP_NAME +"\""
    os.system(mv)

    appId = ""
    if "AGORA_APP_ID" in os.environ:
        appId = os.environ["AGORA_APP_ID"]
    token = ""

    #if need reset
    f = open("./OpenDuo/KeyCenter.swift", 'r+')
    content = f.read()

    #if need reset
    appString = "\"" + appId + "\""
    tokenString = "\"" + token + "\""
    contentNew = re.sub(r'<#Your App Id#>', appString, content)
    contentNew = re.sub(r'<#Temp Access Token#>', tokenString, contentNew)

    f.seek(0)
    f.write(contentNew)
    f.truncate()


if __name__ == "__main__":
    main()
