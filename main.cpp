#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <stack>
using namespace std;
typedef long long LL;
const int INF=0x7fffffff;
const int MAX_N=10000;

int num_of_a,num_of_A,num_of_S,totalnum;
//小写字母个数、大写字母个数、推导式个数、最终结点总数
char a[50];//字母
char S[50][50];//推导式
bool pushed[100];//访问一个结点的时候，再往外拓展时记录非终结符是否已经被push过
int ans[100][100];
char analyse[50];//分析串
int stack1[50];
char stack2[50];
char stack3[50];
int L1,L2,L3;//三个栈的长度
map<char,int>M;
map<char,int>::iterator m;

struct node{//结点
    int id;//编号
    int num;//包含的推导式个数
    char s[20][50];
};
queue<node>q;//结点型队列
node N[100];
int numN=0;

void outputnd(node k){//输出一个结点的s
    for(int i=0;i<k.num;i++){
        cout<<k.s[i]<<endl;
    }
}

void outputlb(){//输出一个数组的s
    cout<<"链表"<<endl;
    for(int i=0;i<numN;i++){
        outputnd(N[i]);
        cout<<endl;
    }
}

bool samenode(node x,node y){//比较两个node是否一样
     if(x.num!=y.num)return 0;//如果两个node包含的s数量不同直接输出不同
     for(int i=0;i<x.num;i++){//暴力比较
        int flag=0;
        for(int j=0;j<y.num;j++){
            if(strcmp(x.s[i],y.s[j])==0){
                flag=1;
                break;
            }
        }
        if(flag==0)return 0;
     }
     return 1;
}

void init(){//初始化
    totalnum=0;//结点数量
    while(!q.empty())q.pop();//情况队列
    memset(S,0,sizeof(S));
    for(int i=0;i<100;i++){
        for(int j=0;j<100;j++){
            ans[i][j]=-10000;
        }
    }
    M.clear();//清空映射
}

void input(){//顾名思义，输入数据
    printf("请输入终结符的个数：");
    scanf("%d",&num_of_a);
    printf("请输入%d个终结符（小写字母）：",num_of_a);
    for(int i=0;i<num_of_a;i++){
        scanf(" %c",&a[i]);
        M.insert(make_pair(a[i],i));//建立映射
    }
    a[num_of_a]='#';
    M.insert(make_pair(a[num_of_a],num_of_a));
    printf("请输入非终结符的个数（不包含起始符）：");
    scanf("%d",&num_of_A);
    printf("请输入%d个非终结符（大写字母）：",num_of_A);
    for(int i=0;i<num_of_A;i++){
        scanf(" %c",&a[num_of_a+1+i]);
        M.insert(make_pair(a[num_of_a+1+i],num_of_a+1+i));
    }
    printf("请输入初始推导式的个数：");
    scanf("%d",&num_of_S);
    printf("下面%d行每行请输入1个推导式：前后用下划线“_”链接（起始为S）:\n",num_of_S);
    for(int i=0;i<num_of_S;i++){
        scanf("%s",S[i]);
        for(int j=strlen(S[i]);j>=3;j--){
            S[i][j]=S[i][j-1];
        }
        S[i][2]='.';
    }
}

node pushback(char p,node f){//node f里把所有p开头的推导式push进去
    for(int i=0;i<num_of_S;i++){
        if(S[i][0]==p){
            strcpy(f.s[f.num++],S[i]);
        }
    }
    return f;
}

