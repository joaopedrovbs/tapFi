import React from 'react'
import FontIcon from 'material-ui/FontIcon';
import IconButton from 'material-ui/IconButton';
import FlatButton from 'material-ui/FlatButton';

import Formater from '../utils/Formater'
import CartState from '../stores/CartState'

import './CartItem.css'

export default class CartItem extends React.Component {
  render () {
    console.log('CartItem.render')

    if (!this.props.selected) {
      return (
        <div className="CartItem"
             onClick={() => {this.props.onSelected(this.props.item.id)}}>
          <div className="qtd">{this.props.item.qtd}</div>
          <div className="name">
            {this.props.item.name} 
            <small>({Formater(this.props.item.value)})</small>
          </div>
          <div className="total">
            {Formater(this.props.item.qtd * this.props.item.value)}
          </div>

        </div>
      )
    } else {
      let deleteIcon = <FontIcon color="#F0F0F0" className="material-icons">
        delete
      </FontIcon>

      return (
        <div className="CartItem selected">
          <IconButton onClick={() => this.changeQtdBy(-1)}>
            <FontIcon className="material-icons" color="#76FF03">remove_circle</FontIcon>
          </IconButton>
          <div className="qtd">{this.props.item.qtd}</div>
          <IconButton onClick={() => this.changeQtdBy(1)}>
            <FontIcon className="material-icons" color="#76FF03">add_circle</FontIcon>
          </IconButton>
          <div className="name">
            {this.props.item.name}
          </div>
          <FlatButton
            backgroundColor="#FF5722"
            hoverColor="#FF5722"
            icon={deleteIcon}
            onClick={() => this.removeItem()}
            style={{margin: 0, minWidth: 60, height: '100%'}}/>

        </div>
      )
    }
  }

  removeItem() {
    CartState.dispatch(CartState.action.REMOVE_ITEM(this.props.item))
  }

  changeQtdBy(amount = 1) {
    let newItem = {...this.props.item}
    newItem.qtd += amount
    newItem.qtd = Math.max(1, newItem.qtd)
    CartState.dispatch(CartState.action.CHANGE_ITEM(newItem))
  }

  shouldComponentUpdate(nextProps, nextState) {
    return this.props.item !== nextProps.item || this.props.selected !== nextProps.selected;
  }
}