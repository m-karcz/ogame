import React from 'react';
import Menu from "./Menu"
import ResourcesView from "./ResourcesView"
import IngameSwitch from "./IngameSwitch"

class Ingame extends React.Component<any, any>{
  render() 
  {
    return (
        <div className="ingame">
          <div><Menu /></div>
          <div><ResourcesView />
               <IngameSwitch /></div>
        </div>);
  }
}

export default Ingame;
