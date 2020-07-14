//
//  DialViewController.swift
//  OpenDuo
//
//  Created by CavanSu on 2020/1/13.
//  Copyright © 2020 Agora. All rights reserved.
//

import UIKit
import AgoraRtmKit

class DialViewController: UIViewController, ShowAlertProtocol {
    @IBOutlet weak var numberView: NumberView!
    @IBOutlet weak var dialView: DialView!
    @IBOutlet weak var idCodeLabel: UILabel!
    
    private lazy var appleCallKit = CallCenter(delegate: self)
    
    var localNumber: String?

    var callerNumber: String?
    
    var prepareToVideoChat: (() -> ())?
    
    override var preferredStatusBarStyle: UIStatusBarStyle {
        return .lightContent
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let image = UIImage(named: "background")
        view.layer.contents = image?.cgImage
        
        dialView.delegate = self
        
        if let localNumber = localNumber {
            idCodeLabel.text?.append(localNumber)
        }
        if !(callerNumber?.isEmpty ?? true) {
            appleCallKit.showIncomingCall(of: callerNumber!)
        }
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        AgoraRtm.shared().inviterDelegate = self
    }
    
    override func shouldPerformSegue(withIdentifier identifier: String, sender: Any?) -> Bool {
        switch identifier {
        case "DialToCalling": return numberView.isInputCompletion
        default:              return true
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier else {
            return
        }
        
        switch identifier {
        case "DialToCalling":
            let vc = segue.destination as? CallingViewController
            vc?.localNumber = localNumber
            vc?.remoteNumber = numberView.string
            vc?.delegate = self
            callingVC(vc!, startOutgoing: numberView.string)
        case "DialToVideoChat":
            guard let sender = sender,
                let data = sender as? (channel: String, remote: UInt) else {
                    fatalError("rtc channel id")
            }
            
            guard let localNumber = localNumber else {
                fatalError("localNumber nil")
            }
            
            let vc = segue.destination as? VideoChatViewController
            vc?.modalPresentationStyle = .fullScreen
            vc?.channel = data.channel
            vc?.localUid = UInt(localNumber)
            vc?.remoteUid = data.remote
            vc?.delegate = self
        default:
            break
        }
    }
    
    @IBAction func doDeletePressed(_ sender: UIButton) {
        numberView.deleteLast()
    }
}

private extension DialViewController {
    func callingVC(_ vc: CallingViewController, startOutgoing numbers: String?) {
        guard let remoteNumber = numbers else {
            fatalError("remoteNumber nil")
        }
        
        appleCallKit.startOutgoingCall(of: remoteNumber)
//        appleCallKit.setCallConnected(of: remoteNumber)
    }
}

extension DialViewController: DialViewDelegate {
    func dialView(_ dialView: DialView, didPressedButton number: String) {
        numberView.append(number)
    }
}

extension DialViewController: CallingVCDelegate {
    func callingVC(_ vc: CallingViewController, didHungup reason: HungupReason) {
        vc.dismiss(animated: reason == .toVideoChat ? false : true) { [unowned self] in
            switch reason {
            case .error:
                self.showAlert(reason.description)
            case .remoteReject(let remote):
                self.appleCallKit.endCall(of: remote)
                self.showAlert(reason.description + ": \(remote)")
            case .normaly(let remote):
                guard let inviter = AgoraRtm.shared().inviter else {
                    fatalError("rtm inviter nil")
                }
                
                let errorHandle: ErrorCompletion = { [weak self] (error: AGEError) in
                    self?.showAlert(error.localizedDescription)
                }
                
                switch inviter.status {
                case .outgoing:
                    self.appleCallKit.endCall(of: remote)
                    inviter.cancelLastOutgoingInvitation(fail: errorHandle)
                default:
                    break
                }
            default:
                break
            }
        }
    }
}

extension DialViewController: VideoChatVCDelegate {
    func videoChat(_ vc: VideoChatViewController, didEndChatWith uid: UInt) {
        appleCallKit.endCall(of: "\(uid)")
        vc.dismiss(animated: true, completion: nil)
    }
}

extension DialViewController: AgoraRtmInvitertDelegate {
    func inviter(_ inviter: AgoraRtmCallKit, didReceivedIncoming invitation: AgoraRtmInvitation) {
        appleCallKit.showIncomingCall(of: invitation.caller)
    }
    
