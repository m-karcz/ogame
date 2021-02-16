import React from 'react';
import {connect } from "react-redux"
import { loadBuildingsPage, loadOverviewPage, loadResourcesPage } from './Actions';
import { PlanetLocation } from "../../build/common/generated/AllGenerated"
import { getChosenPlanet, Store } from './Store';

const mapDispatchToProps = (dispatch: any) => (
{
  onOverviewClick: (planet: PlanetLocation) => dispatch(loadOverviewPage({planet: planet})),
  onBuildingsClick: (planet: PlanetLocation) => dispatch(loadBuildingsPage({planet: planet})),
  onResourcesClick: (planet: PlanetLocation) => dispatch(loadResourcesPage({planet: planet}))
})

const mapStateToProps = (state: Store) =>
  ({
    planet: getChosenPlanet(state)
  })

type MenuProps = ReturnType<typeof mapStateToProps> & ReturnType<typeof mapDispatchToProps>;

class Menu extends React.Component<MenuProps, never>{
  render() 
  {
    return(
      <>
        <a onClick={this.onOverviewClick}>Overview</a>
        <a onClick={this.onBuildingsClick}>Buildings</a>
        <a onClick={this.onResourcesClick}>Resources</a>
      </>)
  }

  onOverviewClick = () => this.props.onOverviewClick(this.props.planet);
  onBuildingsClick = () => this.props.onBuildingsClick(this.props.planet);
  onResourcesClick = () => this.props.onResourcesClick(this.props.planet);
}

export default connect(mapStateToProps, mapDispatchToProps)(Menu)
