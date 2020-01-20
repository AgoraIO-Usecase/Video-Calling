#!/bin/bash

echo 'delete obsolete temp directory and recreate'
rm -rf rtm
mkdir rtm
cd rtm

echo 'downloading rtm sdk'
echo $RTM_SDK_LINK
curl -# -o rtm.zip -OL $RTM_SDK_LINK

echo 'unzipping rtm sdk'
unzip -n rtm.zip

pwd
ls -al

mv Agora_RTM_SDK_for_Android/libs/*.jar ../app/libs/
mv Agora_RTM_SDK_for_Android/libs/armeabi-v7a/*.so ../app/src/main/jniLibs/armeabi-v7a
mv Agora_RTM_SDK_for_Android/libs/arm64-v8a/*.so ../app/src/main/jniLibs/arm64-v8a
mv Agora_RTM_SDK_for_Android/libs/x86/*.so ../app/src/main/jniLibs/x86
mv Agora_RTM_SDK_for_Android/libs/x86_64/*.so ../app/src/main/jniLibs/x86_64

# back from temporary directory
cd ..

rm -rf rtm