    func inviter(_ inviter: AgoraRtmCallKit, remoteDidCancelIncoming invitation: AgoraRtmInvitation) {
        appleCallKit.endCall(of: invitation.caller)
        if let vc = self.presentedViewController as? VideoChatViewController {
            vc.leaveChannel()
            vc.dismiss(animated: true, completion: nil)
        }
    }
}

extension DialViewController: CallCenterDelegate {
    func callCenter(_ callCenter: CallCenter, answerCall session: String) {
        print("callCenter answerCall")
                
        guard let inviter = AgoraRtm.shared().inviter else {
            fatalError("rtm inviter nil")
        }
        
        guard let channel = inviter.lastIncomingInvitation?.content else {
            fatalError("lastIncomingInvitation content nil")
        }
        
        guard let remote = UInt(session) else {
            fatalError("string to int fail")
        }
        
        inviter.accpetLastIncomingInvitation()
        
        // present VideoChat VC after 'callCenterDidActiveAudioSession'
        self.prepareToVideoChat = { [weak self] in
            var data: (channel: String, remote: UInt)
            data.channel = channel
            data.remote = remote
            self?.performSegue(withIdentifier: "DialToVideoChat", sender: data)
        }
    }
    
    func callCenter(_ callCenter: CallCenter, declineCall session: String) {
        print("callCenter declineCall")
        
        guard let inviter = AgoraRtm.shared().inviter else {
            fatalError("rtm inviter nil")
        }
        
        inviter.refuseLastIncomingInvitation {  [weak self] (error) in
            self?.showAlert(error.localizedDescription)
        }
    }
    
    func callCenter(_ callCenter: CallCenter, startCall session: String) {
        print("callCenter startCall")
        
        guard let kit = AgoraRtm.shared().kit else {
            fatalError("rtm kit nil")
        }
        
        guard let localNumber = localNumber else {
            fatalError("localNumber nil")
        }
        
        guard let inviter = AgoraRtm.shared().inviter else {
            fatalError("rtm inviter nil")
        }
        
        guard let vc = self.presentedViewController as? CallingViewController else {
            fatalError("CallingViewController nil")
        }
        
        let remoteNumber = session
        
        // rtm query online status
        kit.queryPeerOnline(remoteNumber, success: { [weak vc] (onlineStatus) in
            switch onlineStatus {
            case .online:      sendInvitation(remote: remoteNumber, callingVC: vc!)
            case .offline:     vc?.close(.remoteReject(remoteNumber))
            case .unreachable: vc?.close(.remoteReject(remoteNumber))
            @unknown default:  fatalError("queryPeerOnline")
            }
        }) { [weak vc] (error) in
            vc?.close(.error(error))
        }
        
        // rtm send invitation
        func sendInvitation(remote: String, callingVC: CallingViewController) {
            let channel = "\(localNumber)-\(remoteNumber)-\(Date().timeIntervalSinceReferenceDate)"
            
            inviter.sendInvitation(peer: remoteNumber, extraContent: channel, accepted: { [weak self, weak vc] in
                vc?.close(.toVideoChat)
                
                self?.appleCallKit.setCallConnected(of: remote)
                
                guard let remote = UInt(remoteNumber) else {
                    fatalError("string to int fail")
                }
                
                var data: (channel: String, remote: UInt)
                data.channel = channel
                data.remote = remote
                self?.performSegue(withIdentifier: "DialToVideoChat", sender: data)
                
            }, refused: { [weak vc] in
                vc?.close(.remoteReject(remoteNumber))
            }) { [weak vc] (error) in
                vc?.close(.error(error))
            }
        }
    }
    
    func callCenter(_ callCenter: CallCenter, muteCall muted: Bool, session: String) {
        print("callCenter muteCall")
    }
    
    func callCenter(_ callCenter: CallCenter, endCall session: String) {
        print("callCenter endCall")
        self.prepareToVideoChat = nil
    }
    
    func callCenterDidActiveAudioSession(_ callCenter: CallCenter) {
        print("callCenter didActiveAudioSession")
        
        // Incoming call
        if let prepare = self.prepareToVideoChat {
            prepare()
        }
    }
}
