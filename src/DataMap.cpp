/*
Copyright (c) 2016 Christopher Higgins Barrett

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgement in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "exported/DataMap.hpp"

#if _MSC_VER > 1000
    #pragma warning(push)
    // disable fprintf unsecure, and other such warnings
    #pragma warning(disable:4996)
#endif

namespace CSaruDataMap {

//=========================================================================
DataMap::DataMap(void) {
    m_rootNode.SetType(DataNode::Type::Null);
    m_rootNode.SetName("UNNAMED");
}

//=========================================================================
/*
DataMap::DataMap(DataNode* root) :
    m_rootNode(root)
{}
//*/

//=========================================================================
void DataMap::Clear(void) {
    m_rootNode.DeleteAllChildren();
}

//=========================================================================
DataMapReader DataMap::GetReader(void) const {
    return DataMapReader(&m_rootNode);
}

//=========================================================================
DataMapMutator DataMap::GetMutator(void) {
    return DataMapMutator(&m_rootNode);
}

//=========================================================================
/*
bool DataMap::ReadFromFile(char const* filename, Formats format) {
    if (filename == NULL) {
        #ifdef _DEBUG
            fprintf(stderr, "DataMap::ReadFromFile() called, but filename == NULL.\n");
        #endif
        return false;
    }

    FILE* file = NULL;

    // open file, and check for success.  This varies by Format
    switch (format) {
        case kJson: file = fopen(filename, "rt"); break;
    }

    if (file == NULL) {
        #ifdef _DEBUG
            fprintf(stderr, "DataMap::ReadFromFile() failed to open desired file.  File was [%s].\n", filename);
        #endif
        return false;
    }

    DataMapMutator mutator(&m_rootNode);
    // read from file
    bool read_result = false;
    switch (format) {
        case kJson: read_result = ReadJsonFromFile(file, &mutator); break;
    }

    fclose(file);
    return read_result;
}
//*/

} // namespace CSaruDataMap

#if _MSC_VER > 100
    #pragma warning(pop)
#endif
