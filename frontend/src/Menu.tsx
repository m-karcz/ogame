import React from 'react';
import {connect } from "react-redux"
import { loadBuildingsPage, loadDependencyTreePage, loadOverviewPage, loadResourcesPage } from './Actions';
import { PlanetLocation } from "./generated/AllGenerated"
import { getChosenPlanet, Store } from './Store';

const mapDispatchToProps = (dispatch: any) => (
{
  onOverviewClick: (planet: PlanetLocation) => dispatch(loadOverviewPage({planet: planet})),
  onBuildingsClick: (planet: PlanetLocation) => dispatch(loadBuildingsPage({planet: planet})),
  onResourcesClick: (planet: PlanetLocation) => dispatch(loadResourcesPage({planet: planet})),
  onDependencyTreeClick: (planet: PlanetLocation) => dispatch(loadDependencyTreePage({planet: planet}))
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
        <button className="link-button" onClick={this.onOverviewClick} id="goToOverviewButton">Overview</button><br/>
        <button className="link-button" onClick={this.onBuildingsClick} id="goToBuildingsButton">Buildings</button><br/>
        <button className="link-button" onClick={this.onResourcesClick} id="goToResourcesButton">Resources</button><br/>
        <button className="link-button" onClick={this.onDependencyTreeClick} id="goToDependencyTreeButton">Dependency Tree</button><br/>
      </>)
  }
  onOverviewClick = () => this.props.onOverviewClick(this.props.planet);
  onBuildingsClick = () => this.props.onBuildingsClick(this.props.planet);
  onResourcesClick = () => this.props.onResourcesClick(this.props.planet);
  onDependencyTreeClick = () => this.props.onDependencyTreeClick(this.props.planet);
}

export default connect(mapStateToProps, mapDispatchToProps)(Menu)
