import React from "react"
import {connect} from "react-redux"
import {Store, OVERVIEW_PAGE, BUILDINGS_PAGE, getIngamePageState, RESOURCES_PAGE, DEPENDENCY_TREE_PAGE} from "./Store"
import BuildingsPage from "./BuildingsPage"
import ResourcesPage from "./ResourcesPage"
import DependencyTreePage from "./DependencyTreePage"

type IngameSwitchPageProps = {
    pageType: string
}

const mapStateToProps = (state: Store) => ({
    pageType: getIngamePageState(state).innerPage.type
})

class IngameSwitch extends React.Component<IngameSwitchPageProps, never>{
  render() 
  {
    console.log(this.props.pageType)
      switch(this.props.pageType)
      {
          case OVERVIEW_PAGE:
              return <div>overview</div>
          case BUILDINGS_PAGE:
              return <BuildingsPage />
          case RESOURCES_PAGE:
              return <ResourcesPage />
          case DEPENDENCY_TREE_PAGE:
              return <DependencyTreePage />
          default:
              return <div>none</div>
      }
  }
}

export default connect(mapStateToProps)(IngameSwitch)