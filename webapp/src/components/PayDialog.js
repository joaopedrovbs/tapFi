import React from 'react'
import Dialog from 'material-ui/Dialog';
import FlatButton from 'material-ui/FlatButton';
import RaisedButton from 'material-ui/RaisedButton';

import TapFiUser from './TapFiUser';

import './PayDialog.css';

export default class PayDialog extends React.Component {
  state = {
    step: 'PAY_TAPFI_SEARCH'
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
      const testUser = {
        name: 'Ivan Seidel',
        acc: 'admin@best-ilp.herokuapp.com',
        thrusted: true,
        avatar: 'https://avatars0.githubusercontent.com/u/3102127?v=3&s=460',
        status: 'found',
        timestamp: Date.now(),
      }

      title = 'Looking for tapFi...'
      content = (
        <div className="cards">
          <TapFiUser userInfo={testUser}/>
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

  componentWillReceiveProps(nextProps) {
    if (nextProps.open === false)
      this.setState({step: 'PAYMENT_METHOD'})
  }
}