import React from 'react'
import Paper from 'material-ui/Paper';
import Badge from 'material-ui/Badge';
import Avatar from 'material-ui/Avatar';
import FontIcon from 'material-ui/FontIcon';

import './TapFiUser.css'

export default class CartItem extends React.Component {
  render () {
    console.log('TapFiUser.render')

    let paid = this.props.device.status === 'Payment complete'
    let infoOk = !!this.props.device.info

    let safetyIcon
    let badgeStyle = {top: 16, right: 16, width: 32, height: 32, fontSize: 12}
    let statusClass = `status ${paid ? 'paid' : (infoOk ? 'found' : '')}`

    let info = this.props.device.info || {
      avatar: 'https://www.kirkleescollege.ac.uk/wp-content/uploads/2015/09/default-avatar.png',
    }

    if (this.props.device.info) {
      badgeStyle.backgroundColor = "#64DD17"
      safetyIcon = <FontIcon color="#F0F0F0" className="material-icons" style={{fontSize: 18}}>security</FontIcon>
    } else {
      badgeStyle.backgroundColor = "#F44336"
      safetyIcon = <FontIcon color="#F0F0F0" className="material-icons" style={{fontSize: 18}}>lock</FontIcon>
    }


    return (
      <Paper className="TapFiUser show" zDepth={3} onClick={() => {this.handleSelection()}}>
        <Badge
          className="avatar"
          badgeContent={safetyIcon}
          secondary={true}
          badgeStyle={badgeStyle}>
          <Avatar
            src={info.avatar}
            size={96}/>
        </Badge>
        <p className="name">{this.props.device.name}</p>
        <p className="acc">{info.acc || '...'}</p>
        <div className={statusClass}>
          {this.props.device.status}
        </div>
      </Paper>
    )
  }

  handleSelection() {
    this.props.onSelect && this.props.onSelect(this.props.device)
  }
}