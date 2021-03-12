import React from "react"
import {connect} from "react-redux"
import {getBuildings, getResearchs, Store } from "./Store"
import { RequirementsForBuilding,
         RequirementsForResearch,
         BuildingRequirement,
         ResearchRequirement} from "./generated/AllGenerated"
import knowledge from "./Knowledge"
import {prettyName} from "./Utility"



function mapStateToProps(state: Store)
{
    return {
        buildings : getBuildings(state),
        researchs : getResearchs(state)
    }
}

type DependencyTreePageProps = ReturnType<typeof mapStateToProps>

function renderRequirement(req: RequirementsForBuilding | RequirementsForResearch)
{
    const renderSingleReq = (r: BuildingRequirement | ResearchRequirement) => {
        return <div>{prettyName(r.name)}(Level {r.level})</div>
    }
    return <tr>
        <th>{prettyName(req.name)}</th>
        <th>{req.requirements.buildings.map(renderSingleReq)}
            {req.requirements.researchs.map(renderSingleReq)}</th>
    </tr>
}

class DependencyTreePage extends React.Component<DependencyTreePageProps, never>
{
    render()
    {
        return <table className="entity-list">
            {knowledge.requirements.buildings.map(renderRequirement)}
        </table>
    }
}

export default connect(mapStateToProps)(DependencyTreePage);