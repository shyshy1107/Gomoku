#include "player.h"
#include "board.h"
#include <iostream>
#include <random>
#include <thread>
#include <limits>
#include <memory>
#include <cmath>
#include <functional>
#include <future>
#include <condition_variable>
#include <string.h>
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

Player::Player(Board* board, char piece) : board(board), piece(piece) {}

char Player::getPiece() const {
    return piece;
}

HumanPlayer::HumanPlayer(Board* board, char piece) : Player(board, piece) {}

void HumanPlayer::makeMove() {
}

AIPlayer::AIPlayer(Board* board, char piece) : Player(board, piece) {}
void AIPlayer::makeMove() {
    mtcl(7000);
}
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
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j)){
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
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j)){
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
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j)){
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

void AIPlayer::mtcl(int cnt){
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            if(board->getPiece(i,j)=='.'&&board->hasneighbor(i,j)){
                if(piece=='X'&&(board->checkjs(i,j,1)||board->checkjs(i,j,2)))continue;
                if(board->checkWinFrom(i,j,piece)){
                    board->placePiece(i,j,piece);
                    return;
                }
                if(board->checkWinFrom(i,j,(piece=='X'?'O':'X'))){
                    board->placePiece(i,j,piece);
                    return;
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
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j)){
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
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j)){
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
    std::cout<<"mtcl"<<std::endl;
    for (auto child : root->children) {
        double winRate = static_cast<double>(child->wins) / (child->visits + 1e-6);
        std::cout<<child->x<<' '<<child->y<<' '<<winRate<<std::endl;
        if (winRate > bestScore) {
            bestScore = winRate;
            bestNode = child;
        }
    }
    board->placePiece(bestNode->x,bestNode->y,piece);
    std::cout<<std::endl;
    deleteNode(root);
}