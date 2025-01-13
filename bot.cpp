#include <stack>
#include <vector>
#include <queue>
#include <sstream>
#include <iostream>
#include "jsoncpp/json.h" 
#include <string>
#include <random>
#include <limits>
#include <cmath>
#include <thread>
#include <limits>
#include <memory>
#include <functional>
#include <future>
#include <condition_variable>
#include <string.h>
#include <cstring>
using namespace std;
class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop ||!tasks.empty(); });
                        if (stop && tasks.empty()) {
                            return;
                        }
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }


    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }


    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }


private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

class Board {
public:
    Board(int size = 15);  // 构造函数，初始化棋盘
    bool placePiece(int x, int y, char piece);  // 放置棋子
    char getPiece(int x, int y) const;  // 获取某个位置的棋子
    bool isFull() const;  // 检查棋盘是否已满
    bool checkWin(char piece);  // 检查胜利条件
    bool isValidMove(int x, int y,char piece) const;  // 判断是否是有效落子
    int getSize()const;
    void clearBoard();
    bool checkWinFrom(int x, int y, char piece)const;
    bool isInBoard(int x,int y)const;
    bool checkjs(int x,int y,int type)const;
    bool hq();
    std::vector<std::vector<char>> grid;  // 棋盘存储
    bool hasneighbor(int x,int y,int dis);
    int size;  // 棋盘大小
    Board(Board& other):grid(other.grid),size(other.size){};
    int lastX,lastY;
    
private:
    std::stack<std::pair<int,int>> op;
};
class Player {
public:
    Player(Board* board, char piece);
    char getPiece() const;
    Board* board;

protected:
    char piece;
};
Board::Board(int size) : size(size), grid(size, std::vector<char>(size, '.')) {}

bool Board::placePiece(int x, int y, char piece) {
    if (isValidMove(x,y,piece)) {
        grid[x][y] = piece;
        lastX=x;
        lastY=y;
        if(piece!='.')op.push({x,y});
        else op.pop();
        return true;
    }
    else return false;
}

int Board::getSize()const{
    return size;
}

char Board::getPiece(int x, int y) const {
    if(isInBoard(x,y))return grid[x][y];
    return '.';
}

bool Board::isFull() const {
    for (const auto& row : grid) {
        for (char cell : row) {
            if (cell == '.') return false;
        }
    }
    return true;
}

bool Board::checkWin(char piece) {
    return checkWinFrom(lastX, lastY, piece);
}

bool Board::hasneighbor(int x,int y,int dis){
    for(int i=dis*-1;i<=dis;i++){
        for(int j=dis*-1;j<=dis;j++){
            if(!(i||j))continue;
            if(!isInBoard(x+i,y+j))continue;
            if(grid[i+x][j+y]!='.')return true;
        }
    }
    return false;
}

bool Board::isValidMove(int x, int y,char piece) const {
    if(!isInBoard(x,y))return false;
    if(piece!='.'&&grid[x][y]!='.')return false;
    if(piece=='X'){
        if(checkjs(x,y,2))return false;
        if(checkWinFrom(x,y,piece))return true;
        if(checkjs(x,y,1))return false;
    }
    return true;
}

bool Board::checkWinFrom(int x, int y, char piece) const{
    int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
    for(int i=0;i<4;i++){
        int xx1=x+dir[i][0],yy1=y+dir[i][1],xx2=x-dir[i][0],yy2=y-dir[i][1];
        bool pos=isInBoard(xx1,yy1),neg=isInBoard(xx2,yy2);
        int cnt=1;
        while(pos||neg){
            if(pos&&getPiece(xx1,yy1)==piece)cnt++;
            else pos=false;
            if(neg&&getPiece(xx2,yy2)==piece)cnt++;
            else neg=false;
            xx1+=dir[i][0],yy1+=dir[i][1],xx2-=dir[i][0],yy2-=dir[i][1];
            pos=pos&&isInBoard(xx1,yy1),neg=neg&&isInBoard(xx2,yy2);
        }
        if(cnt>=5)return true;
    }
    return false;
}

