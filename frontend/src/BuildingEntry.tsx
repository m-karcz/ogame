import React from "react"
import {connect} from "react-redux"
import { Knowledge,
         Building } from "./generated/AllGenerated"
import {startBuilding} from "./Actions"
import { Store, getBuildingQueue } from "./Store"
import rawKnowledge from "./Knowledge.json"
const knowledge : Knowledge = rawKnowledge as Knowledge;

type PropsFromParent =
{
    buildingName: Building
    level: number
}

type CalculatedCost =
{
    metal: number
    crystal: number
    deuter: number
    energy: number | null
}

const mapDispatchToProps = (dispatch: any) => ({
    onBuildClick: (building: Building) => dispatch(startBuilding({building: building}))
})

const mapStateToProps = (state: Store) => ({
    queue: getBuildingQueue(state)
})

function calculateCost(name: Building, level: number) : CalculatedCost
{
    function calc(init: number, multiplier: number, level: number) : number
    {
        return Math.floor(init * Math.pow(multiplier, level));
    }
    const know = knowledge.buildingCosts.find((elem)=>elem.name == name)!.cost;
    return {
        metal: calc(know.metal, know.multiplier, level),
        crystal: calc(know.crystal, know.multiplier, level),
        deuter: calc(know.crystal, know.multiplier, level),
        energy: know.energy > 0.1 ? calc(know.energy, know.multiplier, level) : null
    }
}


class BuildingEntry extends React.Component<PropsFromParent & ReturnType<typeof mapDispatchToProps> & ReturnType<typeof mapStateToProps>,never>
{
    render() {
        return <tr>
            <th align="left">{this.buildingInfo()}</th>
            <th>{this.getBuildButtonField()}</th>
        </tr>
    }

    buildingInfo() {
        const cost = calculateCost(this.props.buildingName, this.props.level);
        return <>
            <span>{this.prettyName()}</span> (Level <span>{this.props.level}</span>)<br/>
            Cost: <br/>
            Metal: <span>{cost.metal}</span> Crystal: <span>{cost.crystal}</span> Deuter: <span>{cost.deuter}</span> {cost.energy ? <>Energy: <span>{cost.energy}</span></> : <></>}
        </>
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
        return <button className="link-button" id={"build_" + this.props.buildingName} onClick={this.startBuilding.bind(this)}>Build to level {this.level() + 1}</button>;
    }

    level()
    {
        return this.props.level
    }

    name()
    {
        return this.props.buildingName;
    }

    prettyName()
    {
        const camelCaseName = this.props.buildingName[0].toUpperCase() + this.props.buildingName.substring(1);
        return camelCaseName.match(/[A-Z][^A-Z]*/g)?.join(" ");
    }
}

export default connect(mapStateToProps, mapDispatchToProps)(BuildingEntry);