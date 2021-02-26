import React from "react"
import {connect} from "react-redux"
import { Store } from "./Store"
import { getBuildings } from "./Store"
import { Buildings } from "./generated/AllGenerated"
import {getKeys} from "./Utility"
import {Building} from "./Building"
import BuildingEntry from "./BuildingEntry"


function mapStateToProps(state: Store) 
{
    return {
        buildings: getBuildings(state)
    }
}

const buildingsOrder : Buildings = {
    metalMine:       1,
    crystalMine:     2,
    deuterExtractor: 3,
    solarGenerator:  4,
    fusionReactor:   5,
    robots:          6,
    nanites:         7,
    shipyard:        8,
    metalStorage:    9,
    crystalStorage:  10,
    deuterStorage:   11,
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
        return <div>
            <table>
            {getKeys(this.props.buildings).sort(buildingsOrderPred).map(this.getBuildingEntry.bind(this))}
            </table>
        </div>
    }

    getBuildingEntry(buildingName: Building)
    {
        return <BuildingEntry buildingName={buildingName} level={this.props.buildings[buildingName]}/>;
    }
}

export default connect(mapStateToProps)(BuildingsPage)

