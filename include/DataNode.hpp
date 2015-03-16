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

#pragma once

#include <vector>

namespace CSaruContainer {

// ASSUMPTION: Does not contain a vtable. // TODO: Double-check this requirement.
// ASSUMPTION: Uses 1-byte chars.
class DataNode {
public:
    // Type and Constants
    static const unsigned s_nameSize = 28;
    static const unsigned s_stringDataSize = 64;
    static const unsigned s_maxDepth = 7;  // TODO: Error on passing MaxDepth

    enum class Type {
        Unused = 0,
        Null,
        Object,
        Array,
        Bool,
        Int,
        Float,
        String
    };

public:
    // Data
    char m_name[s_nameSize];
    Type m_type;

    union {
        int m_int;
        float m_float;
        char m_string[s_stringDataSize];
        bool m_bool;
    } m_data;

    std::vector<DataNode> m_children;

public:
    // Methods
    DataNode (void) : m_type(Type::Unused)  {
        m_name[0] = '\0';
    }

    // WARNING: potentially VERY SLOW
    DataNode (const DataNode & other);

    // WARNING: potentially VERY SLOW
    DataNode & operator=(const DataNode & rhs);

    explicit DataNode (const char * name, Type type = Type::Null);
    explicit DataNode (const char * name, int int_data);
    explicit DataNode (const char * name, float m_floatdata);
    explicit DataNode (const char * name, const char * m_stringdata);
    explicit DataNode (const char * name, bool m_booldata);


    void Initialize (void);
    //void Reset (void);
    //bool Validate (void) const;

    // sets the last char of m_name to NULL.  Additionally, if the m_type is
    //  currently kString, sets the last char of m_stringm_data to NULL.
    void Sanitize (void);

    inline int GetInt (void) const          { return m_data.m_int; }

    // only writes to out_int if this is of type kInt
    // ASSUMPTION: it is valid to write to out_int (it's not NULL, etc.)
    // RETURNS: true if out_int was written to
    bool QueryInt (int * out_int) const;

    inline float GetFloat (void) const      { return m_data.m_float; }

    // only writes to out_float if this is of type kFloat
    // ASSUMPTION: it is valid to write to out_float (it's not NULL, etc.)
    // RETURNS: true if out_float was written to
    bool QueryFloat (float * out_float) const;

    const char * GetString (void) const      { return m_data.m_string; }

    // only writes to out_string if this is of type kString
    // ASSUMPTION: it is valid to write to out_string (it's not NULL, etc.)
    // RETURNS: true if out_string was written to
    bool QueryString (char * out_string, int out_m_stringsize_in_elements) const;

    bool GetBool (void) const               { return m_data.m_bool; }

    // only writes to out_bool if this is of type kBool
    // ASSUMPTION: it is valid to write to out_bool (it's not NULL, etc.)
    // RETURNS: true if out_bool was written to
    bool QueryBool (bool * out_bool) const;

    bool IsNull (void) const                { return m_type == Type::Null; }

    inline const char * GetName (void) const { return m_name; }

    // new_name must be null-terminated
    DataNode * SetName (const char * new_name);

    // size_in_elements should not include the NULL terminator.  If new_string
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    DataNode * SetNameSecure (const char * new_name, int size_in_elements);

    inline Type GetType (void) const        { return m_type; }

    // also deletes children if this is being changed to a type that cannot have
    //  children
    // WARNING: If this DataNode has any children, then this action invalidates
    //  any DataMapMutators/Readers that happen to be pointing to any of those
    //  children without any way of detecting the invalidation.
    DataNode * SetType (Type type);

    // convenience function to check if the current node is either an object
    //   or an array.
    inline bool IsContainerType (void) const { return m_type == Type::Object || m_type == Type::Array; }

    // WARNING: If this DataNode has any children, then this action invalidates
    //  any DataMapMutators/Readers that happen to be pointing to any of those
    //  children without any way of detecting the invalidation.
    DataNode * SetInt (int new_int);

    // WARNING: If this DataNode has any children, then this action invalidates
    //  any DataMapMutators/Readers that happen to be pointing to any of those
    //  children without any way of detecting the invalidation.
    DataNode * SetFloat (float new_float);

    // new_string must be null-terminated
    // WARNING: If this DataNode has any children, then this action invalidates
    //  any DataMapMutators/Readers that happen to be pointing to any of those
    //  children without any way of detecting the invalidation.
    DataNode * SetString (const char * new_string);

    // size_in_elements should not include the NULL terminator.  If new_string
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    // WARNING: If this DataNode has any children, then this action invalidates
    //  any DataMapMutators/Readers that happen to be pointing to any of those
    //  children without any way of detecting the invalidation.
    DataNode * SetStringSecure (const char * new_string, int size_in_elements);

    // WARNING: If this DataNode has any children, then this action invalidates
    //  any DataMapMutators/Readers that happen to be pointing to any of those
    //  children without any way of detecting the invalidation.
    DataNode * SetBool (bool new_bool);

    inline int GetChildCount (void) const   { return static_cast<int>(m_children.size()); }

    inline bool HasChildren (void) const    { return !m_children.empty(); }

    inline const DataNode * GetChildFast (int index) const { return &m_children[index]; }

    inline DataNode * GetChildFast (int index) { return &m_children[index]; }

    // returns a NULL pointer on invalid indices
    inline const DataNode * GetChildSafe (int index) const {
        if (index < 0 || index >= GetChildCount())
            return NULL;
        return GetChildFast(index);
    }

    // returns a NULL pointer on invalid indices
    inline DataNode * GetChildSafe (int index) {
        if (index < 0 || index >= GetChildCount())
            return NULL;
        return GetChildFast(index);
    }

    // children are assumed to have unique names (if they have names; array
    //  children don't have names).  If there are duplicates: 1) You are wrong,
    //  and 2) GetChild will always return the first one of the matching name.
    const DataNode * GetChildByName (const char * name) const;
    DataNode * GetChildByName (const char * name);

    // also changes type to Type::Object if this was previously not of a type which
    //  is permitted to have children.
    // WARNING: Invalidates any DataMapMutators/Readers that happen to be
    //  pointing to any of this DataNode's children without any way of detecting
    //  the invalidation.
    DataNode * AppendNewChild (void);

    // WARNING: potentially _*VERY SLOW *_.  Must shift all following children in
    //  the m_children array by copying them one at a time.  This will cause them
    //  to copy their m_name, data, type, and all their children (which then must
    //  copy all their children, and so on).
    // WARNING: Invalidates any DataMapMutators/Readers that happen to be
    //  pointing to any of this DataNode's children without any way of detecting
    //  the invalidation.
    DataNode * InsertNewChild (int index);

    // WARNING: Invalidates any DataMapMutators/Readers that happen to be
    //  pointing to any of this DataNode's children without any way of detecting
    //  the invalidation.
    void DeleteLastChild (void);

    // WARNING: Invalidates any DataMapMutators/Readers that happen to be
    //  pointing to any of this DataNode's children without any way of detecting
    //  the invalidation.
    // RETURNS: this.
    DataNode * DeleteAllChildren (void);
};

} // namespace CSaruContainer
