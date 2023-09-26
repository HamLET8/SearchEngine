#pragma once

#include <string>
#include <queue>
#include <set>

namespace nycpp
{
using std::string;
using std::priority_queue;

class QResult
{
public:
    friend class QCompare;

    QResult(string word, int freq, int dist = 999)
        :_word(word), _freq(freq), _dist(dist)
    { }

    const string getWord() const
    {
        return _word;
    }

    void setDistance(size_t dist)
    {
        _dist = dist;
    }

private:
    string _word;
    int _freq;
    int _dist; 
};

/*
    less compare
    sort order : _dist , _freq, _word 
*/
class QCompare
{
public:
    bool operator()(const QResult& lhs, const QResult& rhs)
    {
        if(lhs._dist > rhs._dist) 
        {
            return true;
        }

        if(lhs._dist == rhs._dist)
        {
            if(lhs._freq > rhs._freq)
            {
                return true;
            }

            if(lhs._freq < rhs._freq)
            {
                return false;
            }

            return (lhs._word > rhs._word);
        }

        return false;
    }
};


class KeyRecommander
{
public:
using PriorQueResult = priority_queue<QResult, std::vector<QResult>, QCompare>;

public: 
    KeyRecommander() = default;
    ~KeyRecommander() = default;
    string query(const string&);

private:
    void queryIndexedtable();
    void statistic(const string&, std::set<int>&, PriorQueResult&);
    size_t nBytesCode(const char ch);
    size_t length(const string& str);
    int distance(const string&, const string&);

private:
    //serialize the message to json format.
    string serializeNull();
    string serialize(const std::vector<string>& ) ;

private:
    PriorQueResult  _qResultQue;
};


}