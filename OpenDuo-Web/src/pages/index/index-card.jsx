import React, { useEffect } from 'react'
import { makeStyles } from '@material-ui/core/styles'
import { useGlobalState , useGlobalMutation} from '../../utils/container'
import FormControl from '@material-ui/core/FormControl'
import Box from '@material-ui/core/Box'
import Button from '@material-ui/core/Button'
import useRouter from '../../utils/use-router'
import { log } from '../../utils/utils'
import { Link } from 'react-router-dom'


const useStyles = makeStyles((theme) => ({
  midItem: {
    marginTop: '1rem',
    marginBottom: '6rem'
  },
  item: {
    flex: 1,
    display: 'flex',
    alignItems: 'center'
  },
  container: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center'
  },
  card: {
    display: 'flex',
    minWidth: 700,
    minHeight: 500,
    maxHeight: 500,
    borderRadius: '10px',
    boxShadow: '0px 6px 18px 0px rgba(0,0,0,0.2)'
  },
  grid: {
    margin: '0 !important'
  },
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
  identity: {
    fontfamily: 'HelveticaNeue',
    fontSize: '21px',
    color: '#949494',
    textAlign: 'left',
    marginTop: '30px',
  },
  coding: {
    fontSize: '48px',
    fontfamily: 'HelveticaNeue',
    color: '#949494',
    textAlign: 'left',
    margin: '15px',
  },
  explain: {
    fontSize: '12px',
    fontfamily: 'HelveticaNeue',
    color: '#949494',
    textAlign:'center',
  }
}))

export default function IndexCard () {
  const classes = useStyles()

  const routerCtx = useRouter()
  const stateCtx = useGlobalState()
  const mutationCtx = useGlobalMutation()

  const handleClick = () => {
    if(stateCtx.rtmClient.status === 'onLine') {
      routerCtx.history.push({
        pathname: `/dialing/`
      })
    } else {
      mutationCtx.toastError('You are not currently online, Please refresh the webpage')
    }
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
      <a
        href="https://github.com/AgoraIO/Basic-Video-Broadcasting/tree/master/OpenLive-Web"
        className="github"
      ></a>
      <Box
        display="flex"
        alignItems="center"
        justifyContent="center"
        flexDirection="column"
      >
        <span className={classes.identity}>Your identifier code</span>
        <span className={classes.coding}>{stateCtx.userCode}</span>
        <span className={classes.explain}>Share your code</span>
        <span className={classes.explain}>so that others call you</span>
        <FormControl className={classes.grid}>
          <Button
            onClick={handleClick}
            variant="contained"
            color="primary"
            className={classes.button}
          >
            Start calling your friend
          </Button>
        </FormControl>
      </Box> 
    </Box>
  )
}
