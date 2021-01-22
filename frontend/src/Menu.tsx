import React from 'react';
import {connect } from "react-redux"
import { loadBuildingsPage, loadOverviewPage } from './Actions';
import { PlanetLocation } from './PlanetLocation';
import { getChosenPlanet, Store } from './Store';

const mapDispatchToProps = (dispatch: any) => (
{
  onOverviewClick: (planet: PlanetLocation) => dispatch(loadOverviewPage({planet: planet})),
  onBuildingsClick: (planet: PlanetLocation) => dispatch(loadBuildingsPage({planet: planet}))
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
        <a href="#" onClick={this.onOverviewClick}>Overview</a>
        <a href="#" onClick={this.onBuildingsClick}>Buildings</a>
      </>)
  }

  onOverviewClick = () => this.props.onOverviewClick(this.props.planet);
  onBuildingsClick = () => this.props.onBuildingsClick(this.props.planet);
}

export default connect(mapStateToProps, mapDispatchToProps)(Menu)
