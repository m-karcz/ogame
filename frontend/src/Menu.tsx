import React from 'react';
import {connect } from "react-redux"
import { loadBuildingsPage, loadOverviewPage, loadResourcesPage } from './Actions';
import { PlanetLocation } from "./generated/AllGenerated"
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
        <button className="link-button" onClick={this.onOverviewClick}>Overview</button>
        <button className="link-button" onClick={this.onBuildingsClick}>Buildings</button>
        <button className="link-button" onClick={this.onResourcesClick}>Resources</button>
      </>)
  }
  onOverviewClick = () => this.props.onOverviewClick(this.props.planet);
  onBuildingsClick = () => this.props.onBuildingsClick(this.props.planet);
  onResourcesClick = () => this.props.onResourcesClick(this.props.planet);
}

export default connect(mapStateToProps, mapDispatchToProps)(Menu)
