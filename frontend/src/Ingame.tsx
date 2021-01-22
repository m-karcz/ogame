import React from 'react';
import Menu from "./Menu"
import ResourcesView from "./ResourcesView"
import IngameSwitch from "./IngameSwitch"

class Ingame extends React.Component<any, any>{
  render() 
  {
    return(
      <>
        <Menu />
        <ResourcesView />
        <IngameSwitch />
      </>)
  }
}

export default Ingame;
