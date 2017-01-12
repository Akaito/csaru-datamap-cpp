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

#include <cassert>

#include "exported/DataMapMutator.hpp"
#include "exported/DataNode.hpp"

#define DATAMAPMUTATOR_BREAK_ON_INVALIDATING_ACTIONS 1
#define DATAMAPMUTATOR_BASIC_SAFETY_CHECKS 1
#define DATAMAPMUTATOR_EXTRA_SAFETY_CHECKS 1

namespace CSaruDataMap {

//=========================================================================
DataMapMutator::DataMapMutator (DataNode * dataNode)
    : m_node(dataNode)
{}

//=========================================================================
DataMapMutator::DataMapMutator (const DataMapMutator & other)
    : m_node(other.m_node)
{
    m_nodeStack = other.m_nodeStack;
}

//=========================================================================
DataMapMutator & DataMapMutator::operator= (const DataMapMutator & rhs) {
    m_node = rhs.m_node;
    m_nodeStack = rhs.m_nodeStack;
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::PushNode (DataNode * node) {
    m_nodeStack.push_back(m_node);
    m_node = node;
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::PopNode (void) {
    // if at the root node, invalidate this Mutator
    if (m_nodeStack.empty()) {
        #if DATAMAPMUTATOR_BREAK_ON_INVALIDATING_ACTIONS
            assert(false && "DataMapMutator::PopNode() called, but this Mutator was already at the root node.");
        #endif
        m_node = nullptr;
    // otherwise, go up one node in the stack
    } else {
        m_node = m_nodeStack.back();
        m_nodeStack.pop_back();
    }
    return *this;
}

//=========================================================================
DataNode * DataMapMutator::GetParentNode (void) {
    if (m_nodeStack.empty())
        return nullptr;
    return m_nodeStack.back();
}

//=========================================================================
DataMapMutator & DataMapMutator::ToFirstChild (void) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ToFirstChild() called, but m_node == nullptr.");
    #endif

    DataNode * child = m_node->GetChildSafe(0);
    // this is a mutator.  If there are no children, create one
    if (child == nullptr)
        child = m_node->AppendNewChild();

    PushNode(child);
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::ToLastChild (void) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ToLastChild() called, but m_node == nullptr.");
    #endif

    // this is a mutator.  If there are no children, create one
    if (m_node->GetChildCount() == 0)
        PushNode(m_node->AppendNewChild());
    else
        PushNode(m_node->GetChildFast(m_node->GetChildCount() - 1));

    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::ToChild (int index) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ToChild(int index) called, but m_node == nullptr.");
        assert(index >= 0 && "DataMapMutator::ToChild(int index) called with a negative index.");
    #endif

    // this is a mutator.  If there are not enough children, create them
    while (m_node->GetChildCount() <= index)
        m_node->AppendNewChild();

    PushNode(m_node->GetChildFast(index));
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::ToChild (const char * name) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ToChild(const char * name) called, but m_node == nullptr.");
    #endif

    DataNode * desiredChild = m_node->GetChildByName(name);

    // this is a mutator.  If there is no such child, create one
    if (desiredChild == nullptr)
        PushNode(m_node->AppendNewChild()->SetName(name));
    else
        PushNode(desiredChild); 
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::ToNextSibling (void) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ToNextSibling() called, but m_node == nullptr.");
    #endif
    #if DATAMAPMUTATOR_BREAK_ON_INVALIDATING_ACTIONS
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::ToNextSibling() called, but m_node is the root.  "
                "Root nodes are not allowed to have siblings."
        );
    #endif

    DataNode * parent = m_nodeStack.back();
    // calculate the index we are currently at
    int prevIndex = 0;
    // NOTE: If the top of the m_nodeStack does not have as one of its children
    //  the current m_node, this will loop infinitely.
    while (parent->GetChildFast(prevIndex) != m_node)
        ++prevIndex;

    DataNode * sibling = parent->GetChildSafe(prevIndex + 1);

    // this is a mutator.  If there is no next sibling, create one
    if (sibling == nullptr)
        m_node = parent->AppendNewChild();
    else
        m_node = sibling;

    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::ToPreviousSibling (void) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ToPreviousSibling() called, but m_node == nullptr.");
    #endif
    #if DATAMAPMUTATOR_BREAK_ON_INVALIDATING_ACTIONS
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::ToPreviousSibling() called, but m_node is the root.  "
                "Root nodes are not allowed to have siblings."
        );
    #endif