void bfs(){//宽度优先搜索
    node aa;//声明一个空的状态
    aa.num=0;
    aa.id=totalnum++;//初始化num和id
    aa=pushback('S',aa);//把S开头的推导式放到这个起始结点里
    memset(pushed,0,sizeof(pushed));//用于记录哪些字母被一次性push过，避免重复push
    for(int j=0;j<aa.num;j++){//把需要push的大写字母开头的推导式push进去
        if(aa.s[j][3]>='A'&&aa.s[j][3]<='Z'&&pushed[aa.s[j][3]]==0){
            aa=pushback(aa.s[j][3],aa);
            pushed[aa.s[j][3]]=1;
        }
    }
    q.push(aa);//把起始点放入队列
    N[numN++]=aa;//这个东西记录已经存在过的node
    while(!q.empty()){//只要队列不为空，就一直运行，下面就是宽搜了
        node cur=q.front();
        q.pop();//取出头结点保存然后pop掉
        for(int i=0;i<num_of_a+num_of_A+1;i++){//搜索所有字母
            if(i==num_of_a)continue;//这个位置存储#，所以为了提高程序运行效率，直接过掉
            node newnode;//声明一个可能要添加的新结点
            newnode.num=0;
            newnode.id=-1;
            memset(pushed,0,sizeof(pushed));
            for(int j=0;j<cur.num;j++){//搜索当前刚取出的队列头结点的所有推导式
                for(int k=2;k<strlen(cur.s[j]);k++){//对于每个推导式从左到右爆搜
                    if(cur.s[j][strlen(cur.s[j])-1]=='.')continue;//如果最后一个位置是“.”直接continue
                    if(cur.s[j][k]=='.'&&cur.s[j][k+1]==a[i]){//如果“.”后面正好是当前要搜索的字母
                        if(newnode.id==-1)newnode.id=-2;//这个node可能要被添加了，一个字母加一遍就好，以前没加过就是-1
                        char news[50];//声明一个新的字符串
                        strcpy(news,cur.s[j]);//保留现场，先把找到的这条copy下来
                        swap(news[k],news[k+1]);//把“.”和它后面的字母互换位置
                        strcpy(newnode.s[newnode.num++],news);//把处理好的news粘贴到newnode里面
                        if(news[k+2]>='A'&&news[k+2]<='Z'&&pushed[news[k+2]]==0){//如果发现“.”后面的后面跟着大写字母还要麻烦一下
                            newnode=pushback(news[k+2],newnode);//把所有的以这个大写字母开头的推导式push到newnode里面
                            pushed[news[k+2]]=1;//之前没注释的memset(pushed,0,sizeof(pushed));此时派上了用场
                        }
                    }
                }
            }
            if(newnode.id!=-1){//如果找到了可能需要push的结点
                int flag=-1;//我还需要看看这个结点是否已经存在过，是不是形成了环
                for(int p=0;p<numN;p++){//爆搜一下曾经的结点
                    if(samenode(newnode,N[p])==1){//比较一下是不是一样
                        flag=p;//如果找到一样的，就break，并且保留那个一样的结点的位置
                        break;
                    }
                }
                if(flag!=-1){//发现了一样的结点
                    ans[cur.id][i]=N[flag].id;//最终结果里体现一下
                }
                if(flag==-1){//没发现有一样的，那就是新的结点了，把他push到队列里面去
                    newnode.id=totalnum++;
                    q.push(newnode);
                    N[numN++]=newnode;
                    ans[cur.id][i]=newnode.id;
                }
            }
        }
    }
}

void build_table(){//处理表格
    for(int i=0;i<totalnum;i++){
        int flag=0;
        for(int j=0;j<num_of_A+num_of_a+1;j++){
            if(ans[i][j]!=-10000){
                flag=1;
                break;
            }
        }
        if(flag==0){
            char news[50];
            strcpy(news,N[i].s[0]);
            news[strlen(news)+1]='\0';
            for(int k=strlen(news)-1;k>=3;k--){
                news[k]=news[k-1];
            }
            news[2]='.';
            for(int j=0;j<num_of_S;j++){
                if(!strcmp(news,S[j])){
                    if(j!=0){
                        for(int k=0;k<num_of_a+1;k++){
                            ans[i][k]=-1*j;
                        }
                        break;
                    }
                    for(int k=0;k<num_of_a+1;k++){//r0 用-9999表示，s0用0表示
                        ans[i][k]=-9999;
                    }
                    break;
                }
            }
        }
    }
}

void output_action(int act){
    if(act>=0)cout<<"S"<<setw(3)<<act;
    else if(act==-10000){
        cout<<setw(4)<<" ";
    }
    else if(act==-9999){
        cout<<setw(4)<<"r0";
    }
    else{
        cout<<"r"<<setw(3)<<-1*act;
    }
}

