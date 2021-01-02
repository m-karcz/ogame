import React from 'react';
import {connect} from 'react-redux'
import {ResourcesViewState, getResourcesViewState, Store} from "./Store"

const mapStateToProps = (state: Store) => ({
    "resources": getResourcesViewState(state)
})

class ResourcesView extends React.Component<ReturnType<typeof mapStateToProps>, any>{
  render() 
  {
    return(
      <>
      <p>metal: {this.props.resources.metal}</p>
      <p>crystal: {this.props.resources.crystal}</p>
      <p>deuter: {this.props.resources.deuter}</p>
      </>)
  }
}

export default connect(mapStateToProps)(ResourcesView);
