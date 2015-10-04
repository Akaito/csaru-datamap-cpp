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

#include "../include/DataMapReader.hpp"
#include <csaru-core-cpp.h>

namespace CSaruContainer {

class DataMapReaderSimple {

private: // Data

    DataMapReader m_reader;
    int           m_errorDepth;

public: // Construction

    DataMapReaderSimple (const DataMapReader & reader);
    virtual ~DataMapReaderSimple ();
    
public: // Queries / Commands

    DataMapReader GetReader () const { return m_reader; }
    
    bool IsValid () const;
    
    bool ToChild (const char * name);
    bool ToFirstChild ();
    bool ToNextSibling ();
    bool ToParent ();
    
    bool EnterArray (const char * name); // Go's to child of <name>'s first child
    void ExitArray (); // Go to parent's parent
    
    bool Bool (const char * name) const;
    bool Bool (const char * name, bool defaultValue) const;
    
    int Int (const char * name) const;
    int Int (const char * name, int defaultValue) const;
    
    float Float (const char * name) const;
    float Float (const char * name, float defaultValue) const;
    
    std::string String (const char * name) const;
    std::string String (const char * name, const std::string & defaultValue) const;
    
    std::wstring WString (const char * name) const;
    std::wstring WString (const char * name, const std::wstring & defaultValue) const;

};

} // namespace CSaruContainer