bool Board::checkjs(int x,int y,int type)const{
    if(type==2){
        int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
        for(int i=0;i<4;i++){
            int xx1=x+dir[i][0],yy1=y+dir[i][1],xx2=x-dir[i][0],yy2=y-dir[i][1];
            bool pos=isInBoard(xx1,yy1),neg=isInBoard(xx2,yy2);
            int cnt=0;
            while(pos||neg){
                if(pos&&getPiece(xx1,yy1)=='X')cnt++;
                else pos=false;
                if(neg&&getPiece(xx2,yy2)=='X')cnt++;
                else neg=false;
                if(pos)xx1+=dir[i][0],yy1+=dir[i][1];
                if(neg)xx2-=dir[i][0],yy2-=dir[i][1];
                pos=pos&&isInBoard(xx1,yy1),neg=neg&&isInBoard(xx2,yy2);
            }
            if(cnt>=5)return true;
        }
        return false;
    }
    else{
        if(checkWinFrom(x,y,'X'))return false;
        int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
        int cnt33=0,cnt44=0;
        for(int i=0;i<4;i++){
            int flag33=0,flag44=0;
            int cnt01=0,cnt02=0,cnt1=1;
            int xx1=x+dir[i][0],yy1=y+dir[i][1],xx2=x-dir[i][0],yy2=y-dir[i][1];
            bool pos=isInBoard(xx1,yy1),neg=isInBoard(xx2,yy2);
            while(cnt01<=2||cnt02<=2){
                if(isInBoard(xx1,yy1)&&pos&&getPiece(xx1,yy1)=='.')cnt01++;
                else if(isInBoard(xx1,yy1)&&pos&&getPiece(xx1,yy1)=='X')cnt1++;
                else pos=false;
                pos=pos&&isInBoard(xx1,yy1)&&cnt01<2;
                if(pos)xx1+=dir[i][0],yy1+=dir[i][1];
                if(cnt1==3&&cnt01+cnt02<=3&&(isInBoard(xx1,yy1)&&isInBoard(xx2,yy2)&&getPiece(xx1,yy1)=='.'&&getPiece(xx2,yy2)=='.'))flag33=1;
                if(cnt1==4&&(cnt01==1||cnt02==1||(isInBoard(xx1,yy1)&&getPiece(xx1,yy1)=='.')||(neg&&getPiece(xx2,yy2)=='.')))flag44=1;
                if(isInBoard(xx2,yy2)&&neg&&getPiece(xx2,yy2)=='.')cnt02++;
                else if(isInBoard(xx2,yy2)&&neg&&getPiece(xx2,yy2)=='X')cnt1++;
                else neg=false;
                neg=neg&&isInBoard(xx2,yy2)&&cnt02<2;
                if(neg)xx2-=dir[i][0],yy2-=dir[i][1];
                if(cnt1==3&&cnt01+cnt02<=3&&(isInBoard(xx1,yy1)&&isInBoard(xx2,yy2)&&getPiece(xx1,yy1)=='.'&&getPiece(xx2,yy2)=='.'))flag33=1;
                if(cnt1==4&&(cnt01==1||cnt02==1||(pos&&getPiece(xx1,yy1)=='.')||(neg&&getPiece(xx2,yy2)=='.')))flag44=1;
                if(!(pos||neg))break;
            }
            if(flag33)cnt33++;
            if(flag44)cnt44++;
        }
        if(cnt33>1||cnt44>1)return true;
    }
    return false;
}

bool Board::isInBoard(int x,int y)const{
    return x >= 0 && x < size && y >= 0 && y < size;
}
class AIPlayer : public Player {
public:
    double score[25][25]={},cal[15]={1,20,5000,800000,1e8,3919093202,3190932029,3199093202,3190932902,3190932902,3199093202,319093202,3199093202,3190932092,3190993202};
    AIPlayer(Board* board, char piece);
    void iniscore();
    void calscore(Board* board, char piece);
    struct move{
        int x,y;
        double score;
        bool operator<(const move& other) const {
            return score < other.score;  // 小于号的逻辑反转
        }   
    };
    struct Node{
        Node* parent=nullptr;
        std::vector<Node*> children;
        int x,y,wins=0,visits=0;
        Node(int x,int y, Node* p = nullptr) : x(x),y(y),parent(p) {}
    };
    void simulate(Node* node,std::shared_ptr<Board> simboard,char currentPlayer);
    move mtcl(int cnt);
    std::priority_queue<move> bestmove(Board* board);
};

