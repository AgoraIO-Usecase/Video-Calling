//
//  AgoraRtm.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
//

import Foundation
import AgoraRtmKit

typealias Completion = (() -> Void)?
typealias ErrorCompletion = ((AGEError) -> Void)?

enum LoginStatus {
    case online, offline
}

protocol AgoraRtmInvitertDelegate: NSObjectProtocol {
    func inviter(_ inviter: AgoraRtmCallKit, didReceivedIncoming invitation: AgoraRtmInvitation)
    func inviter(_ inviter: AgoraRtmCallKit, remoteDidCancelIncoming invitation: AgoraRtmInvitation)
}

struct AgoraRtmInvitation {
    var content: String?
    var caller: String // outgoint call
    var callee: String // incoming call
    
    fileprivate static func agRemoteInvitation(_ ag: AgoraRtmRemoteInvitation) -> AgoraRtmInvitation {
        guard let account = AgoraRtm.shared().account else {
            fatalError("rtm account nil")
        }
        
        let invitation = AgoraRtmInvitation(content: ag.content,
                                            caller: ag.callerId,
                                            callee: account)
        
        return invitation
    }
    
    fileprivate static func agLocalInvitation(_ ag: AgoraRtmLocalInvitation) -> AgoraRtmInvitation {
        guard let account = AgoraRtm.shared().account else {
            fatalError("rtm account nil")
        }
        
        let invitation = AgoraRtmInvitation(content: ag.content,
                                            caller: account,
                                            callee: ag.calleeId)
        
        return invitation
    }
}

class AgoraRtm: NSObject {
    static let rtm = AgoraRtm()
    
    // Kit
    let kit = AgoraRtmKit(appId: KeyCenter.AppId, delegate: nil)
    var status: LoginStatus = .offline
    fileprivate var account: String?
    
    // CallKit
    var inviter: AgoraRtmCallKit? {
        return kit?.getRtmCall()
    }
    
    weak var inviterDelegate: AgoraRtmInvitertDelegate?
    
    fileprivate var lastOutgoingInvitation: AgoraRtmLocalInvitation?
    fileprivate var lastIncomingInvitation: AgoraRtmRemoteInvitation?
    fileprivate var callKitRefusedBlock: Completion = nil
    fileprivate var callKitAcceptedBlock: Completion = nil
    
    static func shared() -> AgoraRtm {
        return rtm
    }
    
    override init() {
        super.init()
        inviter?.callDelegate = self
    }
    
    func setLogPath(_ path: String) {
        guard let kit = kit else {
            fatalError("rtm kit nil")
        }
        kit.setLogFile(path)
    }
}

extension AgoraRtmKit {
    func login(account: String, token: String?, success: Completion = nil, fail: ErrorCompletion = nil) {
        print("rtm login account: \(account)")
        
        AgoraRtm.shared().account = account
        
        login(byToken: token, user: account) { (errorCode) in
            guard errorCode == AgoraRtmLoginErrorCode.ok else {
                if let fail = fail {
                    fail(AGEError(type: .fail("rtm login fail: \(errorCode.rawValue)")))
                }
                return
            }
            
            AgoraRtm.shared().status = .online
            
            if let success = success {
                success()
            }
        }
    }
    
    func queryPeerOnline(_ peer: String, success: ((_ status: AgoraRtmPeerOnlineState) -> Void)? = nil, fail: ErrorCompletion = nil) {
        print("rtm query peer: \(peer)")
        
        queryPeersOnlineStatus([peer]) { (onlineStatusArray, errorCode) in
            guard errorCode == AgoraRtmQueryPeersOnlineErrorCode.ok else {
                if let fail = fail {
                    fail(AGEError(type: .fail("rtm queryPeerOnline fail: \(errorCode.rawValue)")))
                }
                return
            }
            
            guard let onlineStatus = onlineStatusArray?.first else {
                if let fail = fail {
                    fail(AGEError(type: .fail("rtm queryPeerOnline array nil")))
                }
                return
            }
            
            if let success = success {
                success(onlineStatus.state)
            }
        }
    }
}

extension AgoraRtmCallKit {
    enum Status {
        case outgoing, incoming, none
    }
    
    var lastIncomingInvitation: AgoraRtmInvitation? {
        let rtm = AgoraRtm.shared()
        
        if let agInvitation = rtm.lastIncomingInvitation {
            let invitation = AgoraRtmInvitation.agRemoteInvitation(agInvitation)
            return invitation
        } else {
            return nil
        }
    }
    
    var status: Status {
        if let _ = AgoraRtm.shared().lastOutgoingInvitation {
            return .outgoing
        } else if let _ = AgoraRtm.shared().lastIncomingInvitation {
            return .incoming
        } else {
            return .none
        }
    }
    
    func sendInvitation(peer: String, extraContent: String? = nil, accepted: Completion = nil, refused: Completion = nil, fail: ErrorCompletion = nil) {
        print("rtm sendInvitation peer: \(peer)")
        
        let rtm = AgoraRtm.shared()
        let invitation = AgoraRtmLocalInvitation(calleeId: peer)
        invitation.content = extraContent
        
        rtm.lastOutgoingInvitation = invitation
        
        send(invitation) { [unowned rtm] (errorCode) in
            guard errorCode == AgoraRtmInvitationApiCallErrorCode.ok else {
                if let fail = fail {
                    fail(AGEError(type: .fail("rtm send invitation fail: \(errorCode.rawValue)")))
                }
                return
            }
            
            rtm.callKitAcceptedBlock = accepted
            rtm.callKitRefusedBlock = refused
        }
    }
    
