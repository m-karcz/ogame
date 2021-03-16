import React from "react"
import {connect} from "react-redux"
import { getResearchs, Store } from "./Store"
import { getBuildings } from "./Store"
import { Buildings,
         Building } from "./generated/AllGenerated"
import {getKeys, isSatisfied, findRequirements} from "./Utility"
import BuildingEntry from "./BuildingEntry"
import knowledge from "./Knowledge"


function mapStateToProps(state: Store) 
{
    return {
        buildings: getBuildings(state),
        researchs: getResearchs(state)
    }
}

const buildingsOrder : Buildings = {
    metalMine:       1,
    crystalMine:     2,
    deuteriumSynthesizer: 3,
    solarPlant:  4,
    fusionReactor:   5,
    roboticsFactory:          6,
    naniteFactory:         7,
    shipyard:        8,
    metalStorage:    9,
    crystalStorage:  10,
    deuteriumTank:   11,
    researchLab:     12,
    terraformer:     13,
    alianceDepot:    14,
    missileSilo:     15
};

function buildingsOrderPred(lhs: Building, rhs: Building) : number
{
    return buildingsOrder[lhs] - buildingsOrder[rhs];
}

type BuildingsPageProps = ReturnType<typeof mapStateToProps>;

class BuildingsPage extends React.Component<BuildingsPageProps, never>
{
    render(){
        console.log("rendering BuildingsPage")
        return <div>
            <table className="entity-list">
            {getKeys(this.props.buildings).sort(buildingsOrderPred)
                                          .filter(this.areRequirementsSatisfied.bind(this))
                                          .map(this.getBuildingEntry.bind(this))}
            </table>
        </div>
    }

    getBuildingEntry(buildingName: Building)
    {
        return <BuildingEntry buildingName={buildingName} level={this.props.buildings[buildingName]}/>;
    }

    areRequirementsSatisfied(buildingName: Building) : Boolean
    {
        return isSatisfied(findRequirements(buildingName, knowledge.requirements.buildings),
                           this.props.buildings,
                           this.props.researchs);
    }
}

export default connect(mapStateToProps)(BuildingsPage)

