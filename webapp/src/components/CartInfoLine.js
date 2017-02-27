import React from 'react'

export default class CartInfoLine extends React.Component {
  render () {
    return (
      <div className="infoLine" style={{opacity: (this.props.dim ? 0.5 : 1.0) }}>
          {this.props.label}:
          <span className="value">
            {this.props.value}
          </span>
      </div>
    )
  }
}