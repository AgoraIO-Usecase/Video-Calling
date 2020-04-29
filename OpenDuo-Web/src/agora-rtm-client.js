import AgoraRTM from 'agora-rtm-sdk'
import { log } from './utils/utils'
import EventEmitter from 'events'

const appID = process.env.REACT_APP_AGORA_APP_ID

export default class RTMClient {
  constructor () {
    this._client = AgoraRTM.createInstance(appID)
    this._uid = ''
    this._token = undefined
    this._invitation = null
    this._remoteInvitation = null
    this.eventBus = new EventEmitter() 
    this.status = 'offLine' //onLine, offLine, calling, meeting

    this.peerInvitation()
  }

  login(uid) {
    this._uid = uid
    return this._client.login({
      token: this._token,
      uid: uid,
    }).then(() => {
      this.status = 'onLine'
    })
  }

  inquire(peerIds) {
    return this._client.queryPeersOnlineStatus(peerIds)  
  }
  //a拨打给b，a进入呼叫中页面。
  //b判断自己状态是否在空闲中（也就是在线），
  //如果不是空闲，调用挂断。b不会进入被呼叫中页面

  localInvitation(calleeId) {
    this._invitation = null
    this._invitation = this._client.createLocalInvitation(calleeId)
    this._invitation.on('LocalInvitationReceivedByPeer', () => {
      log('Peers receive calls')
      this.status = 'calling'
      this.eventBus.emit('LocalInvitationReceivedByPeer')
    })
    this._invitation.on('LocalInvitationCanceled', () => {
      log('Cancel call invitation')
      this.eventBus.emit('LocalInvitationCanceled')
    })
    this._invitation.on('LocalInvitationAccepted', () => {
      log('Peers accept invitations to call')
      this.status = 'meeting'
      this.eventBus.emit('LocalInvitationAccepted')
    })
    this._invitation.on('LocalInvitationRefused', () => {
      log('Peers refuse to call invitations')
      this.status = 'onLine'
      this.eventBus.emit('LocalInvitationRefused')
    })
    this._invitation.on('LocalInvitationFailure', () => {
      log('Call process failed')
      this.status = 'onLine'
      this.eventBus.emit('LocalInvitationFailure')
    })
    this._invitation.send()  
  }
  
  peerInvitation() {
    this._remoteInvitation = null
    this._client.on('RemoteInvitationReceived', (remoteInvitation) => {
      log('Receive call invitation', remoteInvitation.callerId)
      if(this.status !== 'onLine') {
        setTimeout(() => {
          remoteInvitation.refuse()
        }, 1000)
        return
      }
      if(this._remoteInvitation !== null) {
        this._remoteInvitation.removeAllListeners()
        this._remoteInvitation = null
      }
      this.status = 'calling'
      this._remoteInvitation = remoteInvitation 
      this.peerEvents()
      this.eventBus.emit('RemoteInvitationReceived', remoteInvitation)
    }) 
  }

  peerEvents () {
    this._remoteInvitation.on('RemoteInvitationCanceled', () => {
      log('The call invitation was cancelled')
      this.status = 'onLine'
      this.eventBus.emit('RemoteInvitationCanceled')
    })
    this._remoteInvitation.on('RemoteInvitationAccepted', () => {
      log('Peers receive calls')
      this.eventBus.emit('RemoteInvitationAccepted')
    })
    this._remoteInvitation.on('RemoteInvitationRefused', () => {
      log('The call invitation has been declined')
      this.eventBus.emit('RemoteInvitationRefused')
    })
    this._remoteInvitation.on('RemoteInvitationFailure', () => {
      log('Peer call process failed')
      this.status = 'onLine'
      this.eventBus.emit('RemoteInvitationFailure')
    })
  }
  
  cancelCall() {
    this._invitation && this._invitation.cancel()
    this.status = 'onLine'
  }

  acceptCall() {
    this._remoteInvitation && this._remoteInvitation.accept()
    this.status = 'meeting'
  }

  refuseCall() {
    this._remoteInvitation && this._remoteInvitation.refuse()
    this.status = 'onLine'
  }

  on (evt, callback) {
    const customEvents = [
      'LocalInvitationReceivedByPeer',
      'LocalInvitationCanceled',
      'LocalInvitationAccepted',
      'LocalInvitationRefused',
      'LocalInvitationFailure',
      'RemoteInvitationReceived',
      'RemoteInvitationCanceled',
      'RemoteInvitationAccepted',
      'RemoteInvitationRefused',
      'RemoteInvitationFailure',
    ]
    if (customEvents.indexOf(evt) !== -1) {
      this.eventBus.on(evt, callback)
      return
    }
    this._client.on(evt, callback)
  }
}