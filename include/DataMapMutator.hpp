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

#pragma once

#include <vector>

namespace CSaruContainer {

class DataNode;

class DataMapMutator {
private:
    // Data
    DataNode *              m_node;
    std::vector<DataNode *> m_nodeStack; // m_nodeStack does *not* contain m_node.

public:
    explicit DataMapMutator (DataNode * dataNode);
    DataMapMutator (const DataMapMutator & other);
    DataMapMutator & operator= (const DataMapMutator & rhs);

    inline const DataNode * GetCurrentNode () const    { return m_node; }
    inline DataNode * GetCurrentNode ()                { return m_node; }

    // RETURNS: 0 if invalidated, 1 if at the root node, 2 if at one of the root
    //  node's children, and so on.
    inline int GetCurrentDepth () const                { return int(m_nodeStack.size() + (m_node == nullptr ? 0 : 1)); }

    inline bool IsValid () const                       { return m_node != nullptr; }

    //
    // Navigation methods
    //

    // goes down from one node to one of its children.  Using this function to
    //  go from the current node to one of its non-children is undefined.  This
    //  class makes no attempt at saving you from this error; for performance
    //  reasons.
    // WARNING: You shouldn't need to use this function!
    DataMapMutator & PushNode (DataNode * node);

    // return to the parent node.
    // NOTE: If this is used on the root node, the Mutator becomes invalidated.
    DataMapMutator & PopNode ();
    // synonym for PopNode.
    // NOTE: If this is used on the root node, the Mutator becomes invalidated.
    inline DataMapMutator & ToParent ()                { return PopNode(); }

    // NOTE: Advanced use only!
    DataNode * GetParentNode ();

    // if there are no children, one will be created.
    DataMapMutator & ToFirstChild ();

    // if there are no children, one will be created.
    DataMapMutator & ToLastChild ();

    // if there is no child at the given index, null children will be created
    //  until a child at the given index can be pointed at.
    // WARNING: If no child at the given index exists, this invalidates any
    //  DataMapMutators/Readers which are pointing at any of this DataNode's
    //  children.
    DataMapMutator & ToChild (int index);

    // if no child with such a name exists, a new null _last_ child will be added
    //  with the given name.
    // WARNING: If no child of the given name exists, this invalidates any
    //  DataMapMutators/Readers which are pointing at any of this DataNode's
    //  children.
    DataMapMutator & ToChild (const char * name);

    // theoretically slow.  Nodes have no knowledge of their siblings, so we
    //  actually PopNode(-ish), then ToChild(prevIndex + 1).  prevIndex must
    //  also be calculated at this time by walking the current parent's children.
    // NOTE: If this is used on the root node, the Mutator becomes invalidated.
    // WARNING: If no next sibling exists, this invalidates any
    //  DataMapMutators/Readers which are pointing at any of this DataNode's
    //  siblings or children.
    DataMapMutator & ToNextSibling ();

    // theoretically slow.  Nodes have no knowledge of their siblings, so we
    //  actually PopNode(-ish), then ToChild(prevIndex - 1).  prevIndex must
    //  also be calculated at this time by walking the current parent's children.
    // NOTE: If this is used on the root node, the Mutator becomes invalidated.
    // WARNING: If no previous sibling exists, this invalidates any
    //  DataMapMutators/Readers which are pointing at any of this DataNode's
    //  siblings or children.
    DataMapMutator & ToPreviousSibling ();

    bool IsFirstChild ();
    // synonym for IsFirstChild().
    inline bool IsFirstSibling ()                      { return IsFirstChild(); }

    //
    // Mutation methods
    //
    // Some will have navigation side effects.
    //

    // most common type for children.  Same as array, but children are named.
    DataMapMutator & SetToObjectType ();

    // same as object, but children are unnamed.
    DataMapMutator & SetToArrayType ();

    // if children of the current node exist, they will be destroyed.
    // This does not write anything to the Node's data.  It is assumed you will
    //   set the data to be true or false yourself.
    DataMapMutator & SetToBooleanType ();

    // if children of the current node exist, they will be destroyed.
    DataMapMutator & SetToNullType ();

    // New child will be appended to end of any current children.
    //   If the current node wasn't of a sort that could have children, it will
    //   be changed to such (and lose any data it previously held).
    // name [in]: can be NULL.
    // WARNING: This invalidates any DataMapReader/DataMapMutators that were
    //   referring to any of this one's node's children!
    DataMapMutator & CreateChild (char const * name = nullptr);

    DataMapMutator & CreateChildSafe (char const * name, std::size_t nameLen);

    // name [in]: can be NULL.
    DataMapMutator & CreateAndGotoChild (char const * name = nullptr);

    DataMapMutator & CreateAndGotoChildSafe (char const * name, std::size_t nameLen);

    void WriteName (char const * name);

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    void WriteNameSecure (char const * name, int sizeInElements);

    void Write (                   bool boolValue);
    void Write (char const * name, bool boolValue);
    void Write (                   int intValue);
    void Write (char const * name, int intValue);
    void Write (                   float floatValue);
    void Write (char const * name, float floatValue);
    void Write (                   char const * stringValue);
    void Write (char const * name, char const * stringValue);

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    void WriteSafe (char const * name, int nameSizeInElements, bool boolValue);

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    void WriteSafe (char const * name, int nameSizeInElements, int intValue);

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    void WriteSafe (char const * name, int nameSizeInElements, float floatValue);

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    void WriteSafe (char const * stringValue, int valueSizeInElements);

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    void WriteSafe (char const * name, int nameSizeInElements, char const * stringValue, int valueSizeInElements);

