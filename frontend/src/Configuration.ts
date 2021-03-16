import rawConfiguration from "./Configuration.json"
import {Configuration} from "./generated/AllGenerated"
const configuration : Configuration = rawConfiguration as Configuration;
export default configuration;