import React, { useEffect, useState, useMemo } from 'react'
import clsx from 'clsx'
import { useGlobalState, useGlobalMutation } from '../utils/container'
import { makeStyles } from '@material-ui/core/styles'
import useRouter from '../utils/use-router'
import useStream from '../utils/use-stream'
import RTCClient from '../rtc-client'
import Tooltip from '@material-ui/core/Tooltip'
import StreamPlayer from './meeting/stream-player'

const useStyles = makeStyles({
  menu: {
    height: '150px',
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center'
  },
  customBtn: {
    width: '50px',
    height: '50px',
    marginLeft: '20px',
    borderRadius: '26px',
    backgroundColor: 'rgba(0, 0, 0, 0.4)',
    backgroundSize: '50px',
    cursor: 'pointer'
  },
  leftAlign: {
    display: 'flex',
    flex: '1',
    justifyContent: 'space-evenly'
  },
  rightAlign: {
    display: 'flex',
    flex: '1',
    justifyContent: 'center'
  },
  menuContainer: {
    width: '100%',
    height: '100%',
    position: 'absolute',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'flex-end',
    zIndex: '2'
  }
})

const MeetingPage = () => {
  const classes = useStyles()

  const routerCtx = useRouter()
  const stateCtx = useGlobalState()
  const mutationCtx = useGlobalMutation()
  
  const localClient = useMemo(() => {
    const client = new RTCClient()
    if (!client._created) {
      client.createClient({ codec: stateCtx.codec, mode: stateCtx.mode })
      client._created = true
    }
    return client
  }, [stateCtx.codec, stateCtx.mode])

  const [localStream, currentStream] = useStream(localClient)
  const [muteVideo, setMuteVideo] = useState(stateCtx.muteVideo)
  const [muteAudio, setMuteAudio] = useState(stateCtx.muteAudio)

  const config = useMemo(() => {
    return {
      token: stateCtx.config.token,
      channel: stateCtx.config.channelName,
      microphoneId: stateCtx.config.microphoneId,
      cameraId: stateCtx.config.cameraId,
      resolution: stateCtx.config.resolution,
      muteVideo: muteVideo,
      muteAudio: muteAudio,
      uid: stateCtx.uid,
      host: stateCtx.config.host
      // beauty: stateCtx.beauty
    }
  }, [stateCtx, muteVideo, muteAudio])

  useEffect(() => {
    return () => {
      localClient && localClient.leave(() => mutationCtx.clearAllStream())
    }
  }, [localClient])

  const history = routerCtx.history

  const params = new URLSearchParams(window.location.search)

  useEffect(() => {
    const roleParams = params.get('role')
    if (!config.channel && roleParams !== 'audience') {
      history.push('/')
    }
  }, [config.channel, history, params])

  useEffect(() => {
    if (
      config.channel &&
      localClient._created &&
      localClient._joined === false
    ) {
      localClient
        .join(config)
        .then((uid) => {
          if (config.host) {
            localClient.publish()
          }
          mutationCtx.updateConfig({ uid })
          mutationCtx.stopLoading()
        })
        .catch((err) => {
          mutationCtx.toastError(`Media ${err.info}`)
          routerCtx.history.push('/')
        })
    }
  }, [localClient, mutationCtx, config, routerCtx])

  const handleClick = (name) => {
    return (evt) => {
      evt.stopPropagation()
      switch (name) {
        case 'video': {
          muteVideo
            ? localStream.muteVideo()
            : localStream.unmuteVideo()
          setMuteVideo(!muteVideo)
          break
        }
        case 'audio': {
          muteAudio
            ? localStream.muteAudio()
            : localStream.unmuteAudio()
          setMuteAudio(!muteAudio)
          break
        }
        case 'screen': {
          if (stateCtx.screen) {
            localClient
              .createRTCStream({
                token: null,
                channel: stateCtx.config.channelName,
                microphoneId: stateCtx.config.microphoneId,
                resolution: stateCtx.config.resolution,
                muteVideo: muteVideo,
                muteAudio: muteAudio
                // beauty: stateCtx.beauty,
              })
              .then(() => {
                localClient.publish()
                mutationCtx.setScreen(false)
              })
              .catch((err) => {
                console.log(err)
                mutationCtx.toastError(`Media ${err.info}`)
                routerCtx.history.push('/')
              })
          } else {
            localClient
              .createScreenSharingStream({
                token: null,
                channel: stateCtx.config.channelName,
                microphoneId: stateCtx.config.microphoneId,
                cameraId: stateCtx.config.cameraId,
                resolution: stateCtx.config.resolution
              })
              .then(() => {
                localClient.publish()
                mutationCtx.setScreen(true)
              })
              .catch((err) => {
                console.log(err)
                mutationCtx.toastError(`Media ${err.info}`)
              })
          }
          break
        }
        case 'profile': {
          break
        }
        default:
          throw new Error(`Unknown click handler, name: ${name}`)
      }
    }
  }

  const handleDoubleClick = (stream) => {
    mutationCtx.setCurrentStream(stream)
  }

  const otherStreams = useMemo(() => {
    return stateCtx.streams.filter(
      (it) => it.getId() !== currentStream.getId()
    )
  }, [stateCtx.streams, currentStream])

  return (
    <div className="meeting">
      <div className="current-view">
        <div className="nav">
          <div className="avatar-container">
            <div className="default-avatar"></div>
            <div className="avatar-text">My Room</div>
            <div className="like"></div>
          </div>
          <Tooltip title="quit">
            <div
              className="quit"
              onClick={() => {
                localClient.leave().then(() => {
                  mutationCtx.clearAllStream()
                  // mutationCtx.resetState()
                  routerCtx.history.push('/')
                })
              }}
            ></div>
          </Tooltip>
        </div>
        {currentStream ? (
          <StreamPlayer
            className={'main-stream-profile'}
            showProfile={stateCtx.profile}
            local={
              config.host
                ? currentStream &&
                  localStream &&
                  currentStream.getId() === localStream.getId()
                : false
            }
            stream={currentStream}
            onDoubleClick={handleDoubleClick}
            uid={currentStream.getId()}
            domId={`stream-player-${currentStream.getId()}`}
          >
            <div className={classes.menuContainer}>
              {config.host && (
                <div className={classes.menu}>
                  <Tooltip title={muteVideo ? 'mute-video' : 'unmute-video'}>
                    <i
                      onClick={handleClick('video')}
                      className={clsx(
                        classes.customBtn,
                        muteVideo ? 'mute-video' : 'unmute-video'
                      )}
                    />
                  </Tooltip>
                  <Tooltip title={muteAudio ? 'mute-audio' : 'unmute-audio'}>
                    <i
                      onClick={handleClick('audio')}
                      className={clsx(
                        classes.customBtn,
                        muteAudio ? 'mute-audio' : 'unmute-audio'
                      )}
                    />
                  </Tooltip>
                  <Tooltip title={stateCtx.screen ? 'stop-screen-share' : 'start-screen-share'}>
                    <i
                      onClick={handleClick('screen')}
                      className={clsx(
                        classes.customBtn,
                        stateCtx.screen
                          ? 'start-screen-share'
                          : 'stop-screen-share'
                      )}
                    />
                  </Tooltip>
                  
                  {/* <i onClick={handleClick('profile')} className={clsx(classes.customBtn, 'show-profile')}/> */}
                </div>
              )}
            </div>
          </StreamPlayer>
        ) : null}
        <div className="stream-container">
          {stateCtx.otherStreams.map((stream, index) => (
            <StreamPlayer
              className={'stream-profile'}
              showProfile={stateCtx.profile}
              local={
                config.host
                  ? stream.getId() === localStream && localStream.getId()
                  : false
              }
              key={`${index}${stream.getId()}`}
              stream={stream}
              isPlaying={stream.isPlaying()}
              uid={stream.getId()}
              domId={`stream-player-${stream.getId()}`}
              onDoubleClick={handleDoubleClick}
              showUid={true}
            ></StreamPlayer>
          ))}
        </div>
      </div>
    </div>
  )
}

export default React.memo(MeetingPage)
