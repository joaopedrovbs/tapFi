import React from 'react'
import Paper from 'material-ui/Paper';
import FontIcon from 'material-ui/FontIcon';

import CartItem from './CartItem'
import CartTotal from './CartTotal'

import './Cart.css'

import CartState from '../stores/CartState'

export default class Cart extends React.Component {

  constructor(props) {
    super(props)

    this.state = Object.assign(
      {},
      CartState.getState(),
      {selectedId: null}
    )

    CartState.subscribe(() => {
      this.setState(CartState.getState())
    })
  }

  handlePay() {
    CartState.dispatch(CartState.action.ADD_ITEM({
      id: 'lol'+Math.random(),
      name: 'Popcorn',
      value: 5.4,
      qtd: 1,
    }))
  }

  handleSelection(itemId, deselect = false) {
    console.log('Selected', itemId)
    this.setState({
      selectedId: itemId
    })
  }

  render() {
    // console.log('Cart.render')

    var self = this
    var cart = this.state.cart

    return (
      <Paper className="Cart" zDepth={2}>
        <Paper className="header" zDepth={2}>
          <div className="title">
            Cart
          </div>
          <div className="icon">
            <FontIcon color="#F0F0F0" className="material-icons">shopping_cart</FontIcon>
          </div>
        </Paper>

        { !cart.length && <div className="emptyCart">
          <div>Empty cart</div>
        </div>}

        {!!cart.length && <div className="StoreItems">
          {cart.map(function (object, i){ 
            return <CartItem 
              item={object}
              key={object.id}
              selected={self.state.selectedId === object.id}
              onSelected={self.handleSelection.bind(self) }/>;
          })}
        </div>}

        <CartTotal 
          cart={this.state.cart}
          cartInfo={this.state.cartInfo}
          handlePay={() => this.props.handlePay && this.props.handlePay()}/>
      </Paper>
    )
  }
}