/*
Copyright (c) 2015 Christopher Higgins Barrett

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

#include <assert.h>

#include "../include/DataMapReader.hpp"
#include "../include/DataNode.hpp"

#define DATAMAPREADER_BREAK_ON_INVALIDATING_ACTIONS 0
#define DATAMAPREADER_BASIC_SAFETY_CHECKS 1
#define DATAMAPREADER_EXTRA_SAFETY_CHECKS 1

namespace CSaruContainer {

//=========================================================================
DataMapReader::DataMapReader (const DataNode * node)
    : m_node(node)
{}

//=========================================================================
DataMapReader::DataMapReader (const DataMapReader & other)
    : m_node(other.m_node)
{
    m_nodeStack = other.m_nodeStack;
}

//=========================================================================
DataMapReader & DataMapReader::operator= (const DataMapReader & rhs) {
    m_node = rhs.m_node;
    m_nodeStack = rhs.m_nodeStack;
    return *this;
}

//=========================================================================
DataMapReader & DataMapReader::PopNode (void) {
    // if at the root node, invalidate this Reader
    if (m_nodeStack.empty()) {
        #if DATAMAPREADER_BREAK_ON_INVALIDATING_ACTIONS
            assert(false && "DataMapReader::PopNode() called, but this Reader was " "already at the root node.");
        #endif
        m_node = NULL;
    // otherwise, go up one node in the stack
    } else {
        m_node = m_nodeStack.back();
        m_nodeStack.pop_back();
    }
    return *this;
}

//=========================================================================
DataMapReader & DataMapReader::ToFirstChild (void) {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ToFirstChild() called, but m_node == NULL.");
    #endif

    const DataNode * child = m_node->GetChildSafe(0);
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(child && "DataMapReader::ToFirstChild() called, but m_node has no " "children.");
    #endif

    PushNode(child);
    return *this;
}

//=========================================================================
DataMapReader & DataMapReader::ToLastChild (void) {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ToLastChild() called, but m_node == NULL.");
    #endif

    int childCount = m_node->GetChildCount();
    PushNode(m_node->GetChildSafe(childCount - 1));
    return *this;
}

//=========================================================================
DataMapReader & DataMapReader::ToChild (int index) {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ToChild(int index) called, " "but m_node == NULL.");
        assert(index >= 0 && "DataMapReader::ToChild(int index) called with a " "negative index.");
    #endif

    PushNode(m_node->GetChildSafe(index));
    return *this;
}

//=========================================================================
DataMapReader & DataMapReader::ToChild (const char * name) {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ToChild(const char * name) called, " "but m_node == NULL.");
    #endif

    const DataNode * desiredChild = m_node->GetChildByName(name);
    PushNode(desiredChild); 
    return *this;
}

//=========================================================================
DataMapReader & DataMapReader::ToNextSibling (void) {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ToNextSibling() called, but " "m_node == NULL.");
    #endif
    #if DATAMAPREADER_BREAK_ON_INVALIDATING_ACTIONS
        assert(
            !m_nodeStack.empty() &&
                "DataMapReader::ToNextSibling() called, but m_node is the root.  "
                "Root nodes are not allowed to have siblings."
        );
    #endif

    // if on root node, invalidate
    if (m_nodeStack.empty()) {
        m_node = NULL;
        return *this;
    }
    const DataNode * parent = m_nodeStack.back();
    // calculate the index we are currently at
    int prevIndex = 0;
    // NOTE: If the top of the m_nodeStack does not have as one of its children
    //  the current m_node, this will loop infinitely.
    while (parent->GetChildFast(prevIndex) != m_node)
        ++prevIndex;

    m_node = parent->GetChildSafe(prevIndex + 1);
    return *this;
}

//=========================================================================
DataMapReader & DataMapReader::ToPreviousSibling (void) {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ToPreviousSibling() called, but m_node == " "NULL.");
    #endif
    #if DATAMAPREADER_BREAK_ON_INVALIDATING_ACTIONS
        assert(
            !m_nodeStack.empty() &&
                "DataMapReader::ToPreviousSibling() called, but m_node is the root.  "
                "Root nodes are not allowed to have siblings."
        );
    #endif

    // if on root node, invalidate
    if (m_nodeStack.empty()) {
        m_node = NULL;
        return *this;
    }
    const DataNode * parent = m_nodeStack.back();
    // calculate the index we are currently at
    int prevIndex = -1;
    // NOTE: If the top of the m_nodeStack does not have as one of its children
    //  the current m_node, this will loop infinitely.
    while (parent->GetChildFast(prevIndex) != m_node)
        ++prevIndex;

    m_node = parent->GetChildSafe(prevIndex);
    return *this;
}

//=========================================================================
const char * DataMapReader::ReadName (void) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ReadName() called, but m_node == NULL.");
    #endif

    return m_node->GetName();
}

//=========================================================================
bool DataMapReader::ReadBool (void) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ReadBool() called, but m_node == NULL.");
    #endif
    #if DATAMAPREADER_EXTRA_SAFETY_CHECKS
        assert(
            m_node->GetType() == DataNode::Type::Bool &&
                "DataMapReader::ReadBool() called, but m_node's type is not Type::Bool."
        );
    #endif

    return m_node->GetBool();
}

//=========================================================================
int DataMapReader::ReadInt (void) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ReadInt() called, but m_node == NULL.");
    #endif
    #if DATAMAPREADER_EXTRA_SAFETY_CHECKS
        assert(
            m_node->GetType() == DataNode::Type::Int &&
                "DataMapReader::ReadInt() called, but m_node's type is not Type::Int."
        );
    #endif

    return m_node->GetInt();
}

//=========================================================================
float DataMapReader::ReadFloat (void) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ReadFloat() called, but m_node == NULL.");
    #endif
    #if DATAMAPREADER_EXTRA_SAFETY_CHECKS
        assert(
            m_node->GetType() == DataNode::Type::Float &&
                "DataMapReader::ReadFloat() called, but m_node's type is not Type::Float."
        );
    #endif

    return m_node->GetFloat();
}

//=========================================================================
const char * DataMapReader::ReadString (void) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapReader::ReadString() called, but m_node == NULL.");
    #endif
    #if DATAMAPREADER_EXTRA_SAFETY_CHECKS
        assert(
            m_node->GetType() == DataNode::Type::String &&
                "DataMapReader::ReadString() called, but m_node's type is not Type::String."
        );
    #endif

    return m_node->GetString();
}

//=========================================================================
bool DataMapReader::ReadBoolSafe (bool * outBool) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(outBool && "DataMapReader::ReadBoolSafe() called, but outBool " "== NULL.");
        assert(m_node && "DataMapReader::ReadBoolSafe() called, but m_node == NULL.");
    #endif

    return m_node->QueryBool(outBool);
}

//=========================================================================
bool DataMapReader::ReadIntSafe (int * outInt) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(outInt && "DataMapReader::ReadIntSafe() called, but outInt " "== NULL.");
        assert(m_node && "DataMapReader::ReadIntSafe() called, but m_node == NULL.");
    #endif

    return m_node->QueryInt(outInt);
}

//=========================================================================
bool DataMapReader::ReadFloatSafe (float * outFloat) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(outFloat && "DataMapReader::ReadFloatSafe() called, but outFloat " "== NULL.");
        assert(m_node && "DataMapReader::ReadFloatSafe() called, but m_node " "== NULL.");
    #endif

    return m_node->QueryFloat(outFloat);
}

//=========================================================================
bool DataMapReader::ReadStringSafe (char * outString,
int buffer_size_in_elements) const {
    #if DATAMAPREADER_BASIC_SAFETY_CHECKS
        assert(outString && "DataMapReader::ReadStringSafe() called, but " "outString == NULL.");
        assert(m_node && "DataMapReader::ReadStringSafe() called, but m_node " "== NULL.");
    #endif

    return m_node->QueryString(outString, buffer_size_in_elements);
}

//=========================================================================
void DataMapReader::PushNode (const DataNode * node) {
    m_nodeStack.push_back(m_node);
    m_node = node;
}

} // namespace CSaruContainer

#undef DATAMAPREADER_EXTRA_SAFETY_CHECKS
#undef DATAMAPREADER_BASIC_SAFETY_CHECKS
#undef DATAMAPREADER_BREAK_ON_INVALIDATING_ACTIONS