    DataNode * parent = m_nodeStack.back();
    // calculate the index we are currently at
    int prevIndex = -1;
    // NOTE: If the top of the m_nodeStack does not have as one of its children
    //  the current m_node, this will loop infinitely.
    while (parent->GetChildFast(prevIndex) != m_node)
        ++prevIndex;

    DataNode * sibling = parent->GetChildSafe(prevIndex);

    // this is a mutator.  If there is no next sibling, create one
    if (sibling == nullptr)
        m_node = parent->InsertNewChild(0);
    else
        m_node = sibling;

    return *this;
}

//=========================================================================
bool DataMapMutator::IsFirstChild (void) {
    // if no parent, is first child
    if (m_nodeStack.size() <= 1)
        return true;
    return (m_nodeStack.back()->GetChildFast(0) == m_node);
}

//=========================================================================
DataMapMutator & DataMapMutator::SetToObjectType (void) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::SetToObjectType() called, but m_node == nullptr.");
    #endif

    m_node->SetType(DataNode::Type::Object);
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::SetToArrayType (void) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::SetToArrayType() called, but m_node == nullptr.");
    #endif

    m_node->SetType(DataNode::Type::Array);
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::SetToBooleanType (void) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::SetToBooleanType() called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::SetToBooleanType() called, but m_node is the root of a DataMap.  "
                "Roots must be of the Object or Array type."
        );
    #endif

    // SetBool() implies SetType()
    m_node->SetType(DataNode::Type::Bool);
    // not using SetBool(), because this is just a type-changing function.
    //  For performance reasons, we'll assume the user will still set the data.
    //m_node->SetBool(false);
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::SetToNullType (void) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::SetToNullType() called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::SetToNullType() called, but m_node is the root of a DataMap.  "
                "Roots must be of the Object or Array type."
        );
    #endif

    m_node->SetType(DataNode::Type::Null);
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::CreateChild (char const * name) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::CreateChild() called, but m_node == nullptr.");
    #endif

    DataNode * child = m_node->AppendNewChild();
    if (name != nullptr)
        child->SetName(name);

    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::CreateChildSafe (char const * name, std::size_t nameLen) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::CreateChildSafe() called, but m_node == nullptr.");
        assert(name && "DataMapMutator::CreateChildSafe() called, but name == nullptr.");
    #endif

    DataNode * child = m_node->AppendNewChild();
    child->SetNameSecure(name, int(nameLen));
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::CreateAndGotoChild (char const * name) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::CreateAndGotoChild() called, but m_node == nullptr.");
    #endif

    DataNode * child = m_node->AppendNewChild();
    if (name != nullptr)
        child->SetName(name);

    PushNode(child);
    return *this;
}

//=========================================================================
DataMapMutator & DataMapMutator::CreateAndGotoChildSafe (char const * name, std::size_t nameLen) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::CreateAndGotoChildSafe() called, but m_node == nullptr.");
        assert(name && "DataMapMutator::CreateAndGotoChildSafe() called, but name == nullptr.");
    #endif

    DataNode * child = m_node->AppendNewChild();
    child->SetNameSecure(name, int(nameLen));
    PushNode(child);
    return *this;
}

//=========================================================================
void DataMapMutator::WriteName (char const * name) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::WriteName() called, but m_node == nullptr.");
        assert(name && "DataMapMutator::WriteName() called, but name == nullptr.");
    #endif

    m_node->SetName(name);
}

