import React from 'react'
import {connect} from 'react-redux'
import { Redirect } from 'react-router-dom';
import { SuccessfulLoginPayload } from './Actions';
import {getLoginFields, getLoginState, LoginState, Store} from "./Store"

const mapStateToProps = (state: Store) => ({
    "fields": getLoginFields(state),
    "loginState": getLoginState(state)
})

const mapDispatchToProps = (dispatch: any) => ({
    onSubmit: (payload: SuccessfulLoginPayload) => dispatch({"type": "LOGIN_SUCCESSFUL", payload: payload})
})

const address: string = "/login"

class LoginProcedure extends React.Component<ReturnType<typeof mapStateToProps> & ReturnType<typeof mapDispatchToProps>, any>
{
    render() {
        return this.props.loginState === LoginState.successful ? <Redirect to="/ingame"/> : null;
    }

    componentDidUpdate() {
        if(this.props.loginState === LoginState.requested)
        {
            this.request = fetch(address, {"method": "POST", "body": JSON.stringify(this.props.fields), "credentials": "same-origin"}).then(
                (resp) => resp.json().then((j) => {console.log(j); this.props.onSubmit({
                    planetList : j.planets,
                    resources : j.firstPlanetStorage[0].data.storage
                })})
            );
        }
    }

    request: Promise<Response> | undefined | any;
}

export default connect(mapStateToProps, mapDispatchToProps)(LoginProcedure);
