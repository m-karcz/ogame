const { removeModuleScopePlugin, override, babelInclude, addWebpackAlias } = require('customize-cra')
const path = require("path")

module.exports = override(removeModuleScopePlugin(),
                          babelInclude([path.resolve("src"),
                                        path.resolve("../router/src"),
                                        path.resolve("../build/common/generated")]),
                          addWebpackAlias({
                              "@router": path.resolve("../router/src")
                          }));
