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
#include <cstring>

#include "exported/DataNode.hpp"

#if _MSC_VER > 1000
#   pragma warning(push)
    // such-and-such is unsafe.  I'm intentionally not using the 'secure' functions
    //  provided by Microsoft to avoid platform dependency.  The original, standard
    //  functions are used instead.  Where "Safe" class methods are used I'm
    //  manually doing what the 'secure' functions would be doing.
#   pragma warning(disable:4996)
#endif

namespace CSaruDataMap {

//=========================================================================
DataNode::DataNode (const DataNode & other) {
    /*
    memcpy(m_name, other.m_name, s_nameSize);
    // NOTE: This assumes that the m_stringm_data is of 1-byte chars, and is the
    //  largest data member
    memcpy(m_stringm_data, other.m_stringm_data, s_nameSize);
    //*/

    //memcpy(this, &other, sizeof(DataNode));

    SetName(other.m_name);
    SetType(other.m_type);
    // NOTE: This assumes that the m_stringm_data is the largest data member
	memcpy(m_data.m_string, other.m_data.m_string, sizeof(m_data.m_string));
    m_children = other.m_children;
}

//=========================================================================
DataNode & DataNode::operator= (const DataNode & rhs) {
    /*
    memcpy(m_name, rhs.m_name, s_nameSize);
    m_type = rhs.m_type;
    // NOTE: This assumes that the m_stringm_data is of 1-byte chars
    memcpy(m_stringm_data, other.m_stringm_data, s_nameSize);
    //*/

    //memcpy(this, &rhs, sizeof(DataNode));

    SetName(rhs.m_name);
    SetType(rhs.m_type);
    // NOTE: This assumes that the m_stringm_data is the largest data member
    memcpy(m_data.m_string, rhs.m_data.m_string, sizeof(m_data.m_string));
    m_children = rhs.m_children;

    return *this;
}

//=========================================================================
DataNode::DataNode (const char * name, Type type) {
    SetName(name);
    SetType(type);
}

//=========================================================================
DataNode::DataNode (const char * name, int m_intdata) {
    SetName(name);
    SetInt(m_intdata);
}

//=========================================================================
DataNode::DataNode (const char * name, float m_floatdata) {
    SetName(name);
    SetFloat(m_floatdata);
}

//=========================================================================
DataNode::DataNode (const char * name, const char * m_stringdata) {
    SetName(name);
    SetString(m_stringdata);
}

//=========================================================================
DataNode::DataNode (const char * name, bool m_booldata) {
    SetName(name);
    SetBool(m_booldata);
}

//=========================================================================
void DataNode::Initialize (void) {
    strcpy(m_name, "_INIT_m_name");
    m_type = Type::String;
    strcpy(m_data.m_string, "_INIT_m_data");
}

//=========================================================================
void DataNode::Sanitize (void) {
    m_name[s_nameSize-1] = '\0';
    //if (m_type == Type::String)
    m_data.m_string[s_stringDataSize-1] = '\0';
}

//=========================================================================
bool DataNode::QueryInt (int * outInt) const {
    if (m_type != Type::Int)
        return false;
    *outInt = m_data.m_int;
        return true;
}

//=========================================================================
bool DataNode::QueryFloat (float * outFloat) const {
    if (m_type != Type::Float)
        return false;
    *outFloat = m_data.m_float;
        return true;
}

//=========================================================================
bool DataNode::QueryString (char * outString, int out_m_stringsize_in_elements)
const {
    if (m_type != Type::String)
        return false;
    int copy_counter = 0;

    while (copy_counter < s_stringDataSize &&
     m_data.m_string[copy_counter] &&
     copy_counter < out_m_stringsize_in_elements - 1) {
        outString[copy_counter] = m_data.m_string[copy_counter];
        ++copy_counter;
    }
    outString[copy_counter] = '\0';

    return true;
}

//=========================================================================
bool DataNode::QueryBool (bool * outBool) const {
    if (m_type != Type::Bool)
        return false;
    *outBool = m_data.m_bool;
        return true;
}

//=========================================================================
DataNode * DataNode::SetName (const char * new_name) {
    strcpy(m_name, new_name);
    return this;
}

//=========================================================================
DataNode * DataNode::SetNameSecure (const char * new_name, int size_in_elements) {
    int i = 0;
    // write empty string first, in case null or empty string was given
    m_name[0] = '\0';

    if (new_name != nullptr) {
        while (i < size_in_elements  &&  i < s_nameSize) {
            m_name[i] = new_name[i];
            ++i;
        }

        // write null-terminating character
        m_name[i] = '\0';
    }

    // write null at last possible char, to protect against read-until-null-char
    //   reading outside our buffer.
    //m_name[s_nameSize - 1] = '\0';
    return this;
}

//=========================================================================
DataNode * DataNode::SetType (Type type) {
    m_type = type;
    if (m_type != Type::Object && m_type != Type::Array)
        DeleteAllChildren();
    else
        m_children.reserve(4);
    return this;
}

//=========================================================================
DataNode * DataNode::SetInt (int new_int) {
    m_data.m_int = new_int;
    SetType(Type::Int);
    return this;
}

//=========================================================================
DataNode * DataNode::SetFloat (float new_float) {
    m_data.m_float = new_float;
    SetType(Type::Float);
    return this;
}

//=========================================================================
DataNode * DataNode::SetString (const char * new_string) {
    strcpy(m_data.m_string, new_string);
    SetType(Type::String);
    return this;
}

//=========================================================================
DataNode * DataNode::SetStringSecure (const char * new_string,
int size_in_elements) {
    int i = 0;
    // write empty string first, in case a null or empty string was given
    m_data.m_string[0] = '\0';

    if (new_string != nullptr) {
        while (i < size_in_elements  &&  i < s_stringDataSize) {
            m_data.m_string[i] = new_string[i];
            ++i;
        }

        // null-terminate our string after copying
        m_data.m_string[i < s_stringDataSize ? i : s_stringDataSize - 1] = '\0';
    }

    //m_data.m_string[s_stringDataSize - 1] = '\0';
    SetType(Type::String);
    return this;
}

//=========================================================================
DataNode * DataNode::SetBool (bool new_bool) {
    SetType(Type::Bool);
    m_data.m_bool = new_bool;
    return this;
}

//=========================================================================
const DataNode * DataNode::GetChildByName (const char * name) const {
    int childCount = int(m_children.size());
    for (int i = 0;  i < childCount;  ++i) {
        // if we find a match, return it
        if (!strcmp(m_children[i].GetName(), name))
            return &m_children[i];
    }
    return nullptr;
}

//=========================================================================
DataNode * DataNode::GetChildByName (const char * name) {
    int childCount = int(m_children.size());
    for (int i = 0;  i < childCount;  ++i) {
        // if we find a match, return it
        if (!strcmp(m_children[i].GetName(), name))
            return &m_children[i];
    }
    return nullptr;
}

//=========================================================================
DataNode * DataNode::AppendNewChild (void) {
    //m_children.resize(m_children.size() + 1);
    m_children.push_back(CSaruDataMap::DataNode());
    if (m_type != Type::Object && m_type != Type::Array)
        SetType(Type::Object);
    return &m_children.back();
}

//=========================================================================
DataNode * DataNode::InsertNewChild (int index) {
    #ifdef _DEBUG
        assert(index >= 0 && "DataNode::InsertNewChild() called with a negative "
         "index.");
        assert(index <= GetChildCount() && "DataNode::InsertNewChild() called with "
         "invalid index.  Index is greater to the number of children of this "
         "DataNode.");
    #endif

    // copy all children from [index, last_child] in reverse order
    return &*m_children.insert(m_children.begin() + index, DataNode());
    // reset the child at [index]
    //  (currently unnecessary.  std::vector::insert() takes a copy of a
    //  default-constructed DataNode above)
    //m_children[index].SetType(kNull);
    //m_children[index].m_stringm_data[0] = '\0';
    // return a pointer to the 'new' child at [index]
    //  (currently being returned above.  The returned iterator is dereferenced,
    //  then the address of that DataNode is taken)
    //return &m_children[index];
}

//=========================================================================
void DataNode::DeleteLastChild (void) {
    if (!m_children.empty())
        m_children.pop_back();
}

//=========================================================================
DataNode * DataNode::DeleteAllChildren (void) {
    m_children.clear();
    return this;
}

} // namespace CSaruDataMap

#if _MSC_VER > 1000
#	pragma warning(pop)
#endif
