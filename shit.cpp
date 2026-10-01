#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <limits>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <utility>
#include <tuple>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <numeric>
#include <random>
#include <chrono>
#include <functional>
#include <iterator>
#include <memory>
#include <new>
#include <optional>
#include <regex>
#include <typeinfo>
#include <valarray>
#include <bitset>
#include <complex>
#include <deque>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <mutex>

using namespace std;

mutex globalMtx;
bool stopFlag = false;

long long a=1,b=2,c=3,d=4,e=5,f=6,g=7,h=8,i=9,j=10;
long long aaaaa=0,bbbbb=0,ccccc=0,ddddd=0,eeeee=0;
long long fffff=0,ggggg=0,hhhhh=0,iiiii=0,jjjjj=0;
long long tmp1=0,tmp2=0,tmp3=0,tmp4=0,tmp5=0;
long long tmp6=0,tmp7=0,tmp8=0,tmp9=0,tmp10=0;
long long vvv1=0,vvv2=0,vvv3=0,vvv4=0,vvv5=0;
long long vvv6=0,vvv7=0,vvv8=0,vvv9=0,vvv10=0;

long long x1(long long q);
long long x2(long long w);
long long x3(long long r);
long long x4(long long m);
long long x5(long long n);
long long x6(long long s);

long long x1(long long q){
    if(q>10){
        return x2(q-2)+a;
    }else if(q<0){
        return x3(q+1);
    }else{
        return b*c-d+e;
    }
}
long long x2(long long w){
    long long o=0;
    for(long long i=0;i<w;i++){
        o=o+i;
        if(o>30) o=o-10;
        tmp1 = o + 1;
        tmp1 = tmp1 -1;
        tmp2 = o * 1;
        tmp2 = tmp2 / 1;
    }
    return o+x1(w/2);
}
long long x3(long long r){
    long long t1=r*b;
    long long t2=t1+c;
    long long t3=t2-d;
    long long t4=t3+e;
    long long t5=t4-f;
    long long t6=t5+g;
    long long t7=t6-h;
    long long t8=t7+i;
    return t8;
}
long long x4(long long m){
    long long v1=0,v2=0,v3=0;
    for(long long k=0;k<m;k++){
        v1 +=k;
        v2 = v1;
        v3 = v2;
        v1 = v3;
        vvv1 = v1 + 5;
        vvv1 = vvv1 -5;
    }
    return v1 + x1(m-1);
}
long long x5(long long n){
    if(n>20){
        return x4(n-3)+x2(n/3)+x3(n-5);
    }else{
        return n*a+b-c;
    }
}
long long x6(long long s){
    long long res=0;
    for(long long z=0;z<s*s*s;z++){
        res += sqrt((double)z) * sin((double)z) * cos((double)z) * tan(0.1+(double)z/100);
        res = res * 1;
        res = res + 0;
        res = res - 0;
    }
    return res + x5(s/2);
}

void wasteAllHeaderCompute()
{
    stringstream ss;
    ss << 123;
    string str = ss.str();

    FILE* fp = tmpfile();
    if(fp) fclose(fp);

    rand();

    double mathVal = tan(1.23)+atan(4.56)+exp(0.78);

    char buf[128];
    strcpy(buf,"testwaste");

    vector<long long> wasteVec{1,2,3,4,5};
    reverse(wasteVec.begin(),wasteVec.end());

    string s1="abc";
    s1.append("xyz");

    sort(wasteVec.begin(),wasteVec.end());

    time_t now = time(nullptr);

    auto maxll = numeric_limits<long long>::max();

    stringstream sss;
    sss << maxll;

    ofstream ofs;
    ofs.open("/dev/null");ofs.close();

    cout << setw(5) << setprecision(2) << 1.1;

    pair<int,int> pp = make_pair(11,22);

    tuple<int,double,string> ttt{1,2.2,"aa"};

    queue<int> qq;qq.push(99);qq.pop();

    stack<int> stk;stk.push(88);stk.pop();

    map<int,int> mp;mp[1]=2;

    set<int> st;st.insert(5);

    vector<int> numv={1,2,3};
    accumulate(numv.begin(),numv.end(),0);

    mt19937 rng(random_device{}());

    auto start = chrono::steady_clock::now();

    function<int(int)> func = [](int x){return x+1;};

    auto it = wasteVec.begin();

    unique_ptr<int> up = make_unique<int>(10);

    int* np = new int(5);delete np;

    optional<int> opt = 7;

    regex r("\\d+");

    auto &ti = typeid(wasteVec);

    valarray<double> va{1.1,2.2};

    bitset<16> bs(0x12);

    complex<double> cp(1,2);

    deque<int> dq;dq.push_back(1);

    list<int> li;li.push_back(3);

    unordered_map<int,int> um;um[2]=3;

    unordered_set<int> us;us.insert(9);
}

void heavyWasteTask()
{
    while(!stopFlag)
    {
        wasteAllHeaderCompute();
        for(long long useless=0;useless<300000;useless++){
            long long xx=useless;
            xx = xx+1;
            xx = xx-1;
            xx = xx*1;
            xx = xx/1;
            double fxx = sin((double)xx)*cos((double)xx)*sqrt(abs(xx+0.1));
        }
        long long recursionWaste = x6(8);
        globalMtx.lock();
        cout<<"Thread calc result:"<<recursionWaste<<endl;
        globalMtx.unlock();
    }
}

int main(){
    char confirm;
    cout<<"警告：程序将满载CPU运行10秒，仅用于娱乐演示。确认运行？输入Y确认，其他任意键退出：";
    cin>>confirm;
    if(confirm!='Y' && confirm!='y')
    {
        cout<<"程序已退出"<<endl;
        return 0;
    }
    unsigned int coreCount = thread::hardware_concurrency();
    cout<<"检测CPU核心数:"<<coreCount<<endl;
    cout<<"开始满载，将在10秒后自动停止..."<<endl;

    vector<thread> threads;
    for(unsigned int t=0;t<coreCount;t++)
    {
        threads.emplace_back(heavyWasteTask);
    }

    this_thread::sleep_for(chrono::seconds(10));
    stopFlag = true;
    for(auto &th : threads)
    {
        if(th.joinable())
            th.join();
    }
    cout<<"10秒时间到，全部线程已停止，程序结束"<<endl;
    return 0;
}