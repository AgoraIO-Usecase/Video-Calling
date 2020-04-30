import AgoraRTC from 'agora-rtc-sdk'
import EventEmitter from 'events'
const appID = process.env.REACT_APP_AGORA_APP_ID
console.log(
  'agora sdk version: ' +
    AgoraRTC.VERSION +
    ' compatible: ' +
    AgoraRTC.checkSystemRequirements()
)
export default class RTCClient {
  constructor () {
    this._client = null
    this._joined = false
    this._localStream = null
    // this._enableBeauty = false;
    this._params = {}
    this._uid = 0
    this._eventBus = new EventEmitter()
    this._showProfile = false
    this._subscribed = false
    this._created = false
  }

  createClient (data) {
    this._client = AgoraRTC.createClient({
      mode: data.mode,
      codec: data.codec
    })
    return this._client
  }

  closeStream () {
    if (this._localStream.isPlaying()) {
      this._localStream.stop()
    }
    this._localStream.close()
  }

  destroy () {
    this._created = false
    this._client = null
  }

  on (evt, callback) {
    const customEvents = ['localStream-added', 'screenShare-canceled']

    if (customEvents.indexOf(evt) !== -1) {
      this._eventBus.on(evt, callback)
      return
    }

    this._client.on(evt, callback)
  }

  setClientRole (role) {
    this._client.setClientRole(role)
  }

  createRTCStream (data) {
    return new Promise((resolve, reject) => {
      this._uid = this._localStream ? this._localStream.getId() : data.uid
      if (this._localStream) {
        this.unpublish()
        this.closeStream()
      }
      // create rtc stream
      const rtcStream = AgoraRTC.createStream({
        streamID: this._uid,
        audio: true,
        video: true,
        screen: false,
        microphoneId: data.microphoneId,
        cameraId: data.cameraId
      })

      if (data.resolution && data.resolution !== 'default') {
        rtcStream.setVideoProfile(data.resolution)
      }

      // init local stream
      rtcStream.init(
        () => {
          this._localStream = rtcStream
          this._eventBus.emit('localStream-added', {
            stream: this._localStream
          })
          if (data.muteVideo === false) {
            this._localStream.muteVideo()
          }
          if (data.muteAudio === false) {
            this._localStream.muteAudio()
          }
          // if (data.beauty === true) {
          //   this._localStream.setBeautyEffectOptions(true, {
          //     lighteningContrastLevel: 1,
          //     lighteningLevel: 0.7,
          //     smoothnessLevel: 0.5,
          //     rednessLevel: 0.1
          //   })
          //   this._enableBeauty = true;
          // }
          resolve()
        },
        (err) => {
          reject(err)
          // Toast.error("stream init failed, please open console see more detail");
          console.error('init local stream failed ', err)
        }
      )
    })
  }

  createScreenSharingStream (data) {
    return new Promise((resolve, reject) => {
      // create screen sharing stream
      this._uid = this._localStream ? this._localStream.getId() : data.uid
      if (this._localStream) {
        this._uid = this._localStream.getId()
        this.unpublish()
      }
      const screenSharingStream = AgoraRTC.createStream({
        streamID: this._uid,
        audio: true,
        video: false,
        screen: true,
        microphoneId: data.microphoneId,
        cameraId: data.cameraId
      })

      if (data.resolution && data.resolution !== 'default') {
        screenSharingStream.setScreenProfile(`${data.resolution}_1`)
      }

      screenSharingStream.on('stopScreenSharing', (evt) => {
        this._eventBus.emit('stopScreenSharing', evt)
        this.closeStream()
        this.unpublish()
      })

      // init local stream
      screenSharingStream.init(
        () => {
          this.closeStream()
          this._localStream = screenSharingStream

          // run callback
          this._eventBus.emit('localStream-added', {
            stream: this._localStream
          })
          resolve()
        },
        (err) => {
          this.publish()
          reject(err)
        }
      )
    })
  }

  subscribe (stream, callback) {
    this._client.subscribe(stream, callback)
  }

  _createTmpStream () {
    this._uid = 0
    return new Promise((resolve, reject) => {
      if (this._localStream) {
        this._localStream.close()
      }
      // create rtc stream
      const _tmpStream = AgoraRTC.createStream({
        streamID: this._uid,
        audio: true,
        video: true,
        screen: false
      })

      // init local stream
      _tmpStream.init(
        () => {
          this._localStream = _tmpStream
          resolve()
        },
        (err) => {
          reject(err)
          // Toast.error("stream init failed, please open console see more detail");
          console.error('init local stream failed ', err)
        }
      )
    })
  }

  getDevices () {
    return new Promise((resolve, reject) => {
      if (!this._client) {
        this.createClient({ codec: 'vp8', mode: 'live' })
      }
      this._createTmpStream().then(() => {
        AgoraRTC.getDevices((devices) => {
          this._localStream.close()
          resolve(devices)
        })
      })
    })
  }

  setStreamFallbackOption (stream, type) {
    this._client.setStreamFallbackOption(stream, type)
  }

  enableDualStream () {
    return new Promise((resolve, reject) => {
      this._client.enableDualStream(resolve, reject)
    })
  }

  setRemoteVideoStreamType (stream, streamType) {
    this._client.setRemoteVideoStreamType(stream, streamType)
  }

  join (data) {
    this._joined = 'pending'
    return new Promise((resolve, reject) => {
      /**
       * A class defining the properties of the config parameter in the createClient method.
       * Note:
       *    Ensure that you do not leave mode and codec as empty.
       *    Ensure that you set these properties before calling Client.join.
       *  You could find more detail here. https://docs.agora.io/en/Video/API%20Reference/web/interfaces/agorartc.clientconfig.html
       **/

      this._params = data

      // handle AgoraRTC client event
      // this.handleEvents();

      // init client
      this._client.init(
        appID,
        () => {
          this._client.join(
            data.token ? data.token : null,
            data.channel,
            data.uid ? +data.uid : null,
            (uid) => {
              this._uid = uid
              // Toast.notice("join channel: " + data.channel + " success, uid: " + uid);
              console.log(
                'join channel: ' + data.channel + ' success, uid: ' + uid
              )
              this._joined = true

              data.uid = uid

              if (data.host) {
                this.createRTCStream(data)
                  .then(() => {
                    this.enableDualStream()
                      .then(() => {
                        this.setRemoteVideoStreamType(this._localStream, 0)
                        resolve(data.uid)
                      })
                      .catch((err) => {
                        reject(err)
                      })
                  })
                  .catch((err) => {
                    reject(err)
                  })
              } else {
                resolve()
              }
            },
            (err) => {
              this._joined = false
              reject(err)
              console.error('client join failed', err)
            }
          )
        },
        (err) => {
          this._joined = false
          reject(err)
          console.error(err)
        }
      )
    })
  }

  publish () {
    // publish localStream
    this._client.publish(this._localStream, (err) => {
      console.error(err)
    })
  }

  unpublish () {
    if (!this._client) {
      return
    }
    this._client.unpublish(this._localStream, (err) => {
      console.error(err)
    })
  }

  leave () {
    return new Promise((resolve) => {
      if (!this._client) return resolve()
      // leave channel
      this._client.leave(
        () => {
          this._joined = false
          this.destroy()
          // if (this._localStream && this._enableBeauty) {
          //   this._localStream.setBeautyEffectOptions(false);
          // }
          resolve()
        },
        (err) => {
          console.log('channel leave failed')
          console.error(err)
        }
      )
    })
  }
}
