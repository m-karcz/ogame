import React from "react"
import {connect} from "react-redux"
import { Building } from "./generated/AllGenerated"
import {startBuilding} from "./Actions"
import { Store, getBuildingQueue } from "./Store"

type PropsFromParent =
{
    buildingName: Building
    level: number
}

const mapDispatchToProps = (dispatch: any) => ({
    onBuildClick: (building: Building) => dispatch(startBuilding({building: building}))
})

const mapStateToProps = (state: Store) => ({
    queue: getBuildingQueue(state)
})



class BuildingEntry extends React.Component<PropsFromParent & ReturnType<typeof mapDispatchToProps> & ReturnType<typeof mapStateToProps>,never>
{
    render() {
        return <tr>
            <th>{this.props.buildingName}</th>
            <th>{this.props.level}</th>
            <th>{this.getBuildButtonField()}</th>
        </tr>
    }

    startBuilding()
    {
        this.props.onBuildClick(this.props.buildingName);
    }
    
    getBuildButtonField()
    {
        return this.props.queue ? this.getBuildTimer() : this.getBuildButton();
    }
    
    getBuildTimer()
    {
        if(this.props.queue?.building! === this.props.buildingName)
        {
            return <div>{this.props.queue?.timeToFinish!}</div>
        }
        else
        {
            return null
        }
    }

    getBuildButton()
    {
        return <a id={"build_" + this.props.buildingName} onClick={this.startBuilding.bind(this)}>Build</a>;
    }
}

export default connect(mapStateToProps, mapDispatchToProps)(BuildingEntry);