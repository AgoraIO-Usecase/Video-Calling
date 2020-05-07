import React from 'react'
import './App.css'
import { Route, Switch } from 'react-router-dom'
import Index from './pages/index'
import { BrowserRouterHook } from './utils/use-router'
function App () {
  return (
    <BrowserRouterHook>
      <Switch>
        <Route path="/" component={Index}></Route>
      </Switch>
    </BrowserRouterHook>
  )
}

export default App
