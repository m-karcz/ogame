import React from "react"
import {ProductionInformation,
        Building, 
        ProductionPercentages} from "./generated/AllGenerated"
import { Store, getTotalProduction, getBuildings } from "./Store"
import {connect} from "react-redux"
import knowledge from "./Knowledge"
import {prettyName} from "./Utility"

type Production =
{
    ideal: number,
    actual: number
}

const EMPTY_PROD : Production = {ideal: -1, actual: -1}

function alwaysIdeal(prod: number) : Production
{
    return {
        ideal: prod,
        actual: prod
    }
}

function makeProd(actual: number, ideal: number)
{
    return {
        ideal: ideal,
        actual: actual
    }
}

type ProductionEntryResources =
{
    metal: Production,
    crystal: Production,
    deuter: Production,
    energy: Production 
}

type ProductionBuilding = Extract<Building, keyof ProductionPercentages>;

type ProductionEntryData = 
{
    name: ProductionBuilding 
    level: number
    percentage: number
} & Partial<ProductionEntryResources>

function mapStateToProps(state: Store) 
{
    return {
        ...getTotalProduction(state),
        buildings: getBuildings(state)
    }
}

class ProductionEntry extends React.Component<ProductionEntryData,{}>
{
    static defaultProps : ProductionEntryResources = {
        metal: EMPTY_PROD,
        crystal: EMPTY_PROD,
        deuter: EMPTY_PROD,
        energy: EMPTY_PROD
    };
    render()
    {
        return (<tr>
            <th>{prettyName(this.props.name)} (Level {this.props.level})</th>
            <th>{this.formatProduction(this.props.metal!)}</th>
            <th>{this.formatProduction(this.props.crystal!)}</th>
            <th>{this.formatProduction(this.props.deuter!)}</th>
            <th>{this.formatProduction(this.props.energy!)}</th>
            <th>{this.props.percentage}%</th>
        </tr>)
    }
    formatProduction(prod: Production)
    {
        //return <>{prod < 0 ? "-" : Math.floor(prod)}</>
        if(prod.ideal == prod.actual)
        {
            if(prod.actual == -1)
            {
                return <>-</>
            }
            return <>{prod.actual}</>
        }
        return <>{prod.actual}<br/>({prod.ideal})</>
    }
}


class ResourcesPage extends React.Component<ReturnType<typeof mapStateToProps>,never>
{
    render()
    {
        const prod = this.props.production;
        return <table>
            <tr>
                <th></th>
                <th>Metal</th>
                <th>Crystal</th>
                <th>Deuter</th>
                <th>Energy</th>
                <th>Percentage</th>
            </tr>
            <tr>
                <th>Base Production</th>
                <th>{knowledge.productions.baseMetal}</th>
                <th>{knowledge.productions.baseCrystal}</th>
                <th>-</th>
                <th>-</th>
                <th></th>
            </tr>
                             
            {this.props.buildings.metalMine > 0 ?
             <ProductionEntry name="metalMine"
                              level={this.props.buildings.metalMine}
                              percentage={this.props.percentages.metalMine}
                              metal={makeProd(prod.metalMineGeneration, prod.metalMineIdealGeneration)}
                              energy={alwaysIdeal(-prod.metalMineEnergyUsage)}></ProductionEntry>
             : null}
            {this.props.buildings.crystalMine > 0 ?
             <ProductionEntry name="crystalMine"
                              level={this.props.buildings.crystalMine}
                              percentage={this.props.percentages.crystalMine}
                              crystal={makeProd(prod.crystalMineGeneration, prod.crystalMineIdealGeneration)}
                              energy={alwaysIdeal(-prod.crystalMineEnergyUsage)}></ProductionEntry>
             : null}
            {this.props.buildings.deuteriumSynthesizer > 0 ?
             <ProductionEntry name="deuteriumSynthesizer"
                              level={this.props.buildings.deuteriumSynthesizer}
                              percentage={this.props.percentages.deuteriumSynthesizer}
                              deuter={makeProd(prod.deuteriumSynthesizerGeneration, prod.deuteriumSynthesizerIdealGeneration)}
                              energy={alwaysIdeal(-prod.deuteriumSynthesizerEnergyUsage)}></ProductionEntry>
             : null}
            {this.props.buildings.solarPlant > 0 ?
             <ProductionEntry name="solarPlant"
                              level={this.props.buildings.solarPlant}
                              percentage={this.props.percentages.solarPlant}
                              energy={alwaysIdeal(prod.solarPlantGeneration)}></ProductionEntry>
             : null}

             <tr>
                 <th>Total</th>
                 <th>{this.props.production.metalMineGeneration + knowledge.productions.baseMetal}</th>
                 <th>{this.props.production.crystalMineGeneration + knowledge.productions.baseCrystal}</th>
                 <th>{this.props.production.deuteriumSynthesizerGeneration - this.props.production.fusionReactorDeuterUsage}</th>
                 <th>{this.props.production.solarPlantGeneration
                      + this.props.production.solarSateliteGeneration
                      + this.props.production.fusionReactorGeneration
                      - this.props.production.metalMineEnergyUsage
                      - this.props.production.crystalMineEnergyUsage
                      - this.props.production.deuteriumSynthesizerEnergyUsage}</th>
                 <th></th>
             </tr>
        </table>
    }
}

export default connect(mapStateToProps)(ResourcesPage);