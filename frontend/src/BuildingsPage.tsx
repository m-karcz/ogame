import React from "react"
import {connect} from "react-redux"
import { Store } from "./Store"
import { getBuildings } from "./Store"
import {Buildings} from "./Buildings"
import BuildingEntry from "./BuildingEntry"
import {Building} from "./Building"


function mapStateToProps(state: Store) 
{
    return {
        buildings: getBuildings(state)
    }
}

type BuildingsPageProps = ReturnType<typeof mapStateToProps>;

class BuildingsPage extends React.Component<BuildingsPageProps, never>
{
    render(){
        return <div>
            <table>
            {Object.keys(this.props.buildings).map(this.getBuildingEntry.bind(this))}
            </table>
        </div>
    }

    getBuildingEntry(buildingName: Building)
    {
        return <BuildingEntry buildingName={buildingName as keyof Buildings} level={(this.props.buildings as any)[buildingName]}/>;
    }
}

export default connect(mapStateToProps)(BuildingsPage)