    func cancelLastOutgoingInvitation(fail: ErrorCompletion = nil) {
        let rtm = AgoraRtm.shared()
        
        guard let last = rtm.lastOutgoingInvitation else {
            return
        }
        
        cancel(last) { (errorCode) in
            guard errorCode == AgoraRtmInvitationApiCallErrorCode.ok else {
                if let fail = fail {
                    fail(AGEError(type: .fail("rtm cancel invitation fail: \(errorCode.rawValue)")))
                }
                return
            }
        }
        
        rtm.lastOutgoingInvitation = nil
    }
    
    func refuseLastIncomingInvitation(fail: ErrorCompletion = nil) {
        let rtm = AgoraRtm.shared()
        
        guard let last = rtm.lastIncomingInvitation else {
            return
        }
        
        refuse(last) { (errorCode) in
            guard errorCode == AgoraRtmInvitationApiCallErrorCode.ok else {
                if let fail = fail {
                    fail(AGEError(type: .fail("rtm refuse invitation fail: \(errorCode.rawValue)")))
                }
                return
            }
        }
    }
    
    func accpetLastIncomingInvitation(fail: ErrorCompletion = nil) {
        let rtm = AgoraRtm.shared()
        
        guard let last = rtm.lastIncomingInvitation else {
            fatalError("rtm lastIncomingInvitation")
        }
        
        accept(last) {(errorCode) in
            guard errorCode == AgoraRtmInvitationApiCallErrorCode.ok else {
                if let fail = fail {
                    fail(AGEError(type: .fail("rtm refuse invitation fail: \(errorCode.rawValue)")))
                }
                return
            }
        }
    }
}

extension AgoraRtm: AgoraRtmCallDelegate {
    func rtmCallKit(_ callKit: AgoraRtmCallKit, localInvitationAccepted localInvitation: AgoraRtmLocalInvitation, withResponse response: String?) {
        print("rtmCallKit localInvitationAccepted")
        
        let rtm = AgoraRtm.shared()
        if let accepted = rtm.callKitAcceptedBlock {
            DispatchQueue.main.async {
                accepted()
            }
            rtm.callKitAcceptedBlock = nil
        }
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, localInvitationRefused localInvitation: AgoraRtmLocalInvitation, withResponse response: String?) {
        print("rtmCallKit localInvitationRefused")
        
        let rtm = AgoraRtm.shared()
        if let refused = rtm.callKitRefusedBlock {
            DispatchQueue.main.async {
                refused()
            }
            rtm.callKitRefusedBlock = nil
        }
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, remoteInvitationReceived remoteInvitation: AgoraRtmRemoteInvitation) {
        print("rtmCallKit remoteInvitationReceived")
        
        let rtm = AgoraRtm.shared()
        
        guard rtm.lastIncomingInvitation == nil else {
            return
        }
        
        guard let inviter = rtm.inviter else {
            fatalError("rtm inviter nil")
        }
        
        DispatchQueue.main.async { [unowned inviter, weak self] in
            self?.lastIncomingInvitation = remoteInvitation
            let invitation = AgoraRtmInvitation.agRemoteInvitation(remoteInvitation)
            self?.inviterDelegate?.inviter(inviter, didReceivedIncoming: invitation)
        }
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, remoteInvitationCanceled remoteInvitation: AgoraRtmRemoteInvitation) {
        print("rtmCallKit remoteInvitationCanceled")
        let rtm = AgoraRtm.shared()
        
        guard let inviter = rtm.inviter else {
            fatalError("rtm inviter nil")
        }
        
        DispatchQueue.main.async { [weak self] in
            let invitation = AgoraRtmInvitation.agRemoteInvitation(remoteInvitation)
            self?.inviterDelegate?.inviter(inviter, remoteDidCancelIncoming: invitation)
            self?.lastIncomingInvitation = nil
        }
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, localInvitationReceivedByPeer localInvitation: AgoraRtmLocalInvitation) {
        print("rtmCallKit localInvitationReceivedByPeer")
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, localInvitationCanceled localInvitation: AgoraRtmLocalInvitation) {
        print("rtmCallKit localInvitationCanceled")
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, localInvitationFailure localInvitation: AgoraRtmLocalInvitation, errorCode: AgoraRtmLocalInvitationErrorCode) {
        print("rtmCallKit localInvitationFailure: \(errorCode.rawValue)")
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, remoteInvitationFailure remoteInvitation: AgoraRtmRemoteInvitation, errorCode: AgoraRtmRemoteInvitationErrorCode) {
        print("rtmCallKit remoteInvitationFailure: \(errorCode.rawValue)")
        self.lastIncomingInvitation = nil
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, remoteInvitationRefused remoteInvitation: AgoraRtmRemoteInvitation) {
        print("rtmCallKit remoteInvitationRefused")
        self.lastIncomingInvitation = nil
    }
    
    func rtmCallKit(_ callKit: AgoraRtmCallKit, remoteInvitationAccepted remoteInvitation: AgoraRtmRemoteInvitation) {
        print("rtmCallKit remoteInvitationAccepted")
        self.lastIncomingInvitation = nil
    }
}
