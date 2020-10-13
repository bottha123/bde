// balxml_configschema.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_configschema_cpp,"$Id$ $CSID$")

#include <balxml_configschema.h>

namespace BloombergLP {
namespace balxml {

// CONSTANTS

const char ConfigSchema::TEXT[] =
"<?xml version='1.0' encoding='UTF-8'?>\n"
"<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
"           xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
"           bdem:package='balxml'\n"
"           elementFormDefault='qualified'>\n"
"\n"
"  <!--\n"
"  This file outlines the schema for encoding and decoding options used to\n"
"  generate and parse XML documents.  This component can be used to validate\n"
"  xml that can be parsed into the corresponding bde value objects, but it does\n"
"  not fully capture the behavior of those components and cannot be used to\n"
"  regenerate them (with bas_codegen.pl) correctly.\n"
"  -->\n"
"\n"
"  <xs:annotation>\n"
"    <xs:documentation>\n"
"      Structures that control the XML encoding and decoding process\n"
"    </xs:documentation>\n"
"  </xs:annotation>\n"
"\n"
"  <xs:simpleType name='EncodingStyle' bdem:preserveEnumOrder='1'>\n"
"    <xs:annotation>\n"
"      <xs:documentation>\n"
"        Enumeration of encoding style (COMPACT or PRETTY).\n"
"\n"
"        The generated C++ code for this schema element is created by using\n"
"        bas_codegen.pl, run by balxml/code_from_xsd.pl. The resulting C++ code\n"
"        must be hand edited to reflect the fact that\n"
"        'COMPACT == BAEXML_COMPACT' and 'PRETTY == BAEXML_PRETTY'.\n"
"      </xs:documentation>\n"
"    </xs:annotation>\n"
"    <xs:restriction base='xs:string'>\n"
"      <xs:enumeration value='COMPACT'/>\n"
"      <xs:enumeration value='PRETTY'/>\n"
"      <xs:enumeration value='BAEXML_COMPACT'/>\n"
"      <xs:enumeration value='BAEXML_PRETTY'/>\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"\n"
"  <xs:complexType name='EncoderOptions'>\n"
"    <xs:annotation>\n"
"      <xs:documentation>\n"
"        Options for performing XML encodings.\n"
"        Encoding style is either COMPACT or PRETTY.  If encoding style is\n"
"        COMPACT, no whitespace will be added between elements.  If encoding\n"
"        style is 'PRETTY', then the 'InitialIndentLevel', 'SpacesPerLevel',\n"
"        and 'WrapColumn' parameters are used to specify the formatting of\n"
"        the output.  Note that 'InitialIndentLevel', 'SpacesPerLevel', and\n"
"        'WrapColumn' are ignored when 'EncodingStyle' is COMPACT (this is\n"
"        the default).\n"
"\n"
"        The generated C++ code for this schema element is created by using\n"
"        bas_codegen.pl, run by balxml/code_from_xsd.pl. The resulting C++ code\n"
"        must be hand edited to make 'DatetimeFractionalSecondPrecision' default\n"
"        to 3 rather than 6.\n"
"      </xs:documentation>\n"
"    </xs:annotation>\n"
"    <xs:sequence>\n"
"      <xs:element name='ObjectNamespace' type='xs:string'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:isNullable='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            namespace where object is defined\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='SchemaLocation' type='xs:string'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:isNullable='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>location of the schema</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='Tag' type='xs:string'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:isNullable='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>tag for top level</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='FormattingMode' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='0'\n"
"            bdem:cppdefault='bdlat_FormattingMode::e_DEFAULT'\n"
"            bdem:cpptype='int'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>Formatting mode</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='InitialIndentLevel' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>initial indentation level</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='SpacesPerLevel' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='4'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            spaces per level of indentation\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='WrapColumn' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='80'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            number of characters to wrap text\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='MaxDecimalTotalDigits' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            Maximum total digits of the decimal value that should be\n"
"            displayed\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='MaxDecimalFractionDigits' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            Maximum fractional digits of the decimal value that should be\n"
"            displayed\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='SignificantDoubleDigits' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            The number of significant digits that must be displayed for the\n"
"            double value.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='EncodingStyle'\n"
"                  type='EncodingStyle'\n"
"                  minOccurs='0'\n"
"                  default='COMPACT'\n"
"                  bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            encoding style (see component-level doc)\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='AllowControlCharacters' type='xs:boolean'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='false'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            Allow control characters to be encoded.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='OutputXMLHeader' type='xs:boolean'\n"
"                  minOccurs='0' maxOccurs='1'\n"
"                  default='true'\n"
"                  bdem:allocatesMemory='0'\n"
"                  bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            This option controls if the baexml encoder should output the XML\n"
"            header.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='OutputXSIAlias' type='xs:boolean'\n"
"                  minOccurs='0' maxOccurs='1'\n"
"                  default='true'\n"
"                  bdem:allocatesMemory='0'\n"
"                  bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            This option controls if the baexml encoder should output the XSI\n"
"            alias with the top-level element.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='DatetimeFractionalSecondPrecision' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='3'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            This option controls the number of decimal places used for seconds\n"
"            when encoding 'Datetime' and 'DatetimeTz'.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='UseZAbbreviationForUtc' type='xs:boolean'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='false'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            This option control whether 'Z' should be used for the zone\n"
"            designator of 'DateTz' or instead of '+00:00' (specific to UTC).\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"    </xs:sequence>\n"
"  </xs:complexType>\n"
"\n"
"  <xs:complexType name='DecoderOptions'>\n"
"    <xs:annotation>\n"
"      <xs:documentation>\n"
"        Options for controlling the XML decoding process.\n"
"\n"
"        The generated C++ code for this schema element is created by using\n"
"        bas_codegen.pl, run by balxml/code_from_xsd.pl with no hand-editing.\n"
"      </xs:documentation>\n"
"\n"
"    </xs:annotation>\n"
"    <xs:sequence>\n"
"      <xs:element name='MaxDepth' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='32'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>maximum recursion depth</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='FormattingMode' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='0'\n"
"            bdem:cppdefault='bdlat_FormattingMode::e_DEFAULT'\n"
"            bdem:cpptype='int'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>Formatting mode</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='SkipUnknownElements' type='xs:boolean'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='true'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            Option to skip unknown elements\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"    </xs:sequence>\n"
"  </xs:complexType>\n"
"\n"
"</xs:schema>\n"
;

}  // close namespace balxml
}  // close enterprise namespace

// BAS_CODEGEN RUN BY code_from_xsd.pl RUN ON Mon Oct 12 11:49:24 EDT 2020
// GENERATED BY BLP_BAS_CODEGEN_2020.10.05

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