Player::Player(Board* board, char piece) : board(board), piece(piece) {}

AIPlayer::AIPlayer(Board* board, char piece) : Player(board, piece) {}

void deleteNode(AIPlayer::Node* node) {
    for (auto child : node->children) {
        deleteNode(child);
    }
    delete node;
}

void AIPlayer::simulate(Node* node,std::shared_ptr<Board> simboard,char currentPlayer){
    char winner='A';
    int steps=0;
    while(winner=='A'){
        if(simboard->isFull()){
            winner='.';
            break;
        }
        int vis[15][15]={0};
        std::vector<std::pair<int,int>> moves;
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j,1)){
                    if(currentPlayer=='X'&&(simboard->checkjs(i,j,1)||simboard->checkjs(i,j,2)))continue;
                    if(simboard->checkWinFrom(i,j,currentPlayer)){
                        winner=currentPlayer;
                        goto win;
                    }
                    if(simboard->checkWinFrom(i,j,(currentPlayer=='X'?'O':'X'))){
                        if(!vis[i][j]){moves.emplace_back(i,j);vis[i][j]=1;}
                        goto xq;
                    }
                }
            }
        }
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j,1)){
                    if(currentPlayer=='X'&&(simboard->checkjs(i,j,1)||simboard->checkjs(i,j,2)))continue;
                    int dir[8][2]={{0,1},{1,0},{1,-1},{1,1},{0,-1},{-1,0},{-1,1},{-1,-1}};
                    for(int k=0;k<8;k++){
                        if(simboard->isInBoard(i+4*dir[k][0],j+4*dir[k][1])&&simboard->getPiece(i+3*dir[k][0],j+3*dir[k][1])==currentPlayer&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])==currentPlayer&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==currentPlayer&&simboard->getPiece(i+4*dir[k][0],j+4*dir[k][1])=='.'){
                            winner=currentPlayer;
                            goto win;
                        }
                        if(simboard->isInBoard(i+3*dir[k][0],j+3*dir[k][1])&&simboard->isInBoard(i-2*dir[k][0],j-2*dir[k][1])&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])==currentPlayer&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==currentPlayer&&simboard->getPiece(i-1*dir[k][0],j-1*dir[k][1])==currentPlayer&&simboard->getPiece(i+3*dir[k][0],j+3*dir[k][1])=='.'&&simboard->getPiece(i-2*dir[k][0],j-2*dir[k][1])=='.'){
                            winner=currentPlayer;
                            goto win;
                        }
                        if(simboard->isInBoard(i+2*dir[k][0],j+2*dir[k][1])&&simboard->isInBoard(i-3*dir[k][0],j-3*dir[k][1])&&simboard->getPiece(i-2*dir[k][0],j-2*dir[k][1])==currentPlayer&&simboard->getPiece(i-1*dir[k][0],j-1*dir[k][1])==currentPlayer&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==currentPlayer&&simboard->getPiece(i-3*dir[k][0],j-3*dir[k][1])=='.'&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])=='.'){
                            winner=currentPlayer;
                            goto win;
                        }
                        if(simboard->isInBoard(i+4*dir[k][0],j+4*dir[k][1])&&simboard->getPiece(i+3*dir[k][0],j+3*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+4*dir[k][0],j+4*dir[k][1])=='.'){
                            if(!vis[i][j]){moves.emplace_back(i,j);vis[i][j]=1;}
                        }
                        if(simboard->isInBoard(i+3*dir[k][0],j+3*dir[k][1])&&simboard->isInBoard(i-2*dir[k][0],j-2*dir[k][1])&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i-1*dir[k][0],j-1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+3*dir[k][0],j+3*dir[k][1])=='.'&&simboard->getPiece(i-2*dir[k][0],j-2*dir[k][1])=='.'){
                            if(!vis[i][j]){moves.emplace_back(i,j);vis[i][j]=1;}
                        }
                        if(simboard->isInBoard(i+2*dir[k][0],j+2*dir[k][1])&&simboard->isInBoard(i-3*dir[k][0],j-3*dir[k][1])&&simboard->getPiece(i-2*dir[k][0],j-2*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i-1*dir[k][0],j-1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i-3*dir[k][0],j-3*dir[k][1])=='.'&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])=='.'){
                            if(!vis[i][j]){moves.emplace_back(i,j);vis[i][j]=1;}
                        }
                    }
                }
            }
        }
        if(!moves.empty())goto xq;
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j,1)){
                    if(currentPlayer=='X'&&(simboard->checkjs(i,j,1)||simboard->checkjs(i,j,2)))continue;
                    if(!vis[i][j]){moves.emplace_back(i,j);vis[i][j]=1;}
                }
            }
        }
        if(moves.empty()){
            winner=(currentPlayer=='X'?'O':'X');
            break;
        }
        xq:;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, moves.size()-1);
        std::pair<int,int> best=moves[dis(gen)];
        simboard->placePiece(best.first,best.second, currentPlayer);
        steps++;
        if(steps>8){
            winner='.';
            break;
        }
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
    win:;
    while (node != nullptr) {
        node->visits++;
        if (winner == piece) {
            node->wins++;
        }
        if(winner==(piece=='X'?'O':'X'))node->wins--;
        node = node->parent;
    }
}

