import React from 'react'

import Formater from '../utils/Formater'

import './StoreItem.css'

export default class StoreItem extends React.Component {
  render () {
    console.log('StoreItem.render')
    
    let props = this.props
    let onSelected = props.onSelected
    let isCategory = !props.item

    let styles = {
      borderColor: props.category.color
    }

    if (isCategory)
      styles['backgroundColor'] = props.category.color
    else 
      styles['color'] = props.category.color

    return (
      <div
        className="StoreItem" 
        style={styles}
        onClick={() => {onSelected && onSelected()}}>

        { isCategory && <div className="title category">
          { props.category.name}
        </div>}

        {!isCategory && <div className="title item">
          { props.item.name}
          <br/>
          <small>{ Formater(props.item.value) }</small>
        </div>}
      </div>
    )
  }
}