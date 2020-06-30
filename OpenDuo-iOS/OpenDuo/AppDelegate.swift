//
//  AppDelegate.swift
//  OpenDuo
//
//  Created by CavanSu on 2020/1/9.
//  Copyright © 2020 Agora. All rights reserved.
//

import UIKit
import AVFoundation

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {
    var window: UIWindow?
    
    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        try? AVAudioSession.sharedInstance().setMode(.videoChat)
        try? AVAudioSession.sharedInstance().setCategory(.playAndRecord,
                                                         options: [.allowBluetooth, .allowBluetoothA2DP])
        return true
    }
}

