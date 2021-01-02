from yaml import load
import sys
import re

output_dir = sys.argv[1]

headers_list = []

def getHeader(typename):
    specialHeaders = {
        "string" : "<string>"
    }
    if typename[0].isupper():
        return '"{}.hpp"'.format(typename)
    else:
        return specialHeaders.get(typename)

def mapTypeToCpp(typename):
    mapping = {
        "string": "std::string"
    }
    return mapping.get(typename, typename)

def mapTypeToTs(typename):
    mapping = {
        "int": "number",
        "Timestamp": "number",
        "Duration": "number",
        "BigNum": "number"
    }
    return mapping.get(typename, typename)

def mapNameAndGetHeader(field_type):
    if(field_type == "string"):
        return ["std::string", "<string>"]
    if field_type[0].islower():
        return [field_type, None]
    return [field_type, '"' + field_type + '.hpp"']


def writeFieldToHpp(field_type):
    headers = []
    isOptional = field_type.startswith("optional")
    if isOptional:
        field_type = field_type[9:-1]
    isArray = field_type.endswith("[]")
    if isArray:
        field_type = field_type[:-2]
    isUnion = field_type.startswith("union")

    if isUnion:
        types = []
        for single_type in field_type[6:-1].split(","):
            [name, header] = mapNameAndGetHeader(single_type)

            if header:
                headers.append(header)
            types.append(name)
        headers.append("<variant>")
        field_type = "std::variant<" + ",".join(types) + ">"

    else:
        [name, header] = mapNameAndGetHeader(field_type)

        if header:
            headers.append(header)

        field_type = name
    
    if isArray:
        headers.append("<vector>")
        field_type = "std::vector<" + field_type + ">"

    if isOptional:
        headers.append("<optional>")
        field_type = "std::optional<" + field_type + ">"
    
    return [headers, field_type]
    


def dumpToHpp(name, fields):
    headers_list.append(name + ".hpp")
    with open(output_dir + "/" + name + ".hpp", "w") as fp:
        headers = ['<boost/hana/define_struct.hpp>']
        body = ["struct " + name,
                "{",
                "\tBOOST_HANA_DEFINE_STRUCT(" + name + ","]

        for field_name, field_type in fields.items():
            [new_headers, new_field_type] = writeFieldToHpp(field_type)
            headers.extend(new_headers)
            body.append("\t({},{}),".format(new_field_type, field_name))

        #if len(body) > 3:
        body[-1] = body[-1][0:-1]

        body.append("\t);")
        body.append("};")
        fp.write("#pragma once\n")
        fp.write("\n".join(["#include " + header for header in headers]))
        fp.write("\n\n")
        fp.write("\n".join(body))

def dumpAliasOnlyToHpp(new_name, name):
    headers_list.append(new_name + ".hpp")
    with open(output_dir + "/" + new_name + ".hpp", "w") as fp:
        headers = []
        [new_headers, new_field_type] = writeFieldToHpp(name)
        headers.extend(new_headers)
        body = ["using {} = {};".format(new_name, new_field_type)]

        fp.write("#pragma once\n")
        fp.write("\n".join(["#include " + header for header in headers]))
        fp.write("\n\n")
        fp.write("\n".join(body))


def dumpToTs(name, fields):
    with open(output_dir + "/" + name + ".ts", "w") as fp:
        imports = []
        body = ["export type " + name + " = {"]

        for field_name, field_type in fields.items():
            body.append("\t{} : {}".format(field_name, mapTypeToTs(field_type)))
        body.append("}")

        fp.write("\n".join(imports))
        fp.write("\n\n")
        fp.write("\n".join(body))

def defineTsAlias(original, new_name):
    with open(output_dir + "/" + new_name + ".ts", "w") as fp:
        fp.write("export type {} = {}".format(new_name, original))

with open("types.yaml") as fp:
    y = load(fp)
    defineTsAlias("string", "Status")
    defineTsAlias("number", "BigNum")
    defineTsAlias("number", "Timestaap")
    defineTsAlias("number", "Duration")
    defineTsAlias("string", "Building")
    for name, fields in y.items():
        if isinstance(fields,dict):
            dumpToHpp(name, fields)
            dumpToTs(name, fields)
        else:
            dumpAliasOnlyToHpp(name, fields)


with open(output_dir + "/CMakeLists.txt", "w") as fp:
    cmake = []
    cmakeTarget = "GenHeaders"
    cmake.append("add_library({} {})".format(cmakeTarget, " ".join([output_dir + "/" + header for header in headers_list])))
    cmake.append("add_library({}::{} ALIAS {})".format(cmakeTarget, cmakeTarget, cmakeTarget))
    cmake.append("target_include_directories({} PUBLIC {})".format(cmakeTarget, output_dir))
    cmake.append("set_target_properties({} PROPERTIES LINKER_LANGUAGE CXX)".format(cmakeTarget))
    fp.write("\n".join(cmake))