AIPlayer::move AIPlayer::mtcl(int cnt){
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            if(board->getPiece(i,j)=='.'&&board->hasneighbor(i,j,1)){
                if(piece=='X'&&(board->checkjs(i,j,1)||board->checkjs(i,j,2)))continue;
                if(board->checkWinFrom(i,j,piece)){
                    return {i,j,1e9};
                }
                if(board->checkWinFrom(i,j,(piece=='X'?'O':'X'))){
                    return {i,j,1e9};
                }
            }
        }
    }
    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<void>> futures;
    Node* root=new Node(-1,-1);
    for(int tttt=0;tttt<cnt;tttt++){
        Node* node=root;
        char currentPlayer=piece;
        std::shared_ptr<Board> simboard=std::make_shared<Board>(*board);
        while (!node->children.empty()) {
            Node* bestNode = nullptr;
            double bestValue = -std::numeric_limits<double>::infinity();

            for (auto child : node->children) {
                double winRate,uctValue;
                if (child->visits == 0) {
                    uctValue = std::numeric_limits<double>::infinity(); // 强制选择未访问节点
                } else {
                    double winRate = static_cast<double>(child->wins) / child->visits;
                    uctValue = winRate + std::sqrt(2 * std::log(node->visits + 1) / child->visits);
                }
                uctValue += static_cast<double>(std::rand()) / RAND_MAX * 1e-6; 
                if (uctValue > bestValue) {
                    bestValue = uctValue;
                    bestNode = child;
                }
            }
            node=bestNode;
            if(!simboard->placePiece(node->x,node->y,currentPlayer)){
                std::cerr<<"Invalid move!"<<std::endl;
                exit(1);
            }
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
        int visit[15][15]={0};
        std::vector<std::pair<int,int>> children;
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                if(simboard->getPiece(i,j)=='.'){
                    if(currentPlayer=='X'&&(simboard->checkjs(i,j,1)||simboard->checkjs(i,j,2)))continue;
                    if(simboard->checkWinFrom(i,j,currentPlayer)){
                        if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                        goto child;
                    }
                    if(simboard->checkWinFrom(i,j,(currentPlayer=='X'?'O':'X'))){
                        if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                    }
                }
            }
        }
        if(!children.empty())goto child;
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j,1)){
                    if(currentPlayer=='X'&&(simboard->checkjs(i,j,1)||simboard->checkjs(i,j,2)))continue;
                    int dir[8][2]={{0,1},{1,0},{1,-1},{1,1},{0,-1},{-1,0},{-1,1},{-1,-1}};
                    for(int k=0;k<8;k++){
                        if(simboard->isInBoard(i+4*dir[k][0],j+4*dir[k][1])&&simboard->getPiece(i+3*dir[k][0],j+3*dir[k][1])==currentPlayer&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])==currentPlayer&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==currentPlayer&&simboard->getPiece(i+4*dir[k][0],j+4*dir[k][1])=='.'){
                            if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                            goto child;
                        }
                        if(simboard->isInBoard(i+3*dir[k][0],j+3*dir[k][1])&&simboard->isInBoard(i-2*dir[k][0],j-2*dir[k][1])&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])==currentPlayer&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==currentPlayer&&simboard->getPiece(i-1*dir[k][0],j-1*dir[k][1])==currentPlayer&&simboard->getPiece(i+3*dir[k][0],j+3*dir[k][1])=='.'&&simboard->getPiece(i-2*dir[k][0],j-2*dir[k][1])=='.'){
                            if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                            goto child;
                        }
                        if(simboard->isInBoard(i+2*dir[k][0],j+2*dir[k][1])&&simboard->isInBoard(i-3*dir[k][0],j-3*dir[k][1])&&simboard->getPiece(i-2*dir[k][0],j-2*dir[k][1])==currentPlayer&&simboard->getPiece(i-1*dir[k][0],j-1*dir[k][1])==currentPlayer&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==currentPlayer&&simboard->getPiece(i-3*dir[k][0],j-3*dir[k][1])=='.'&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])=='.'){
                            if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                            goto child;
                        }
                        if(simboard->isInBoard(i+4*dir[k][0],j+4*dir[k][1])&&simboard->getPiece(i+3*dir[k][0],j+3*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+4*dir[k][0],j+4*dir[k][1])=='.'){
                            if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                        }
                        if(simboard->isInBoard(i+3*dir[k][0],j+3*dir[k][1])&&simboard->isInBoard(i-2*dir[k][0],j-2*dir[k][1])&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i-1*dir[k][0],j-1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+3*dir[k][0],j+3*dir[k][1])=='.'&&simboard->getPiece(i-2*dir[k][0],j-2*dir[k][1])=='.'){
                            if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                        }
                        if(simboard->isInBoard(i+2*dir[k][0],j+2*dir[k][1])&&simboard->isInBoard(i-3*dir[k][0],j-3*dir[k][1])&&simboard->getPiece(i-2*dir[k][0],j-2*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i-1*dir[k][0],j-1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i+1*dir[k][0],j+1*dir[k][1])==(currentPlayer=='X'?'O':'X')&&simboard->getPiece(i-3*dir[k][0],j-3*dir[k][1])=='.'&&simboard->getPiece(i+2*dir[k][0],j+2*dir[k][1])=='.'){
                            if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                        }
                    }
                }
            }
        }
        if(!children.empty())goto child;
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j,1)){
                    if(currentPlayer=='X'&&(simboard->checkjs(i,j,1)||simboard->checkjs(i,j,2)))continue;
                    if(!visit[i][j]){children.emplace_back(i,j);visit[i][j]=1;}
                }
            }
        }
        child:;
        for(int i=0;i<children.size();i++){
            Node* newNode=new Node(children[i].first,children[i].second,node);
            node->children.emplace_back(newNode);
        }
        futures.emplace_back(pool.enqueue([this, node, simboard, currentPlayer] {
            simulate(node, simboard, currentPlayer);
        }));
    }
    for(auto& future : futures) {
        future.get();
    }
    Node* bestNode = nullptr;
    double bestScore = -std::numeric_limits<double>::infinity();
    for (auto child : root->children) {
        double winRate = static_cast<double>(child->wins) / (child->visits + 1e-6);
        if (winRate > bestScore) {
            bestScore = winRate;
            bestNode = child;
        }
    }
    return {bestNode->x,bestNode->y,bestScore};
    deleteNode(root);
}

char Player::getPiece() const {
    return piece;
}

int main(){
    string str;
    getline(cin, str);
 	Json::Reader reader;
 	Json::Value input;
 	reader.parse(str, input);
    // 分析自己收到的输入和自己过往的输出，并恢复状态
    AIPlayer ai=AIPlayer(new Board(15),'X');
	int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++) {
		ai.board->placePiece(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), ai.getPiece()=='X'?'O':'X');
        ai.board->placePiece(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), ai.getPiece());
	}
    if(turnID==0&&input["requests"][turnID]["x"].asInt()==-1){
        Json::Value ret;
        ret["response"]["x"]=7;
        ret["response"]["y"]=7;
        Json::FastWriter writer;
        cout << writer.write(ret) << endl;
        return 0;
    }
	ai.board->placePiece(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(), ai.getPiece()=='X'?'O':'X');
	// 输出决策JSON
	Json::Value ret;
    AIPlayer::move best=ai.mtcl(1000);
    ret["response"]["x"]=best.x;
    ret["response"]["y"]=best.y;
	Json::FastWriter writer;
	cout << writer.write(ret) << endl;
}