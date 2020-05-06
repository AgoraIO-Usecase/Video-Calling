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

    //This status is used to control whether the phone can be accessed
    //The 'onLine' is ok.
    this.status = 'offLine' //onLine, offLine, calling, meeting

    this.peerInvitation()
  }

  //login uid
  login(uid) {
    this._uid = uid
    return this._client.login({
      token: this._token,
      uid: uid,
    }).then(() => {
      this.status = 'onLine'
    })
  }

  //Query login status of peers and return asynchronous
  inquire(peerIds) {
    return this._client.queryPeersOnlineStatus(peerIds)  
  }

  localInvitation(calleeId, channel) {
    if(this._invitation !== null) {
      this._invitation.removeAllListeners()
      this._invitation = null
    }

    //Create a local invitation
    this._invitation = this._client.createLocalInvitation(calleeId)

    //Local monitoring and inviting peers
    this._invitation.on('LocalInvitationReceivedByPeer', () => {
      log('Peers receive calls')
      this.status = 'calling'
      this.eventBus.emit('LocalInvitationReceivedByPeer')
    })

    //Cancel call invitation
    this._invitation.on('LocalInvitationCanceled', () => {
      log('Cancel call invitation')
      this.eventBus.emit('LocalInvitationCanceled')
    })

    //Called accepted call invitation
    this._invitation.on('LocalInvitationAccepted', () => {
      log('Peers accept invitations to call')
      this.status = 'meeting'
      this.eventBus.emit('LocalInvitationAccepted')
    })

    //Called down
    this._invitation.on('LocalInvitationRefused', () => {
      log('Peers refuse to call invitations')
      this.status = 'onLine'
      this.eventBus.emit('LocalInvitationRefused')
    })

    //Local call failed
    this._invitation.on('LocalInvitationFailure', () => {
      log('Call process failed')
      this.status = 'onLine'
      this.eventBus.emit('LocalInvitationFailure')
    })
    
    this._invitation.content = channel

    //Send call invitation locally
    this._invitation.send()  
  }
  
  peerInvitation() {
    //Remote monitor receives call invitation
    this._client.on('RemoteInvitationReceived', (remoteInvitation) => {
      log('Receive call invitation', remoteInvitation.callerId)
      log('remoteInvitation', remoteInvitation)
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
    //The caller has cancelled the call invitation
    this._remoteInvitation.on('RemoteInvitationCanceled', () => {
      log('The call invitation was cancelled')
      this.status = 'onLine'
      this.eventBus.emit('RemoteInvitationCanceled')
    })

    //Accepted call invitation successfully
    this._remoteInvitation.on('RemoteInvitationAccepted', () => {
      log('Peers receive calls')
      this.eventBus.emit('RemoteInvitationAccepted')
    })

    //Call invitation rejected successfully
    this._remoteInvitation.on('RemoteInvitationRefused', () => {
      log('The call invitation has been declined')
      this.eventBus.emit('RemoteInvitationRefused')
    })

    //Call invitation process failed
    this._remoteInvitation.on('RemoteInvitationFailure', () => {
      log('Peer call process failed')
      this.status = 'onLine'
      this.eventBus.emit('RemoteInvitationFailure')
    })
  }
  
  //Cancel call invitation
  cancelCall() {
    this._invitation && this._invitation.cancel()
    this.status = 'onLine'
  }

  //Accept call invitation
  acceptCall() {
    this._remoteInvitation && this._remoteInvitation.accept()
    this.status = 'meeting'
  }

  //Decline call invitation
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