    // Calls ToNextSibling (theoretically slow) count times.
    // NOTE: Since this is a Mutator, children will be created if none exist
    //   to walk over.
    void Walk (int count);

    inline void WriteWalk (                  bool boolValue) {
        Write(boolValue);
        ToNextSibling();
    }

    inline void WriteWalk (char const * name, bool boolValue) {
        Write(name, boolValue);
        ToNextSibling();
    }

    inline void WriteWalk (                  int intValue) {
        Write(intValue);
        ToNextSibling();
    }

    inline void WriteWalk (char const * name, int intValue) {
        Write(name, intValue);
        ToNextSibling();
    }

    inline void WriteWalk (                  float floatValue) {
        Write(floatValue);
        ToNextSibling();
    }

    inline void WriteWalk (char const * name, float floatValue) {
        Write(name, floatValue);
        ToNextSibling();
    }

    inline void WriteWalk (                  char const * stringValue) {
        Write(stringValue);
        ToNextSibling();
    }

    inline void WriteWalk (char const * name, char const * stringValue) {
        Write(name, stringValue);
        ToNextSibling();
    }

    // sizeInElements should not include the NULL terminator.
    void WriteWalkSafeBooleanValue (char const * name, int nameSizeInElements, bool value);

    // sizeInElements should not include the NULL terminator.
    void WriteWalkSafeIntegerValue (char const * name, int nameSizeInElements, int value);

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    inline void WriteWalkSafeNullValue (char const * name, int nameSizeInElements) {
        WriteNameSecure(name, nameSizeInElements);
        SetToNullType();
        ToNextSibling();
    }

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    inline void WriteWalkSafe (char const * name, int nameSizeInElements, bool boolValue) {
        WriteSafe(name, nameSizeInElements, boolValue);
        ToNextSibling();
    }

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    inline void WriteWalkSafe (char const * name, int nameSizeInElements, int intValue) {
        WriteSafe(name, nameSizeInElements, intValue);
        ToNextSibling();
    }

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    inline void WriteWalkSafe (char const * name, int nameSizeInElements, float floatValue) {
        WriteSafe(name, nameSizeInElements, floatValue);
        ToNextSibling();
    }

    // sizeInElements should not include the NULL terminator.  If newString
    //  is too large, as much of it as possible will be copied, and the internal
    //  copy will be NULL-terminated.
    inline void WriteWalkSafe (
        char const * name,
        int          nameSizeInElements,
        char const * stringValue,
        int          valueSizeInElements
    ) {
        WriteSafe(name, nameSizeInElements, stringValue, valueSizeInElements);
        ToNextSibling();
    }

    // Deletes this node's last ((count)) children.
    // NOTE: This invalidates any DataMapMutators/DataMapReaders currently
    //   referring to any deleted children or any of their children.
    void DeleteLastChildren (int count);

    //
    // Reading
    //
    // Some will have navigation side effects.
    //

    const char * ReadName () const;
    bool         ReadBool () const;
    int          ReadInt () const;
    float        ReadFloat () const;
    const char * ReadString () const;

    // there's no ReadNameSafe().  This would be to copy the name to a given
    //  buffer.

    // RETURNS: true on success (and the out parameter is written to).
    //          false otherwise, and the out parameter is not written to.
    bool ReadBoolSafe (bool * outBool) const;

    // RETURNS: true on success (and the out parameter is written to).
    //          false otherwise, and the out parameter is not written to.
    bool ReadIntSafe (int * outInt) const;

    // RETURNS: true on success (and the out parameter is written to).
    //          false otherwise, and the out parameter is not written to.
    bool ReadFloatSafe (float * outFloat) const;

    // RETURNS: true on success (and the out parameter is written to).
    //          false otherwise, and the out parameter is not written to.
    bool ReadStringSafe (char * outString, int bufferSizeInElements) const;

    inline bool ReadBoolWalk ()                        {
        const bool result = ReadBool();
        ToNextSibling();
        return result;
    }

    inline int ReadIntWalk ()                          {
        const int result = ReadInt();
        ToNextSibling();
        return result;
    }

    inline float ReadFloatWalk ()                      {
        const float result = ReadFloat();
        ToNextSibling();
        return result;
    }

    inline const char * ReadStringWalk ()              {
        const char * result = ReadString();
        ToNextSibling();
        return result;
    }

    inline bool ReadBoolWalkSafe (bool * outBool)         {
        const bool result = ReadBoolSafe(outBool);
        ToNextSibling();
        return result;
    }

    inline bool ReadIntWalkSafe (int * outInt)            {
        const bool result = ReadIntSafe(outInt);
        ToNextSibling();
        return result;
    }

    inline bool ReadFloatWalkSafe (float * outFloat)      {
        const bool result = ReadFloatSafe(outFloat);
        ToNextSibling();
        return result;
    }

    inline bool ReadStringWalkSafe (char * outString, int bufferSizeInElements) {
        const bool result = ReadStringSafe(outString, bufferSizeInElements);
        ToNextSibling();
        return result;
    }
};

} // namespace CSaruContainer
