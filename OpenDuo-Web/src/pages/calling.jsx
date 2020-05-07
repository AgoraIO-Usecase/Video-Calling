import React, {useState, useEffect}  from 'react'
import { makeStyles } from '@material-ui/core/styles'
import Box from '@material-ui/core/Box'
import Card from '@material-ui/core/Card'
import { log } from '../utils/utils'
import { useGlobalState } from '../utils/container'


const useStyles = makeStyles(theme => ({
  call: {
    color: '#fff'
  },
  midItem: {
    marginTop: '1rem',
    marginBottom: '6rem'
  },
  item: {
    flex: 1,
    display: 'flex',
    alignItems: 'center'
  },
  coverCall: {
    background: 'linear-gradient(to bottom, #307AFF, 50%, #46cdff)',
    alignItems: 'center',
    flex: 1,
    display: 'flex',
    flexDirection: 'column'
  },
  coverContent: {
    display: 'flex',
    justifyContent: 'center',
    flexDirection: 'column',
    color: '#fff',
    position: 'relative'
  },
  container: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center'
  },
  card: {
    display: 'flex',
    minWidth: 350,
    maxWidth: 350,
    minHeight: 500,
    maxHeight: 500,
    borderRadius: '10px',
    boxShadow: '0px 6px 18px 0px rgba(0,0,0,0.2)'
  },
  input: {
    maxWidth: '250px',
    minWidth: '250px',
    alignSelf: 'center'
  },
  grid: {
    margin: '0 !important'
  },
  wait: {
    fontFamily: 'HelveticaNeue',
    fontSize: '18px',
    color: '#ffffff',
    textAlign: 'left',
    marginTop: '-78px'
  },
  number: {
    fontFamily: 'HelveticaNeue',
    fontSize: '48px',
    color: '#ffffff',
    letterSpacing: '10px',
    textAlign: 'left',
  },
  button: {
    height: '44px',
    width: '260px',
    '&:hover': {
      backgroundColor: '#307AFF'
    },
    margin: theme.spacing(1),
    marginTop: '33px',
    backgroundColor: '#44a2fc',
    borderRadius: '30px'
  }
}))

export default function CardPage () {
  const classes = useStyles()
  const stateCtx = useGlobalState()
  const [cancel, setCancel] = useState(false)

  const handleClickEndCall = () => {
    stateCtx.rtmClient.cancelCall()
  }

  const mouseOver = () => {
    setCancel(true)
  }

  const mouseOut = () => {
    setCancel(false)
  }

  return (
    <Box display="flex" alignItems="center" justifyContent="center" width="100%" height="100%">
      <Card className={classes.card}>
        <Box display="flex" flex="1" >
          <div className={classes.coverCall}>
            <div className={classes.item}>
              <div className='head-box'>
                <div className='agora-pluse'></div>
                <div className='head-image' />
              </div>
            </div>
            <div className={classes.item}>
              <div className={classes.number}>{stateCtx.peerCode}</div>
            </div>
            <div className={classes.item}>
              <div className={classes.wait}>Calling...</div>
            </div>
            <div className={classes.item}>
              <div
                onMouseOver={mouseOver} onMouseOut={mouseOut}
                className={cancel ? 'endCall-image-calling-pressed' : 'endCall-image-calling'}
                onClick={handleClickEndCall}
              />
            </div>
          </div>
        </Box>
      </Card>
    </Box>
  )
}
