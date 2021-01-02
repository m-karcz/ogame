import React from 'react';
import Menu from "./Menu"
import ResourcesView from "./ResourcesView"

class Ingame extends React.Component<any, any>{
  render() 
  {
    return(
      <>
        <Menu />
        <ResourcesView />
      </>)
  }
}

export default Ingame;
