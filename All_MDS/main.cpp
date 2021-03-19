#include<iostream>
#include<set>
#include<vector>
#include<fstream>
#include <algorithm>
#include <iterator>
#include <cassert>

using namespace std;

struct Graph
{
	int numNodes; //网络总节点数
	vector<vector<int>> link_to_left; //；邻接表 link_to_left[a]表示从a出发能到达的邻居
	vector<vector<int>> link_to_right; //；邻接表 link_to_right[i]表示能到达i的所有邻居
	vector<int>mark;// 标记数组，标记节点匹配相关信息,值只会为0或者1
	vector<int>match; //match[u] = v，表示v-u匹配了

}Gra;  //


template<class T>
void print_1(T nums)
{
	cout << "***********************" << endl;
	for (const auto& x : nums)
	{
		cout << x << "\t";
	}
	cout << endl << "***********************" << endl;
}

template<class T>
void print_2(T nums)
{
	cout << "-------------------------" << endl;
	for (const auto& x : nums)
	{
		for (const auto t : x)
		{
			cout << t << "\t";
		}
		cout << endl;
	}
	cout << "-------------------------" << endl;
}

int GetMaxIdOfNetwork(string filename) //获取网络的最大节点编号
{
	ifstream fin(filename);
	int a, b;
	int maxID = -1;
	while (fin >> a >> b)
	{
		maxID = std::max(std::max(a, b), maxID);
	}
	fin.close();
	return maxID;
}

void read_edgelist(string filename) //获取网络的边信息
{
	int numEdges = 0;
	ifstream fin(filename);
	int start, end;
	while (fin >> start >> end)
	{
		Gra.link_to_left[start].push_back(end);
		Gra.link_to_right[end].push_back(start);
		++numEdges;
	}
	fin.close();
	cout << "number of nodes：" << Gra.numNodes << endl;
	cout << "number of links：" << numEdges << endl;
}

bool DFS(int u)   //从二分图左边的节点u出发寻找增广路
{
	for (auto i = 0; i < Gra.link_to_left[u].size(); ++i)  //节点u的所有能到达的邻居
	{
		int v = Gra.link_to_left[u][i]; //u的一个邻居v
		if (!Gra.mark[v])
		{
			Gra.mark[v] = true;
			if (Gra.match[v] == -1 || DFS(Gra.match[v]))
			{
				Gra.match[v] = u; //形成匹配u - v
				return true;
			}
		}
	}
	return false;
}



pair<set<int>, vector<int>>maxMatching() //最大匹配，返回驱动节点集合
{
	std::fill(Gra.match.begin(), Gra.match.end(), -1); //先默认让每个节点和-1匹配
	for (int i = 0; i < Gra.numNodes; ++i)
	{
		std::fill(Gra.mark.begin(), Gra.mark.end(), 0); //让每个节点都能被访问
		DFS(i);
	}
	set<int>right_match;
	for (int i = 0; i < Gra.numNodes; ++i)
	{
		if (Gra.match[i] != -1)  //Gra.match[i]  -- > i
		{
			right_match.insert(i);
		}
	}
	return make_pair(right_match, Gra.match);
}

//寻找二分图右边始终匹配的节点集合，这些节点肯定是存在于右边匹配节点集合中的
//且我们需要知道当前的匹配是怎样的，match_right[i]表示和二分图右边节点i匹配的节点（左边）
set<int> finding_always_matched_nodes_on_right(const set<int>& right_match, const vector<int>& match_right)
{
	set<int>always_match;
	for (const auto& u : right_match)
	{
		Gra.match = match_right; //将当前的匹配保存下来
		std::fill(Gra.mark.begin(), Gra.mark.end(), 0);//让所有节点都能被访问
		Gra.mark[u] = 1; //让u节点不能被访问,代替删除节点和边的操作

		if (DFS(match_right[u]) == false) //v-->u，从v出发找不到增广路
		{
			always_match.insert(u);
		}
	}
	return always_match;
}