//=========================================================================
void DataMapMutator::WriteNameSecure (char const * name, int sizeInElements) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::WriteNameSecure() called, but m_node == nullptr.");
    #endif

    m_node->SetNameSecure(name, sizeInElements);
}

//=========================================================================
void DataMapMutator::Write (bool boolValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::Write(bool) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::Write(bool) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetBool(boolValue);
}

//=========================================================================
void DataMapMutator::Write (char const * name, bool boolValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::Write(bool) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::Write(char const *, bool) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetName(name);
    m_node->SetBool(boolValue);
}

//=========================================================================
void DataMapMutator::Write (int intValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::Write(int) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::Write(int) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetInt(intValue);
}

//=========================================================================
void DataMapMutator::Write (char const * name, int intValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::Write(name, int) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::Write(char const *, int) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetName(name);
    m_node->SetInt(intValue);
}

//=========================================================================
void DataMapMutator::Write (float floatValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::Write(float) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::Write(float) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetFloat(floatValue);
}

//=========================================================================
void DataMapMutator::Write (char const * name, float floatValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::Write(name, float) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::Write(char const *, float) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetName(name);
    m_node->SetFloat(floatValue);
}

//=========================================================================
void DataMapMutator::Write (char const * stringValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::Write(char const *) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::Write(char const *) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetString(stringValue);
}

//=========================================================================
void DataMapMutator::Write (char const * name, char const * stringValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::Write(char const *, char const *) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::Write(char const *, char const *) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetName(name);
    m_node->SetString(stringValue);
}

//=========================================================================
void DataMapMutator::WriteSafe (char const * name, int nameSizeInElements, bool boolValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::WriteSafe(char const *, int, int) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::WriteSafe(char const *, int, bool) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetNameSecure(name, nameSizeInElements);
    m_node->SetBool(boolValue);
}

//=========================================================================
void DataMapMutator::WriteSafe (char const * name, int nameSizeInElements, int intValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::WriteSafe(char const *, int, int) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::WriteSafe(char const *, int, int) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetNameSecure(name, nameSizeInElements);
    m_node->SetInt(intValue);
}

//=========================================================================
void DataMapMutator::WriteSafe (char const * name, int nameSizeInElements, float floatValue) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::WriteSafe(char const *, int, float) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::WriteSafe(char const *, int, float) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetNameSecure(name, nameSizeInElements);
    m_node->SetFloat(floatValue);
}

//=========================================================================
void DataMapMutator::WriteSafe (char const * stringValue, int valueSizeInElements) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::WriteSafe(char const *, int) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::WriteSafe(char const *, int) called, but m_node is currently the root.  "
                "The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetStringSecure(stringValue, valueSizeInElements);
}

//=========================================================================
void DataMapMutator::WriteSafe (
    char const * name,
    int          nameSizeInElements,
    char const * stringValue,
    int          valueSizeInElements
) {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::WriteSafe(char const *, int, char const *, int) called, but m_node == nullptr.");
        assert(
            !m_nodeStack.empty() &&
                "DataMapMutator::WriteSafe(char const *, int, char const *, int) called, but m_node is currently the "
                "root.  The root node of a DataMap must be of either the Object or Array type."
        );
    #endif

    m_node->SetNameSecure(name, nameSizeInElements);
    m_node->SetStringSecure(stringValue, valueSizeInElements);
}

//=========================================================================
void DataMapMutator::Walk (int count) {
    for (int i = 0;  i < count;  ++i)
        ToNextSibling();
}

//=========================================================================
void DataMapMutator::WriteWalkSafeBooleanValue (char const * name, int nameSizeInElements, bool value) {
    WriteNameSecure(name, nameSizeInElements);
    // SetBool() implies SetToBooleanType()
    //SetToBooleanType();
    m_node->SetBool(value);
    ToNextSibling();
}

//=========================================================================
void DataMapMutator::WriteWalkSafeIntegerValue (char const * name, int nameSizeInElements, int value) {
    WriteNameSecure(name, nameSizeInElements);
    m_node->SetInt(value);
    ToNextSibling();
}

