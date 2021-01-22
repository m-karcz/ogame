import React from "react"
import {connect} from "react-redux"
import {Store, OVERVIEW_PAGE, BUILDINGS_PAGE, getIngamePageState} from "./Store"
import BuildingsPage from "./BuildingsPage"

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
          default:
              return <div>none</div>
      }
  }
}

export default connect(mapStateToProps)(IngameSwitch)