// 决策方法论大作业算法,S跳路由器为-1路由器
// 算法伪代码:
// 1)输入各个路由器名称以及到其他各路由器的开销，默认存在S路由器；
// 2)随机指定不循环的初始策略（为算法简易，假设共n个输入路由器，采取自0号路由器到第n-1号服务器依次路由，最后第n-1号服务器路由到S的初始策略），以迭代次数k=0开始；
// 3)计算每个路由器转发路由请求的代价函数Juk(x)=guk(x,y)+Puk(x,y)Juk(y)；
// 4)选择使Juk最小的策略uk+1,其中Juk(y)为目标转发节点到S节点的总开销，Puk(x,y)为某uk决策下自x状态转移到y状态的概率，本问题中为1，guk(x,y)为自x状态转移到y状态的开销，具体数值查表1获得；
// 5)若uk+1=uk，则结束，否则返回3)；
// 6)完成迭代，输出最终决策合集以及每个路由器路由到外网的开销，结束程序。

#include <iostream>
#include <vector>
#include <string>
#include <map>

// 定义路由器结构体:自身名字、直接转发开销、总开销(当前代价函数)、下一跳路由器
struct Router
{
    int number;               // 存储自身序号或名称(可选)
    std::map<int, int> costs; // 存储到其他路由器的直接开销,即开销表
    int costToInternet;       // 当前路由到外网的总开销,即当前代价函数
    int nextRouter;           // 当前路由器路由到外网所对应的的下一跳路由器
};
using Router = struct Router;

// 输入各个路由器名称以及到其他各路由器的开销,考虑程序鲁棒性,对非对称情况也考虑在内,两个路由器之间的开销需对双向分别输入
std::vector<Router> inputRouters()
{
    std::vector<Router> routers;
    int numRouters;
    printf("请输入路由器的数量：");
    scanf("%d", &numRouters);
    for (int i = 0; i < numRouters; i++)
    {
        Router router;
        router.number = i;
        int cost; // 花销
        for (int j = 0; j < numRouters; j++)
        {
            printf("请输入路由器 %d 到路由器 %d 的开销：", i, j);
            scanf("%d", &cost);
            router.costs[j] = cost; // 散列存储,没二维数组直接存储搜索高效,但封装方便
        }
        printf("请输入路由器 %d 到外网S路由器开销:", i);
        scanf("%d", &cost);
        router.costs[-1] = cost;
        routers.push_back(router); // 入栈
    }
    // 路由器存储向量不计入S=-1路由器
    return routers;
}

// 初始化策略以及代价函数
// policy方便封装,而routers里面的下一跳存储则是方便读取计算
std::vector<int> initialPolicy(std::vector<Router> &routers)
{
    std::vector<int> policy;
    policy.resize(routers.size()); // 确定策略集大小
    // 末路由初始化策略
    policy[routers.size() - 1] = -1;
    routers[routers.size() - 1].nextRouter = -1;
    routers[routers.size() - 1].costToInternet = routers[routers.size() - 1].costs[-1];
    for (int i = routers.size() - 2; i >= 0; i--)
    {
        policy[i] = i + 1;
        routers[i].nextRouter = i + 1;
        routers[i].costToInternet = routers[i].costs[i + 1] + routers[i + 1].costToInternet; // 递推计算,Juk(x) = g(x,y) + Juk(y)
    }
    return policy;
}

// 计算代价函数 Juk(x),应当是由外界给出一个新的代价函数集合,然后和旧的进行比较,再考虑是否更新
void calculateCostFunction(std::vector<Router> &routers, std::vector<int> &policy)
{
    int minCost = 0;
    int minRouter = -1;
    std::vector<Router> routersCopy = routers; // 复制品,非异步迭代
    // 对每个路由器计算代价函数
    for (int i = 0; i < routers.size(); i++)
    {
        int minCost = routersCopy[i].costToInternet; // 默认为原策略
        int minRouter = routersCopy[i].nextRouter;
        for (int j = 0; j < routers.size(); j++)
        {
            if (j == i) // 不原地踏步
                continue;
            else if (routersCopy[i].costs[j] + routersCopy[j].costToInternet < minCost) // 多个同等大小开销,选择第一个出现者
            {
                minCost = routersCopy[i].costs[j] + routersCopy[j].costToInternet; // 更新minCost
                minRouter = j;                                                     // 更新minRouter
            }
        }
        // 完成内部路由扫描后,还要考虑路由器直连外网的开销
        if (routersCopy[i].costs[-1] < minCost)
        {
            minCost = routersCopy[i].costs[-1];
            minRouter = -1;
        }
        // 更新策略
        policy[i] = minRouter;
        routersCopy[i].nextRouter = minRouter;
        routersCopy[i].costToInternet = minCost;
    }
    // 完成代价函数迭代后,整体更新路由器向量
    routers = routersCopy;
}

int main()
{
    // 可选自动化输入选项
    freopen("input.txt", "r", stdin);
    // 路由信息初始化,依次输入各路由器到其余路由器开销
    std::vector<Router> routers = inputRouters();
    // 指定初始策略,完成代价函数的初始化
    std::vector<int> currentPolicy = initialPolicy(routers);
    std::vector<int> lastPolicy = currentPolicy;
    // 迭代次数
    int k = 0;
    while (true)
    {
        // 记录上一次的策略
        lastPolicy = currentPolicy;
        // 迭代更新代价函数 Juk(x)
        calculateCostFunction(routers, currentPolicy);
        // 若 uk+1 = uk，则结束
        if (lastPolicy == currentPolicy)
        {
            break;
        }
        else
        {
            k++;
        }
    }
    // 输出最终决策合集以及每个路由器路由到外网的开销
    printf("最终决策合集：\n");
    for (int i = 0; i < currentPolicy.size(); i++)
    {
        printf("路由器 %d 的下一跳路由器为 %d,到外网的开销为 %d\n", i, currentPolicy[i], routers[i].costToInternet);
    }
    fclose(stdin);
    return 0;
}
