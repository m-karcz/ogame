import React from 'react'
import {connect} from 'react-redux'
import {loginRequest, registerRequest} from "./Actions"
import {LoginFormState, LoginState, RegisterState, getLoginFormState, DEFAULT_LOGIN_FIELDS} from "./Store"
import {UserCredentials } from "../../build/common/generated/AllGenerated"

const mapDispatchToProps = (dispatch: any) => ({
    onLoginClick: (fields: UserCredentials) => dispatch(loginRequest(fields)),
    onRegisterClick: (fields: UserCredentials) => dispatch(registerRequest(fields))
})

type LoginFormProps = LoginFormState & ReturnType<typeof mapDispatchToProps>;

class LoginForm extends React.Component<LoginFormProps, never>
{
    constructor(props: LoginFormProps) 
    {
        super(props)
        this.fields = DEFAULT_LOGIN_FIELDS;
    }
    render()
    {
        return <div>
            <input type="text" name="login-input" onChange={this.setLogin.bind(this)}/>
            <input type="password" name="passcode-input" onChange={this.setPasscode.bind(this)}/>
            <input type="submit" value="Login" disabled={this.props.loginState === LoginState.requested} onClick={this.loginClick.bind(this)}/>
            <input type="submit" value="Register" disabled={this.props.registerState === RegisterState.requested} onClick={this.registerClick.bind(this)}/>
        </div>
    }
    private setLogin(event: any)
    {
        this.fields.login = event.target.value;
    }
    private setPasscode(event: any)
    {
        this.fields.passcode = event.target.value;
    }
    private loginClick()
    {
        this.props.onLoginClick(this.fields);
    }
    private registerClick()
    {
        this.props.onRegisterClick(this.fields);
    }
    fields: UserCredentials
}


export default connect(getLoginFormState, mapDispatchToProps)(LoginForm);