//=========================================================================
void DataMapMutator::DeleteLastChildren (int count) {
    for (int i = 0;  i < count;  ++i)
        m_node->DeleteLastChild();
}

//=========================================================================
const char * DataMapMutator::ReadName (void) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ReadName() called, but m_node == nullptr.");
    #endif

    return m_node->GetName();
}

//=========================================================================
bool DataMapMutator::ReadBool (void) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ReadBool() called, but m_node == nullptr.");
    #endif
    #if DATAMAPMUTATOR_EXTRA_SAFETY_CHECKS
        assert(
            m_node->GetType() == DataNode::Type::Bool &&
                "DataMapMutator::ReadBool() called, but m_node's type is not Type::Bool."
        );
    #endif

    return m_node->GetBool();
}

//=========================================================================
int DataMapMutator::ReadInt (void) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ReadInt() called, but m_node == nullptr.");
    #endif
    #if DATAMAPMUTATOR_EXTRA_SAFETY_CHECKS
        assert(
            m_node->GetType() == DataNode::Type::Int &&
                "DataMapMutator::ReadInt() called, but m_node's type is not Type::Int."
        );
    #endif

    return m_node->GetInt();
}

//=========================================================================
float DataMapMutator::ReadFloat (void) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ReadFloat() called, but m_node == nullptr.");
    #endif
    #if DATAMAPMUTATOR_EXTRA_SAFETY_CHECKS
        assert(
            m_node->GetType() == DataNode::Type::Float &&
                "DataMapMutator::ReadFloat() called, but m_node's type is not Type::Float."
        );
    #endif

    return m_node->GetFloat();
}

//=========================================================================
const char * DataMapMutator::ReadString (void) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(m_node && "DataMapMutator::ReadString() called, but m_node == nullptr.");
    #endif
    #if DATAMAPMUTATOR_EXTRA_SAFETY_CHECKS
        assert(
            m_node->GetType() == DataNode::Type::String &&
                "DataMapMutator::ReadString() called, but m_node's type is not Type::String."
        );
    #endif

    return m_node->GetString();
}

//=========================================================================
bool DataMapMutator::ReadBoolSafe (bool * outBool) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(outBool && "DataMapMutator::ReadBoolSafe() called, but outBool == nullptr.");
        assert(m_node && "DataMapMutator::ReadBoolSafe() called, but m_node == nullptr.");
    #endif

    return m_node->QueryBool(outBool);
}

//=========================================================================
bool DataMapMutator::ReadIntSafe (int * outInt) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(outInt && "DataMapMutator::ReadIntSafe() called, but outInt == nullptr.");
        assert(m_node && "DataMapMutator::ReadIntSafe() called, but m_node == nullptr.");
    #endif

    return m_node->QueryInt(outInt);
}

//=========================================================================
bool DataMapMutator::ReadFloatSafe (float * outFloat) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(outFloat && "DataMapMutator::ReadFloatSafe() called, but outFloat == nullptr.");
        assert(m_node && "DataMapMutator::ReadFloatSafe() called, but m_node == nullptr.");
    #endif

    return m_node->QueryFloat(outFloat);
}

//=========================================================================
bool DataMapMutator::ReadStringSafe (char * outString, int buffer_sizeInElements) const {
    #if DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
        assert(outString && "DataMapMutator::ReadStringSafe() called, but outString == nullptr.");
        assert(m_node && "DataMapMutator::ReadStringSafe() called, but m_node == nullptr.");
    #endif

    return m_node->QueryString(outString, buffer_sizeInElements);
}

} // namespace CSaruDataMap

#undef DATAMAPMUTATOR_EXTRA_SAFETY_CHECKS
#undef DATAMAPMUTATOR_BASIC_SAFETY_CHECKS
#undef DATAMAPMUTATOR_BREAK_ON_INVALIDATING_ACTIONS
