import React from "react"
import {connect} from "react-redux"
import {Store, getIngamePageState, IngamePageType} from "./Store"
import BuildingsPage from "./BuildingsPage"
import ResourcesPage from "./ResourcesPage"
import DependencyTreePage from "./DependencyTreePage"

type IngameSwitchPageProps = {
    pageType: string
}

const mapStateToProps = (state: Store) => ({
    pageType: getIngamePageState(state).innerPage
})

class IngameSwitch extends React.Component<IngameSwitchPageProps, never>{
  render() 
  {
    console.log(this.props.pageType)
      switch(this.props.pageType)
      {
          case IngamePageType.Overview:
              return <div>overview</div>
          case IngamePageType.Buildings:
              return <BuildingsPage />
          case IngamePageType.Resources:
              return <ResourcesPage />
          case IngamePageType.DependencyTree:
              return <DependencyTreePage />
          default:
              return <div>none</div>
      }
  }
}

export default connect(mapStateToProps)(IngameSwitch)