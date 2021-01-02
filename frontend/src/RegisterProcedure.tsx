import React from 'react'
import {connect} from 'react-redux'
import {LoginFields, RegisterState, getLoginFields, getRegisterState, Store} from "./Store";

const mapStateToProps = (state: Store) => ({
    "fields" : getLoginFields(state),
    "registerState": getRegisterState(state)
});
const mapDispatchToProps = (dispatch: any) => ({
    onSuccessful: () => dispatch({type: "REGISTER_SUCCESSFUL", payload: null})
})

const address: string = "/register"

class RegisterProcedure extends React.Component<ReturnType<typeof mapStateToProps> & ReturnType<typeof mapDispatchToProps>, any>
{
    render() {
        return null;
    }

    componentDidUpdate() {
        if(this.props.registerState == RegisterState.requested)
        {
            this.request = fetch(address, {"method": "POST", "body": JSON.stringify(this.props.fields), "credentials": "same-origin"}).then(
                () => this.props.onSuccessful()
            );
        }
    }

    request: Promise<Response> | undefined;
}

export default connect(mapStateToProps, mapDispatchToProps)(RegisterProcedure);
