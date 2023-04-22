#include "CompareSimhash.hh"
#include "log4class.hh"
#include "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/Simhasher.hpp"


namespace nycpp
{
    bool CompareSimhash::same(const WebPage& webPage)
    {
        uint64_t pageHash; //hash value 4 *　16
        
        logDebug("call simhasher make func.");
        _simHasher.make(webPage.getContent(), _topN, pageHash);

        for(auto& hashNode : _hashList)
        {
            if( (pageHash & 0xffff000000000000) == hashNode._keyA )
            {
                for( auto& hashVal : hashNode._listA)
                {
                    if(simhash::Simhasher::isEqual(pageHash, hashVal))
                    {
                        return true;
                    }
                }

                hashNode._listA.push_back(pageHash);
            }

            if( (pageHash & 0x0000ffff00000000) == hashNode._keyB )
            {
                for( auto& hashVal : hashNode._listB)
                {
                    if(simhash::Simhasher::isEqual(pageHash, hashVal))
                    {
                        return true;
                    }
                }
                hashNode._listB.push_back(pageHash);
            }
            
            if( (pageHash & 0x00000000ffff0000) == hashNode._keyC )
            {
                for( auto& hashVal : hashNode._listC)
                {
                    if(simhash::Simhasher::isEqual(pageHash, hashVal))
                    {
                        return true;
                    }
                }
                hashNode._listC.push_back(pageHash);
            }
            
            if( (pageHash & 0x000000000000ffff) == hashNode._keyD )
            {
                for( auto& hashVal : hashNode._listD)
                {
                    if(simhash::Simhasher::isEqual(pageHash, hashVal))
                    {
                        return true;
                    }
                }
                hashNode._listD.push_back(pageHash);
            }

        }
        
        HashNode node(pageHash);
        _hashList.push_back(node);
        return false;
    }
};