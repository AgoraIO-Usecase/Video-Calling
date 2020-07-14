//
//  MainViewController.swift
//  OpenDuo
//
//  Created by CavanSu on 2020/1/9.
//  Copyright © 2020 Agora. All rights reserved.
//

import UIKit
import AgoraRtmKit

protocol ShowAlertProtocol: UIViewController {
    func showAlert(_ message: String, handler: ((UIAlertAction) -> Void)?)
    func showAlert(_ message: String)
}

extension ShowAlertProtocol {
    func showAlert(_ message: String, handler: ((UIAlertAction) -> Void)?) {
        view.endEditing(true)
        let alert = UIAlertController(title: nil, message: message, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .default, handler: handler))
        present(alert, animated: true, completion: nil)
    }
    
    func showAlert(_ message: String) {
        showAlert(message, handler: nil)
    }
}

class MainViewController: UIViewController, ShowAlertProtocol {
    @IBOutlet weak var idCodeLabel: UILabel!
    
    var localNumber: String?
    
    override var preferredStatusBarStyle: UIStatusBarStyle {
        return .lightContent
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let rtm = AgoraRtm.shared()
        let path = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true).first! + "/rtm.log"
        rtm.setLogPath(path)
        rtm.inviterDelegate = self
        
        let image = UIImage(named: "background")
        view.layer.contents = image?.cgImage
        
        // create local number
        let rand = arc4random_uniform(UInt32(9999 + 1 - 1000)) + 1000
        idCodeLabel.text?.append("\(rand)")
        localNumber = "\(rand)"
        
        // rtm login
        guard let localNumber = localNumber else {
            fatalError("localNumber nil")
        }
        
        guard let kit = AgoraRtm.shared().kit else {
            self.showAlert("AgoraRtmKit nil")
            return
        }
        
        kit.login(account: localNumber, token: nil) { [unowned self] (error) in
            self.showAlert(error.localizedDescription)
        }
    }
    
    override func shouldPerformSegue(withIdentifier identifier: String, sender: Any?) -> Bool {
        return (AgoraRtm.shared().status == .online ? true : false)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier else {
            return
        }
        
        switch identifier {
        case "MainToDial":
            let vc = segue.destination as? DialViewController
            vc?.localNumber = localNumber
            if sender != nil{
                vc?.callerNumber = sender as? String
            }
        default:
            break
        }
    }
}


extension MainViewController: AgoraRtmInvitertDelegate {
    func inviter(_ inviter: AgoraRtmCallKit, didReceivedIncoming invitation: AgoraRtmInvitation) {
        self.performSegue(withIdentifier: "MainToDial", sender: invitation.caller)
    }

    func inviter(_ inviter: AgoraRtmCallKit, remoteDidCancelIncoming invitation: AgoraRtmInvitation) {
    }
}
