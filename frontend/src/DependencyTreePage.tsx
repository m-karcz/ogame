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

    return <><div className={"entity-entry"}>
        {prettyName(req.name)}
    </div>
    <div className={"entity-entry"}>
        {req.requirements.buildings.map(renderSingleReq)}
        {req.requirements.researchs.map(renderSingleReq)}
    </div></>
}

class DependencyTreePage extends React.Component<DependencyTreePageProps, never>
{
    render()
    {
        /*return <table className="entity-list">
            {knowledge.requirements.buildings.map(renderRequirement)}
        </table>*/

        return <div className="requirements-list">
            {knowledge.requirements.buildings.map(renderRequirement)}
        </div>
    }
}

export default connect(mapStateToProps)(DependencyTreePage);