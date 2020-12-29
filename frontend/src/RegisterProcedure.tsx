import React from 'react'
import {connect} from 'react-redux'

const mapStateToProps = (state: any) => ({
    "requested" : state.registerRequested
});
const mapDispatchToProps = (dispatch: any) => ({
    onSuccesful: () => dispatch({type: "REGISTER_SUCCESSFUL", payload: null})
})

const address: string = "/register"

class RegisterProcedure extends React.Component<any, any>
{
    render() {
        return null;
    }

    componentDidUpdate() {
        if(this.props.requested != null && this.request == undefined)
        {
            const message = this.props.requested;
            this.request = fetch(address, {"method": "POST", "body": JSON.stringify(message), "credentials": "same-origin"}).then(
                () => this.props.onSuccessful()
            );
        }
    }

    request: Promise<Response> | undefined;
}

export default connect(mapStateToProps, mapDispatchToProps)(RegisterProcedure);