////求差集，移除始终匹配的节点
//set<int> remove_always_matched_from_right_match_set(const set<int>&right_match,const set<int>&always_match)
//{
//	set<int>right_remain;
//	std::set_difference(right_match.begin(), right_match.end(), always_match.begin(), always_match.end(), inserter(right_remain, right_remain.begin()));
//	return right_remain;
//}

vector<set<int>> construct_Set_for_each_left_node(const set<int>& always_match)
{
	//先为左边节点建立集合
	vector<set<int>> S;
	set<int>temp; //二分图左边节点v对应的集合S，将它能达到的元素加入（不含始终匹配的节点）
	for (int i = 0; i < Gra.numNodes; ++i)
	{
		temp.clear();
		for (const auto& x : Gra.link_to_left[i])
		{
			if (!always_match.count(x)) //只加入always_match之外的节点
			{
				temp.insert(x);
			}
		}
		if (!temp.empty()) //只需要非空集合
		{
			S.push_back(temp);
		}
	}
	return S;
}


//下面得到的集合中还需要去重
void backTrace(set<set<int>>& res, const vector<set<int>>& S, vector<int>temp, const int num, int idx)
{
	if (idx == static_cast<int>(S.size())) //此次遍历结束
	{
		set<int>ret(temp.begin(), temp.end()); //去重
		if (static_cast<int>(ret.size()) == num)
		{
			res.insert(ret);
		}
		return;
	}
	for (const auto& x : S[idx])
	{
		temp.push_back(x);  //不可将temp定义为set,如果temp中已经存在x,再加入x，temp也只有一个x
		backTrace(res, S, temp, num, idx + 1);
		temp.pop_back();
	}
}

void write_All_MDS_to_file(string filename, const set<set<int>>& s, const set<int>& always_match)
{
	ofstream fout(filename);
	set<int>all_nodes;
	for (int i = 0; i < Gra.numNodes; ++i)
	{
		all_nodes.insert(i);
	}
	set<int>remain_set;
	set_difference(all_nodes.begin(), all_nodes.end(), always_match.begin(), always_match.end(), inserter(remain_set, remain_set.begin()));

	for (const auto& x : s)
	{
		set<int>MDS;
		set_difference(remain_set.begin(), remain_set.end(), x.begin(), x.end(), inserter(MDS, MDS.begin()));
		for (const auto &t : MDS)
		{
			fout << t << endl;
		}
		fout << endl;
	}
	fout.close();
	cout << s.size() << " MDSs have been writtend to file \"" << filename << "\" " << endl;
}

int main()
{
	string fin = "net.txt"; // 输入文件
	string fout = "res.txt";  //输出文件,将驱动节点集合写入该文件
	int GraphSize = GetMaxIdOfNetwork(fin) + 1; //认为： 最大编号 + 1 == 网络节点数
	Gra.numNodes = GraphSize;
	Gra.link_to_left.resize(GraphSize);
	Gra.link_to_right.resize(GraphSize);
	Gra.mark.resize(GraphSize);
	Gra.match.resize(GraphSize);

	read_edgelist(fin);

	//分别获取二分图右边匹配的，以及匹配数组（记录谁和谁匹配）
	auto match_set_arr = maxMatching();
	set<int>right_match_set = match_set_arr.first;
	vector<int>match_right = match_set_arr.second;

	set<int>always_match = finding_always_matched_nodes_on_right(right_match_set, match_right);
	//print_1(always_match);
	auto S = construct_Set_for_each_left_node(always_match);
	//print_2(S);
	
	int num = static_cast<int>(right_match_set.size()) - static_cast<int>(always_match.size());
	assert(num >= 0);
	if(num == 0)
	{
		cout << "there is only one MDS (the always matched set)!" << endl;
		
	}
	else
	{
		set<set<int>> res;
		vector<int>temp;
		backTrace(res, S, temp, num, 0);
		write_All_MDS_to_file(fout, res, always_match);
	}
	system("pause");
	return 0;
}