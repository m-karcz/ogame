import Login from "./Login"
import Ingame from "./Ingame"
import React from "react"
import {LOGIN_PAGE, INGAME_PAGE, Store} from "./Store"
import { connect } from "react-redux"

type PageProps = {
    actualPage: typeof LOGIN_PAGE | typeof INGAME_PAGE
};

function mapStateToProps(store: Store) : PageProps
{
    return {
        actualPage: store.page.type
    }
}

class Page extends React.Component<PageProps, never>{
  render() 
  {
      switch(this.props.actualPage)
      {
          case LOGIN_PAGE:
              return <Login/>
          case INGAME_PAGE:
              return <Ingame/>
      }
  }
}

export default connect(mapStateToProps)(Page)