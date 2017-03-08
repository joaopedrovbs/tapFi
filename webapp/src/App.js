import React from 'react'
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider'

import './App.css'

import Cart from './components/Cart'
import Store from './components/Store'
import PayDialog from './components/PayDialog'

import Devices from './stores/Devices'
import CartState from './stores/CartState'
import StoreItems from './stores/StoreItems'

class App extends React.Component {
  state = {
    payWindowOpen: true,
  }

  render() {
    console.log('App.render')

    return (
      <MuiThemeProvider className="App">
        <div className="content">
          <Store/>
          <Cart handlePay={this.handlePay.bind(this)}/>
          <PayDialog
            open={this.state.payWindowOpen}
            handleClose={this.handlePayWindowClose.bind(this)}/>
        </div>
      </MuiThemeProvider>
    )
  }

  handlePayWindowClose() {
    console.log('handle close')
    this.setState({payWindowOpen: false})
  }

  handlePay() {
    console.log('Request pay')
    this.setState({payWindowOpen: true})
  }
}

// Listen to barcodes
var audio = new Audio('./assets/audio.wav');
window['require'] && window['require']('electron').ipcRenderer.on('barcode', (event, barcode) => {
  console.log('Barcode: ', barcode)

  // Find product for barcode specified 
  let item = StoreItems.items.find(item => item.barcode === barcode)
  if (item) {
    audio.play()
    CartState.dispatch(CartState.action.ADD_ITEM(item))
  }
})

// Listen to device changes
window['require'] && window['require']('electron').ipcRenderer.on('devices', (event, devices) => {
  console.log('devices:', devices)
  Devices.dispatch(Devices.action.UPDATE(devices))
})

// Listen to payEvents
window['require'] && window['require']('electron').ipcRenderer.on('didPay', (event, success) => {
  console.log('didPay:', success)
  CartState.dispatch(CartState.action.CLEAR())
})

export default App