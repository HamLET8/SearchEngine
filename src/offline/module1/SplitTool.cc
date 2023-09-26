#include "SplitTool.hh"
#include <memory>
namespace nycpp
{
   SplitTool::SplitTool()
   :_jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH)
   {}

    std::shared_ptr<std::vector<string>> SplitTool::cut(const string& sentence)
   {
        std::shared_ptr<std::vector<string>> result(new std::vector<string>);
        _jieba.CutForSearch(sentence, *result);

        return result;  
   } 
} // namespace nycpp
