import React, {useEffect, useState} from "react"
import "./inputCall.css";

function InputCall ({ onChange, initCaptcha }){

  const [activeIndex, setActiveIndex] = useState(0)
  const [captcha, setCaptcha] = useState(initCaptcha)
  const [inputEl, setInputEl] = useState(null)
  const [begin, setBegin] = useState(false)

  const inputKeyDown = (event, index) => {
    let keyCode = event.keyCode
    // 8 is rollback
    if (keyCode === 8) {
      backSpace(index)
    }
  }
  
  useEffect(() => {
    if (begin) {
      inputEl && inputEl.focus()
    }
  }, [inputEl, begin])

  const backSpace = (index) => {
    let preIndex = index - 1
    if (captcha[index] !== '') {
      fixCaptchaByIndex(index, '')
    } else {
      fixCaptchaByIndex(preIndex, '')
    }
    if(index !== 0) {
      setActiveIndex(preIndex)
    }
  }

  const inputChange = (event, index) => {
    let allowValue = "0123456789"
    let value = event.target.value
    let next = index + 1
    if (!allowValue.includes(value)) {
      return
    }
    if (captcha[index] === '') {
      fixCaptchaByIndex(index, value)
    } else {
      fixCaptchaByIndex(next, value)
    }
    if(index !== captcha.length - 1) {
      setActiveIndex(next)
    }
  }

  const fixCaptchaByIndex = (index, value) => {
    let arr = [...captcha]
    if (arr[index] === value) {
      return
    }
    arr[index] = value
    onChange(arr.join(''))
    if (index === arr.length - 1 && arr[index] !== '') {
      inputEl && inputEl.blur()
    }
    setCaptcha(arr)
  }

  return (
    <div
      className={`captcha-input-box`}
      onClick={() => {
        setBegin(true)
        inputEl && inputEl.focus()
      }}
    >
      {captcha.map((v, index) => {
        let isActive = activeIndex === index
        return (
          <div className="captcha-input-wrap" key={index}>
            <input
              className="captcha-input-item"
              type="tel"
              name="register-form-items"
              autoComplete="off"
              autoCapitalize="off"
              maxLength={1}
              disabled={!isActive}
              value={v}
              onChange={e => inputChange(e, index)}
              onKeyDown={e => inputKeyDown(e, index)}
              ref={el => {
                if (isActive) {
                  setInputEl(el)
                }
              }}
            />
          </div>
        )
      })}
    </div>
  )
}

export default InputCall