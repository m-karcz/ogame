import React from 'react'
import {connect} from 'react-redux'

const mapStateToProps = (state: any) => ({
    "requested" : state.loginRequested
});
const mapDispatchToProps = (dispatch: any) => ({
    onSubmit: (resp: any) => console.log("no elo", resp)
})

const address: string = "/login"

class LoginProcedure extends React.Component<any, any>
{
    render() {
        return null;
    }

    componentDidUpdate() {
        if(this.props.requested != null && this.request == undefined)
        {
            const message = this.props.requested;
            this.request = fetch(address, {"method": "POST", "body": JSON.stringify(message), "credentials": "same-origin"}).then(
                (resp) => this.props.onSubmit(resp)
            );
        }
    }

    request: Promise<Response> | undefined;
}

export default connect(mapStateToProps, mapDispatchToProps)(LoginProcedure);
