#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
namespace nycpp
{
    using std::vector;
    using std::set;
    using std::map;
    using std::string;
    using std::unordered_map;
    using std::unordered_set;


    class SplitTool;

    class DictProducer
    {
    public:
        DictProducer(const string&);
        DictProducer(const string&, SplitTool* );

        void buildEnDict();
        void buildCnDict();
        void createIndex();
        void storeDict(const string&);
        void storeIndex(const string& );
    private:
        void getFiles(const string&);
        size_t getNumByte_UTF8(const char);
        void showFiles() const;
        void showDict() const;
        void loadStopWord(const string&);

    private:
    vector<string> _files;
    vector<string> _enfiles;
    vector<std::pair<string, int>> _dictR;
    unordered_map<string, int> _dictW;

    SplitTool* _splitTool;
    unordered_map<string, set<int>> _index;
    unordered_set<string> _stopWordSet;
    string _cnDir;
    string _enDir;
    };

}