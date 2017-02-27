import React from 'react'
import Paper from 'material-ui/Paper';
import RaisedButton from 'material-ui/RaisedButton';

import Formater from '../utils/Formater'
import CartInfoLine from './CartInfoLine';

import './CartTotal.css'

export default class CartTotal extends React.Component {

  handlePay () {
    console.log('Paying.')
    this.props.handlePay && this.props.handlePay()
  }

  render () {
    console.log('CartTotal.render')

    var cartInfo = this.props.cartInfo

    return (
      <Paper className="CartTotal" zDepth={4}>
        <CartInfoLine dim={true}  label="Sub-total" value={Formater(cartInfo.subtotal)}/>
        <CartInfoLine dim={true}  label="Discount"  value={Formater(cartInfo.discount)}/>
        <CartInfoLine dim={true}  label="VAT"       value={Formater(cartInfo.vat)}/>
        <CartInfoLine dim={false} label="Total"     value={Formater(cartInfo.total)}/>
        <div style={{height: 18}}></div>
        <RaisedButton
          label="Pay"
          disabled={cartInfo.total <= 0}
          secondary={true}
          fullWidth={true}
          onClick={() => {this.handlePay()} }/>
      </Paper>
    )
  }
}