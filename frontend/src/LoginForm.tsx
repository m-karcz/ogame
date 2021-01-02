import React from 'react'
import {connect} from 'react-redux'
import {LOGIN_SUBMIT, REGISTER_SUBMIT, LoginSubmitAction, RegisterSubmitAction} from "./Actions"
import {LoginFormState, LoginState, RegisterState, getLoginFormState, LoginFields, DEFAULT_LOGIN_FIELDS} from "./Store"

const mapDispatchToProps = (dispatch: any) => ({
    onLoginClick: (fields: LoginFields) => dispatch({type: LOGIN_SUBMIT, payload: fields} as LoginSubmitAction),
    onRegisterClick: (fields: LoginFields) => dispatch({type: REGISTER_SUBMIT, payload: fields} as RegisterSubmitAction)
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
    fields: LoginFields
}


export default connect(getLoginFormState, mapDispatchToProps)(LoginForm);