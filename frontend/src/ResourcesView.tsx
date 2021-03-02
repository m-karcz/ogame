import React from 'react';
import {connect} from 'react-redux'
import {getActualPlanetStorage, Store} from "./Store"

const mapStateToProps = (state: Store) => ({
    "resources": getActualPlanetStorage(state)
})

class ResourcesView extends React.Component<ReturnType<typeof mapStateToProps>, any>{
  render() 
  {
    return(
      <>
      <p>metal: <span id="resources_amount_metal">{this.props.resources.metal}</span></p>
      <p>crystal: <span id="resources_amount_crystal">{this.props.resources.crystal}</span></p>
      <p>deuter: <span id="resources_amount_deuter">{this.props.resources.deuter}</span></p>
      </>)
  }
}

export default connect(mapStateToProps)(ResourcesView);
