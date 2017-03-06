import React from 'react'
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import RaisedButton from 'material-ui/RaisedButton';

import Devices from '../stores/Devices';
import CartState from '../stores/CartState';

import TapFiUser from './TapFiUser';

import './PayDialog.css';

export default class PayDialog extends React.Component {
  state = {
    step: 'PAYMENT_METHOD',
    device: null,
    devices: [],
  }

  constructor(props) {
    super(props)

    Devices.subscribe(() => {
      this.setState({devices: Devices.getState()})
    })
  }

  render() {
    // Change it's state to 

    const actions = [
      <FlatButton
        label="Cancel"
        primary={true}
        onTouchTap={this.props.handleClose}
      />,
    ];

    let content, title

    if (this.state.step === 'PAYMENT_METHOD') {
      title = 'Payment Method'
      const style = { margin: 12 }
      const methods = [
        'Cash',
        'Check',
        'Debit Card',
        'Credit Card',
        'tapFi',
      ]

      content = (<div className="cards">
        {methods.map( method => (
          <RaisedButton label={method} primary={true} style={style} onClick={() => this.handleSelectMethod(method) }/>
        ))}
      </div>)
    } else if (this.state.step === 'PAY_TAPFI_SEARCH') {
      // const testUser = {
      //   name: 'Ivan Seidel',
      //   acc: 'admin@best-ilp.herokuapp.com',
      //   thrusted: true,
      //   avatar: 'https://avatars0.githubusercontent.com/u/3102127?v=3&s=460',
      //   status: 'found',
      //   timestamp: Date.now(),
      // }

      title = 'Looking for tapFi...'
      content = (
        <div className="cards">
          {this.state.devices.map((device) => (
            <TapFiUser device={device} key={device.id} onSelect={() => {this.handleSelectDevice(device)}}/> 
          ))}
        </div>
      )
    } else if (this.state.step === 'PAY_WAIT_CONFIRMATION') {
      let device = this.state.devices.find(d => d.id == this.state.device)
      // If not found, go back
      if (!device) {
        return this.setState({
          step: 'PAYMENT_METHOD',
          device: null,
        })
      }

      title = 'Paying: ' + device.name
      content = (
        <div className="cards">
          <TapFiUser device={device} key={device.id} onSelect={() => {}}/> 
        </div>
      )
    } else {
      title = 'Invalid'
      content = 'Invalid'
    }


    return (
      <div>
        <Dialog className="PayDialog"
          title={title}
          actions={actions}
          modal={false}
          open={this.props.open}
          onRequestClose={this.props.handleClose}>
          {content}
        </Dialog>
      </div>
    );
  }

  handleSelectMethod(method) {
    if (method === 'tapFi') {
      this.setState({step: 'PAY_TAPFI_SEARCH'})
    } else {
      this.props.handleClose && this.props.handleClose()
    }
  }

  handleSelectDevice(device) {
    this.setState({
      device: device.id,
      step: 'PAY_WAIT_CONFIRMATION'
    })

    // Get value from cart
    let value = CartState.getState().cartInfo.total
    let deviceId = device.id

    // Request payment
    window['require'] && window['require']('electron').ipcRenderer.send('pay', deviceId, value)
  }

  componentWillReceiveProps(nextProps) {
    if (nextProps.open === false)
      this.setState({step: 'PAYMENT_METHOD', device: null})
  }
}