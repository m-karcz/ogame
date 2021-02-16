/*import React from "react"
import {connect} from "react-redux"
import { Store, getTotalProduction } from "./Store"
import { TotalProductionData} from './TotalProductionData'
import { MineGeneration} from "./MineGeneration"

type EntryState =
{
    name: keyof TotalProductionData
    data: MineGeneration
}


class ResourcesEntry extends React.Component<EntryState,{}>
{
    render()
    {
        return <tr>
            <th>{this.props.name}</th>
            <th>{this.props.data.generation}</th>
            <th>{this.props.data.energyUsage}</th>
        </tr>
    }
}

const mapStateToProps = (state: Store) => ({
    totalProduction : getTotalProduction(state)
})

class ResourcesPage extends React.Component<ReturnType<typeof mapStateToProps>,never>
{
    render()
    {
        return <table>
            <tr>
                <th>Building</th>
                <th>Generation</th>
                <th>Energy usage</th>
            </tr>
            {
                Object.entries(this.props.totalProduction).filter(x => x[1] !== null)
                                                          .map(x => <ResourcesEntry name={x[0] as keyof TotalProductionData} data={x[1]!}/>)
            }
        </table>
    }
}

export default connect(mapStateToProps)(ResourcesPage);*/

export default null;