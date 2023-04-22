## 其余变量
VER:=debug ## 调试选项
OPTM:=elide ## 构造器优化选项
STD:=c++17
CPP:=g++

## 头文件
HEADER:=include
## 可执行程序
EXE_RECOMMENDER:=bin/offline1
EXE_SEARCHER:=bin/offline2
EXE_OFFLINE:= $(EXE_RECOMMENDER) $(EXE_SEARCHER)
EXE_SERVER:=bin/server
EXE_CLIENT:=bin/client
EXE_TEST:=bin/test
EXES:= $(EXE_OFFLINE) $(EXE_SERVER) $(EXE_CLIENT) $(EXE_TEST)
## 源程序
SRC_RECOMMENDER:=$(wildcard src/offline/module1/*.cc)
SRC_SEARCHER:=$(wildcard src/offline/module2/*.cc)
SRC_SERVER:=$(wildcard src/online/*.cc) $(wildcard src/online/tcpServer/*.cc)
SRC_CLIENT:=$(wildcard src/client/*.cc)
SRC_TEST:=$(wildcard src/test/*.cc)

## 测试变量
# all:
# 	echo $(SRC_SERVER) 

## 编译参数确定
ifeq ($(VER), debug) ## ifeq后要有空格
	OPT+=-g
	OPT+=-D __DEBUG__
endif


ifeq ($(OPTM), no-elide)
	OPT+=-fno-elide-constructors
endif


## 全编译（默认）
all:$(EXE_SERVER) $(EXE_CLIENT)
## 仅编译offline
offline:$(EXE_OFFLINE)
## 仅编译recommender
recommender:$(EXE_RECOMMENDER)
## 仅编译searcher
searcher:$(EXE_SEARCHER)
## 仅编译server
server:$(EXE_SERVER)
## 仅编译client
client:$(EXE_CLIENT)
## 仅编译test
test:$(EXE_TEST)


## recommender的编译与链接
$(EXE_RECOMMENDER):$(SRC_RECOMMENDER)
	$(CPP) $(OPT) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) 
## searcher的编译与链接
$(EXE_SEARCHER):$(SRC_SEARCHER)
	$(CPP) $(OPT) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) 
## server的编译与链接
$(EXE_SERVER):$(SRC_SERVER)
	$(CPP) -g $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) 
## client的编译与链接
$(EXE_CLIENT):$(SRC_CLIENT)
	$(CPP) $(OPT) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) 
## test的编译与链接
$(EXE_TEST):$(SRC_TEST)
	$(CPP) $(OPT) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) 


## 伪目标
.PHONY:clean rebuild
clean:
	$(RM) $(EXES)
rebuild:clean $(EXES)
