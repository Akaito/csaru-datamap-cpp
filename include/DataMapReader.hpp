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

#pragma once

#include <vector>

#include <csaru-core-cpp.h>

// TODO: Speed up Reader by maintaining the current child's index?

namespace CSaruContainer {

class DataNode;
//class DataMapMutator;

class DataMapReader {
protected:
    // Helpers
    void PushNode (const DataNode * node);

    // Data
    const DataNode *              m_node;
    std::vector<const DataNode *> m_nodeStack;

public:
    // Methods
    explicit DataMapReader (const DataNode * node);
    DataMapReader (const DataMapReader & other);
    DataMapReader & operator=(const DataMapReader & rhs);

    //DataMapReader & CopyMutator (const DataMapMutator * mutator);

    inline const DataNode * GetCurrentNode (void) const { return m_node; }
    inline const DataNode * GetCurrentNode (void)       { return m_node; }

    // RETURNS: -1 if invalidated, 0 if at the root node, 1 if at one of the root
    //  node's children, and so on.
    inline int GetCurrentDepth (void) const            { return int(m_nodeStack.size() - 1 + (m_node == NULL ? 0 : 1)); }

    inline bool IsValid (void) const                   { return m_node != NULL; }

    ///////
    // navigation (begin)

    // return to the parent node.
    // NOTE: If this is used on the root node, the Reader becomes invalidated.
    DataMapReader & PopNode (void);

    // if there are no children, the current node will become NULL.  You must
    //  PopNode to back out of this state.
    DataMapReader & ToFirstChild (void);

    // if there are no children, the current node will become NULL.  You must
    //  PopNode to back out of this state.
    DataMapReader & ToLastChild (void);

    // if there is no child at the given index, the current node will become
    //  NULL.  You must PopNode to back out of this state.
    DataMapReader & ToChild (int index);

    // if no child with such a name exists, the current node will become
    //  NULL.  You must PopNode to back out of this state.
    DataMapReader & ToChild (const char * name);

    // theoretically slow.  Nodes have no knowledge of their siblings, so we
    //  actually PopNode(-ish), then ToChild(prevIndex + 1).  prevIndex must
    //  also be calculated at this time by walking the current parent's children.
    // NOTE: If this is used on the root node, the Reader becomes invalidated.
    DataMapReader & ToNextSibling (void);

    // theoretically slow.  Nodes have no knowledge of their siblings, so we
    //  actually PopNode(-ish), then ToChild(prevIndex - 1).  prevIndex must
    //  also be calculated at this time by walking the current parent's children.
    // NOTE: If this is used on the root node, the Reader becomes invalidated.
    DataMapReader & ToPreviousSibling (void);

    // navigation (end)
    ///////
    // reading (begin)

    const char * ReadName (void) const;
    bool        ReadBool (void) const;
    int         ReadInt (void) const;
    float       ReadFloat (void) const;
    const char * ReadString (void) const;

    // there's no ReadNameSafe ().  This would be to copy the name to a given
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

    inline bool ReadBoolWalk (void)                    {
        const bool result = ReadBool();
        ToNextSibling();
        return result;
    }

    inline int ReadIntWalk (void)                      {
        const int result = ReadInt();
        ToNextSibling();
        return result;
    }

    inline float ReadFloatWalk (void)                  {
        const float result = ReadFloat();
        ToNextSibling();
        return result;
    }

    inline const char * ReadStringWalk (void)           {
        const char * result = ReadString();
        ToNextSibling();
        return result;
    }

    inline bool ReadBoolWalkSafe (bool * outBool)      {
        const bool result = ReadBoolSafe(outBool);
        ToNextSibling();
        return result;
    }

    inline bool ReadIntWalkSafe (int * outInt)         {
        const bool result = ReadIntSafe(outInt);
        ToNextSibling();
        return result;
    }

    inline bool ReadFloatWalkSafe (float * outFloat)   {
        const bool result = ReadFloatSafe(outFloat);
        ToNextSibling();
        return result;
    }

    inline bool ReadStringWalkSafe (char * outString, int bufferSizeInElements) {
        const bool result = ReadStringSafe(outString, bufferSizeInElements);
        ToNextSibling();
        return result;
    }

    // reading (end)
    ///////

    DataMapReader () = delete;
};

} // namespace CSaruContainer
