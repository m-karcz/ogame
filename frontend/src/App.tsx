import React from 'react';
import LoginForm from "./LoginForm"
import LoginProcedure from "./LoginProcedure"
import RegisterProcedure from "./RegisterProcedure"

class App extends React.Component<any, any>{
  render()
  {
  return (
    <div className="App">
      <LoginProcedure store={this.props.store}/>
      <RegisterProcedure store={this.props.store}/>
      <LoginForm store={this.props.store}/>
    </div>
  );
  }
}

export default App;
