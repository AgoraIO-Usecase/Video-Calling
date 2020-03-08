#!/usr/bin/python
# -*- coding: UTF-8 -*-
import re
import os

def main():
    RTM_SDK_URL = "https://download.agora.io/rtmsdk/release/Agora_RTM_SDK_for_iOS_v1_2_2.zip?_ga=2.86470680.1650799636.1583667533-5559409.1503304822"
    
    if "RTM_SDK_URL" in os.environ:
        RTM_SDK_URL = os.environ["RTM_SDK_URL"]
    
    TARGET_RTM_LIBS_ZIP = "agora_rtm_sdk.zip"
    TARGET_RTM_INTERNAL_FOLDER = "agora_rtm_sdk"
    
    RTM_ZIP_STRUCTURE_FOLDER = "Agora_RTM_SDK_for_iOS/libs"
    
    RTM_FRAMEWORK_NAME = "AgoraRtmKit.framework"
    
    APP_NAME = "OpenDuo"

    rm = "rm -r " + "*_sdk"
    os.system(rm)
    
    rm = "rm -r " + "*.zip"
    os.system(rm)
    
    # RTM SDK Download
    wget = "wget -q " + RTM_SDK_URL + " -O " + TARGET_RTM_LIBS_ZIP
    os.system(wget)
    
    unzip = "unzip -q " + TARGET_RTM_LIBS_ZIP + " -d " + TARGET_RTM_INTERNAL_FOLDER
    os.system(unzip)
    
    # mv sdks
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
