import React from 'react'

import CameraUtils from '../utils/CameraUtils'

export default class CameraView extends React.Component {

  state = {
    src: null,
    // device: /./g,
    device: /Logitech Webcam C930e/g,
  }

  stream = null

  // constructor(props) {
  //   super(props)
  // }

  componentDidMount() {
    this.connectToCamera()
  }

  connectToCamera() {
    let self = this 
    if (self.state.src !== null)
      return

    CameraUtils.getVideoDevices( (devices) => {
      let device = CameraUtils.getBestDevice(devices, self.state.device)

      CameraUtils.connectToDevice(device, (error, stream) => {
        console.log(window.URL.createObjectURL(stream))
        self.setState({src: window.URL.createObjectURL(stream)})
      })
    })
  }

  render() {
    console.log('CameraView.render')
    
    return (
      <video autoPlay src={this.state.src} width="100%"></video>
    )
  }
}