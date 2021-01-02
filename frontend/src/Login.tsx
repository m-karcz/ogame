import React from 'react';
import LoginForm from "./LoginForm"
import LoginProcedure from "./LoginProcedure"
import RegisterProcedure from "./RegisterProcedure"

class Login extends React.Component<any, any>{
  render() 
  {
    return(
      <>
        <LoginForm />
        <LoginProcedure />
        <RegisterProcedure />
      </>)
  }
}

export default Login;
