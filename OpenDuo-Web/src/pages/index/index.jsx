import React, { useEffect } from 'react'
import { useGlobalState, useGlobalMutation } from '../../utils/container'
import { makeStyles } from '@material-ui/core/styles'
import { Container } from '@material-ui/core'
import { log } from '../../utils/utils'
import useRouter from '../../utils/use-router'
import { useHistory } from 'react-router-dom'
import { Switch, Route } from 'react-router-dom'

import Card from '../index/card'
import Meeting from '../meeting/index'
import Calling from '../calling'
import Incoming from '../incoming'

const useStyles = makeStyles(() => ({
  container: {
    height: '100%',
    width: '100%',
    minWidth: 800,
    minHeight: 600,
    boxSizing: 'content-box',
    display: 'flex',
    justifyContent: 'center'
  }
}))

const Index = () => {
  const stateCtx = useGlobalState()
  const mutationCtx = useGlobalMutation()
  const classes = useStyles()
  const routerCtx = useRouter()

  const history = useHistory()

  useEffect(() => {
    if (stateCtx.loading === true) {
      mutationCtx.stopLoading()
    }
  }, [stateCtx.loading, mutationCtx])

  useEffect(() => {
    if (stateCtx.rtmClient.status === 'offLine') {
      stateCtx.rtmClient.login(stateCtx.userCode).then(() => {
        log( stateCtx.userCode ,'is online')
      }).catch(() => {
        log('Logon failure')
        mutationCtx.toastError('Logon failure')
        return
      })
    }
  }, [stateCtx.rtmClient])
  
  useEffect(() => {
    stateCtx.rtmClient.on('RemoteInvitationReceived', (remoteInvitation) => {
      mutationCtx.updatePeerCode(remoteInvitation.callerId)
      mutationCtx.updateIncomingCode(remoteInvitation.callerId)
      mutationCtx.updateConfig({ channelName: remoteInvitation.content })
      routerCtx.history.push({
        pathname: `/incoming/`
      })
    })

    stateCtx.rtmClient.on('RemoteInvitationCanceled', () => {
      history.goBack()
    })

    stateCtx.rtmClient.on('RemoteInvitationFailure', () => {
      mutationCtx.toastError('Call not answered')
      history.goBack()
    })

    stateCtx.rtmClient.on('RemoteInvitationAccepted', () => {
      log('Accept success')
      mutationCtx.startLoading()
      let channel = stateCtx.config.channelName
      log('channel id', channel)
      routerCtx.history.push({
        pathname: `/meeting/${channel}`
      }) 
    })

    stateCtx.rtmClient.on('LocalInvitationAccepted', () => {
      mutationCtx.startLoading()
      let channel = stateCtx.config.channelName
      log('channel id', channel)
      routerCtx.history.push({
        pathname: `/meeting/${channel}`
      })
    }) 

    stateCtx.rtmClient.on('LocalInvitationRefused', () => {
      mutationCtx.toastError('Peer is busy')
      history.goBack()
    })

    stateCtx.rtmClient.on('RemoteInvitationRefused', () => {
      history.goBack()
    })

    stateCtx.rtmClient.on('LocalInvitationCanceled', () => {
      history.goBack()
    })

    stateCtx.rtmClient.on('LocalInvitationReceivedByPeer', () => {
      routerCtx.history.push({
        pathname: `/calling/`
      })
    })

    stateCtx.rtmClient.on('LocalInvitationFailure', () => {
      mutationCtx.toastError('Call process failed')
      history.goBack()
    })

    return () => {
      stateCtx.rtmClient.eventBus.removeAllListeners()
    }
  }, [stateCtx.rtmClient, stateCtx])


  return (
    <Container maxWidth="sm" className={classes.container}>
      <Switch>
        <Route exact path="/meeting/:name" component={Meeting}></Route>
        <Route exact path="/calling" component={Calling}></Route>
        <Route exact path="/incoming" component={Incoming}></Route>
        <Route path="/" component={Card}></Route>
      </Switch>
    </Container>
  )
}

export default Index
