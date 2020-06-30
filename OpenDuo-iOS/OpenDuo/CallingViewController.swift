//
//  CallingViewController.swift
//  OpenDuo
//
//  Created by CavanSu on 2020/1/13.
//  Copyright © 2020 Agora. All rights reserved.
//

import UIKit
import AgoraRtmKit
import AudioToolbox

enum HungupReason {
    case remoteReject(String), toVideoChat, normaly(String), error(Error)
    
    fileprivate var rawValue: Int {
        switch self {
        case .remoteReject: return 0
        case .toVideoChat:  return 1
        case .normaly:      return 2
        case .error:        return 3
        }
    }
    
    static func==(left: HungupReason, right: HungupReason) -> Bool {
        return left.rawValue == right.rawValue
    }
    
    var description: String {
        switch self {
        case .remoteReject:     return "remote reject"
        case .toVideoChat:      return "start video chat"
        case .normaly:          return "normally hung up"
        case .error(let error): return error.localizedDescription
        }
    }
}

protocol CallingVCDelegate: NSObjectProtocol {
    func callingVC(_ vc: CallingViewController, didHungup reason: HungupReason)
}

class CallingViewController: UIViewController {
    enum Operation {
        case on, off
    }
    
    @IBOutlet weak var headImageView: UIImageView!
    @IBOutlet weak var numberLabel: UILabel!
    
    private var ringStatus: Operation = .off {
        didSet {
            guard oldValue != ringStatus else {
                return
            }
            
            switch ringStatus {
            case .on:  startPlayRing()
            case .off: stopPlayRing()
            }
        }
    }
    
    private var animationStatus: Operation = .off {
        didSet {
            guard oldValue != animationStatus else {
                return
            }
            
            switch animationStatus {
            case .on:  startAnimating()
            case .off: stopAnimationg()
            }
        }
    }
    
    private let aureolaView = AureolaView(color: UIColor(red: 173.0 / 255.0,
                                                         green: 211.0 / 255.0,
                                                         blue: 252.0 / 255.0, alpha: 1))
    private var timer: Timer?
    private var soundId = SystemSoundID()
    
    weak var delegate: CallingVCDelegate?
    
    var localNumber: String?
    var remoteNumber: String?
    
    override var preferredStatusBarStyle: UIStatusBarStyle {
        return .lightContent
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let image = UIImage(named: "background")
        view.layer.contents = image?.cgImage
        
        guard let remoteNumber = remoteNumber else {
            fatalError("remoteNumber nil")
        }
        
        let numberString = NSMutableAttributedString(string: remoteNumber)
        numberString.addAttributes([NSAttributedString.Key.kern: 20],
                                   range: NSMakeRange(0, 3))
        numberLabel.attributedText = numberString
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        animationStatus = .on
        ringStatus = .on
    }
    
    @IBAction func doHungUpPressed(_ sender: UIButton) {
        close(.normaly(remoteNumber!))
    }
    
    func close(_ reason: HungupReason) {
        animationStatus = .off
        ringStatus = .off
        delegate?.callingVC(self, didHungup: reason)
    }
}

private extension CallingViewController {
    @objc func animation() {
        aureolaView.startLayerAnimation(aboveView: headImageView,
                                        layerWidth: 2)
    }
    
    func startAnimating() {
        let timer = Timer(timeInterval: 0.3,
                          target: self,
                          selector: #selector(animation),
                          userInfo: nil,
                          repeats: true)
        timer.fire()
        RunLoop.main.add(timer, forMode: .common)
        self.timer = timer
    }
    
    func stopAnimationg() {
        timer?.invalidate()
        timer = nil
        aureolaView.removeAnimation()
    }
    
    func startPlayRing() {
        let path = Bundle.main.path(forResource: "ring", ofType: "mp3")
        let url = URL.init(fileURLWithPath: path!)
        AudioServicesCreateSystemSoundID(url as CFURL, &soundId)
        
        AudioServicesAddSystemSoundCompletion(soundId,
                                              CFRunLoopGetMain(),
                                              nil, { (soundId, context) in
                                                AudioServicesPlaySystemSound(soundId)
        }, nil)
        
        AudioServicesPlaySystemSound(soundId)
    }
    
    func stopPlayRing() {
        AudioServicesDisposeSystemSoundID(soundId)
        AudioServicesRemoveSystemSoundCompletion(soundId)
    }
}