void output_table(){//输出表格
    cout<<left<<"项目  ";
    for(int i=0;i<num_of_A+num_of_a+1;i++){
        cout<<setw(4)<<a[i];
    }cout<<endl;
    for(int i=0;i<totalnum;i++){
        cout<<setw(6)<<i;
        for(int j=0;j<num_of_A+num_of_a+1;j++){
            output_action(ans[i][j]);
        }cout<<endl;
    }
}

int step,action,GOTO;

void output_stacks(){
    cout<<endl<<"步骤："<<step++<<endl;
    cout<<"状态栈/符号栈/输入串：";
    for(int j=0;j<L1;j++){
        cout<<stack1[j]<<" ";
    }cout<<"     ";
    for(int j=0;j<L2;j++){
        cout<<stack2[j];
    }cout<<"     ";
    for(int j=L3-1;j>=0;j--){
        cout<<stack3[j];
    }cout<<endl;
}

int judge(){
    m=M.find(stack3[L3-1]);//用映射的方式找到要分析的字母是第几个,节省时间提升程序运行效率
    if(m==M.end()){
        return 0;
    }
    action=ans[stack1[L1-1]][m->second];
    if(action==-10000)return 0;
    if(action>=0){
        GOTO=-10000;
        return 1;
    }
    if(action<0&&action>-9999){
        return 2;
    }
    if(action==-9999){
        return 3;
    }
    return 0;
}

void solve(){
    cout<<endl<<"分析字符串"<<endl;
    char c;
    while(1){
        cout<<endl<<"输入一行字符串并且以#结尾:"<<endl;
        memset(analyse,0,sizeof(analyse));
        int i=0;
        while(scanf(" %c",&c)&&c!='#'){
            analyse[i++]=c;
        }
        step=1;
        memset(stack1,0,sizeof(stack1));
        memset(stack2,0,sizeof(stack2));
        memset(stack3,0,sizeof(stack3));
        L1=0,L2=0,L3=0;
        stack1[L1++]=0;
        stack2[L2++]='#';
        stack3[L3++]='#';
        for(int j=strlen(analyse)-1;j>=0;j--){
            stack3[L3++]=analyse[j];
        }
        while(1){
            int oprt=judge();
            if(oprt==0){
                cout<<"error!"<<endl;
                break;
            }
            if(oprt==1){
                output_stacks();
                cout<<"ACTION:";
                output_action(action);
                cout<<"     GOTO:blank"<<endl;
                stack1[L1++]=action;
                stack2[L2++]=stack3[--L3];
            }
            if(oprt==2){
                output_stacks();
                cout<<"ACTION:";
                output_action(action);
                int ct=0;
                for(int j=strlen(S[action*-1])-1;j>=0;j--){
                    if(S[action*-1][j]!='.')ct++;
                    if(S[action*-1][j]=='.')break;
                }
                L1-=ct;
                L2-=ct;
                stack2[L2++]=S[-1*action][0];
                m=M.find(stack2[L2-1]);
                GOTO=ans[stack1[L1-1]][m->second];
                stack1[L1++]=GOTO;
                cout<<"     GOTO:"<<GOTO<<endl;
            }
            if(oprt==3){
                output_stacks();
                if(L3==1){
                    cout<<"ACTION:acc!"<<endl;
                }
                else{
                    cout<<"error!"<<endl;
                }
                break;
            }
        }
    }
}

int main(){
    init();//初始化
    input();//输入
    bfs();//核心程序宽度优先搜索生成表格
    build_table();
    output_table();//输出表格
    solve();
    return 0;
}

//测试数据
/*

4
abcd
3
EAB
7
S_E
E_aA
E_bB
A_cA
A_d
B_cB
B_d
bccd#

4
abcd
3
EAB
7
S_E
E_aB
E_bA
B_bB
A_aA
B_d
A_c


5
abcde
3
EAB
8
S_E
E_aB
E_bB
B_cA
B_cB
A_dB
A_d
B_e

*/
