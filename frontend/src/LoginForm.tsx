import React from 'react'
import {connect} from 'react-redux'

const mapStateToProps = () => ({});
const mapDispatchToProps = (dispatch: any) => ({
    onLoginClick: (login: string, passcode:string ) => dispatch({type: "LOGIN_SUBMIT", payload: {login: login, passcode: passcode}}),
    onRegisterClick: (login: string, passcode:string ) => dispatch({type: "REGISTER_SUBMIT", payload: {login: login, passcode: passcode}})
})


class LoginForm extends React.Component<any, any>
{
    constructor(props: any) 
    {
        super(props)
        this.passcode = "";
        this.login = "";
    }
    render()
    {
        return <div>
            <input type="text" onChange={this.setLogin.bind(this)}/>
            <input type="password" onChange={this.setPasscode.bind(this)}/>
            <input type="submit" value="Login" onClick={this.loginClick.bind(this)}/>
            <input type="submit" value="Register" onClick={this.registerClick.bind(this)}/>
        </div>
    }
    private setLogin(event: any)
    {
        this.login = event.target.value;
    }
    private setPasscode(event: any)
    {
        this.passcode = event.target.value;
    }
    private loginClick()
    {
        this.props.onLoginClick(this.login, this.passcode);
    }
    private registerClick()
    {
        this.props.onRegisterClick(this.login, this.passcode);
    }
    login: string
    passcode: string

}


export default connect(mapStateToProps, mapDispatchToProps)(LoginForm);