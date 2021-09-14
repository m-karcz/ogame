add_library(DeploymentCommon SimpleLoadBalancer.hpp)
add_library(DeploymentCommon::DeploymentCommon ALIAS DeploymentCommon)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET DeploymentCommon PROPERTY CXX_STANDARD 17)

target_include_directories(DeploymentCommon PUBLIC .)
target_link_libraries(DeploymentCommon SerializationInterface::SerializationInterface Messaging::Messaging)

