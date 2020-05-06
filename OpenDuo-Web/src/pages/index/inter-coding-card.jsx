import React, {useState, useEffect} from 'react'
import { makeStyles } from '@material-ui/core/styles'
import { useGlobalState, useGlobalMutation } from '../../utils/container'
import FormControl from '@material-ui/core/FormControl'
import Box from '@material-ui/core/Box'
import Button from '@material-ui/core/Button'
import { log, getRoomCode} from '../../utils/utils'
import { Link } from 'react-router-dom'
import InputCall from './inputCall'


const useStyles = makeStyles((theme) => ({
  button: {
    lineHeight: '21px',
    color: 'rgba(255,255,255,1)',
    fontSize: '17px',
    textTransform: 'none',
    height: '44px',
    width: '260px',
    '&:hover': {
      backgroundColor: '#82C2FF'
    },
    margin: theme.spacing(1),
    marginTop: '66px',
    backgroundColor: '#44a2fc',
    borderRadius: '30px'
  },
  internumber: {
    fontFamily: 'HelveticaNeue-Medium',
    fontSize: '21px',
    color: '#949494',
    textAlign: 'left',
    margin: '15px',
  },
  outlined: {
    width: '55px',
    margin: '5px',
    fontWeight: 'bold',  
  },
  interexplain: {
    fontFamily: 'PingFangSC-Medium;',
    fontSize: '14px',
    color: '#949494',
    textAlign: 'left',
    margin: '12px',
  },
}))

export default function IndexCard () {
  const classes = useStyles()

  const [code, setCode] = useState('')
  const stateCtx = useGlobalState()
  const mutationCtx = useGlobalMutation()

  const handleClickCall = () => {
    if(code.length !== 4) {
      mutationCtx.toastError('Please enter the complete code')
      return
    }
    stateCtx.rtmClient.inquire([code]).then((res) => {
      if(code === stateCtx.userCode) {
        log('Please do not enter your own code', stateCtx.userCode)
        mutationCtx.toastError('Please do not enter your own code')
        return
      }
      if(res[code]) {
        log( 'peer', code ,'is online')
        mutationCtx.updatePeerCode(code)
        let roomCode = getRoomCode(stateCtx.userCode, code)
        mutationCtx.updateConfig({ channelName: roomCode })
        stateCtx.rtmClient.localInvitation(code, roomCode)
      } else {
        log('peer is not login')
        mutationCtx.toastError('peer has not logged on')
        return
      }
    }).catch(() => {
      log('The query failed')
      mutationCtx.toastError('The query failed')
    })
  }

  const codeChange = (value) => {
    setCode(value)
  }

  return (
    <Box
      marginTop="114px"
      flex="1"
      display="flex"
      alignItems="center"
      justifyContent="flex-start"
      flexDirection="column"
    >
      <Link to="/setting" className="setting-btn" />
      <Box
        display="flex"
        alignItems="center"
        flexDirection="column"
        marginTop="15px"
      >
        <span className={classes.internumber}>Dial Number</span>
        <div>
          <div className="input-width">
            <InputCall
              onChange={codeChange}
              initCaptcha={['', '', '', '']}
            />
          </div>
        </div>
        <span className={classes.interexplain}>Enter your friend's indentifier code here</span>
        <FormControl className={classes.grid}>
          <Button
            onClick={handleClickCall}
            variant="contained"
            color="primary"
            className={classes.button}
          >
            Call
          </Button>
        </FormControl>
      </Box>
    </Box>
  )
}
