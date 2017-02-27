import React from 'react'
import Paper from 'material-ui/Paper';
import Badge from 'material-ui/Badge';
import Avatar from 'material-ui/Avatar';
import FontIcon from 'material-ui/FontIcon';

import './TapFiUser.css'

export default class CartItem extends React.Component {
  render () {
    console.log('TapFiUser.render')

    let safetyIcon
    let badgeStyle = {top: 16, right: 16, width: 32, height: 32, fontSize: 12}
    let statusClass = `status ${this.props.userInfo.status} ${this.props.userInfo.thrusted ? 'thrust' : ''}`

    if (this.props.userInfo.thrusted) {
      badgeStyle.backgroundColor = "#64DD17"
      safetyIcon = <FontIcon color="#F0F0F0" className="material-icons" style={{fontSize: 18}}>security</FontIcon>
    } else {
      badgeStyle.backgroundColor = "#F44336"
      safetyIcon = <FontIcon color="#F0F0F0" className="material-icons" style={{fontSize: 18}}>lock</FontIcon>
    }


    return (
      <Paper className="TapFiUser show" zDepth={3}>
        <Badge
          className="avatar"
          badgeContent={safetyIcon}
          secondary={true}
          badgeStyle={badgeStyle}>
          <Avatar
            src={this.props.userInfo.avatar}
            size={96}/>
        </Badge>
        <p className="name">{this.props.userInfo.name}</p>
        <p className="acc">{this.props.userInfo.acc}</p>
        <div className={statusClass}>
          {this.props.userInfo.status}
        </div>
      </Paper>
    )
  }
}