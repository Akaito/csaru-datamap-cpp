/*
Copyright (c) 2015 Christopher Higgins Barrett

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <DataMap.hpp>

#if _MSC_VER > 1000
    #pragma warning(push)
    // disable fprintf unsecure, and other such warnings
    #pragma warning(disable:4996)
#endif

namespace CSaruContainer {

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

} // namespace CSaruContainer

#if _MSC_VER > 100
    #pragma warning(pop)
#endif
