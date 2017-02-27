import React from 'react'
import Paper from 'material-ui/Paper';
import FontIcon from 'material-ui/FontIcon';
import IconButton from 'material-ui/IconButton';

import StoreItem from './StoreItem'
import CameraView from './CameraView'

import CartState from '../stores/CartState'
import StoreItems from '../stores/StoreItems'

import './Store.css'

export default class Store extends React.Component {

  state = {
    category: null,
  }

  render() {
    console.log('Store.render')
    let self = this
    let items
    let category = this.state.category

    if(category)
      items = StoreItems.itemsByCategory(category.name)

    console.log(items)

    return (
      <div className="Store">
        <Paper className="header" zDepth={2}>
          {!!category && <IconButton
              style={{margin: 6}}
              onClick={() => self.goBack()}>
              <FontIcon className="material-icons" color="#FFF">
                navigate_before
              </FontIcon>
            </IconButton>
          }
          <div className="title">
            Store
          </div>
        </Paper>

        <div className="grid">

          { !category && StoreItems.categories.map((category) => (
            <StoreItem 
              key={category.name}
              item={null}
              category={category}
              onSelected={() => { self.selectCategory(category, null)}}/>
          ))}

          {!!category && items.map((item) => (
            <StoreItem
              key={item.id}
              item={item}
              category={category}
              onSelected={() => { self.addToCart(item)}}/>
          ))}

        </div>

        <Paper className="scanner">
          <div className="product">
            
          </div>
          <div className="video">
            <CameraView width="500"/>
            <div className="line"/>
          </div>
        </Paper>

      </div>
    )
  }

  addToCart(item) {
    console.log(item)
    CartState.dispatch(CartState.action.ADD_ITEM(item))
  }

  goBack() {
    this.setState({category: null})
  }

  selectCategory(category) {
    this.setState({category})
  }
}