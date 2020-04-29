const log = console.log.bind(console)

const produceCode = () => {
  let code = ''
  for(let i = 0; i < 4; i++) {
    let number = Math.floor(Math.random()*10)
    code += number
  }
  return code
}

const getUserCode = () => {
  let localStorageCode = localStorage.getItem('usercode')
  if(localStorageCode === null) {
    var usercode = produceCode()
    localStorage.setItem('usercode', usercode)
    return usercode
  }
  return localStorageCode
}

const getRoomCode = (c1, c2) => {
  let roomCode = ''
  roomCode = c1 + c2
  return roomCode
}

export { 
  log,
  produceCode,
  getUserCode,
  getRoomCode,
}