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
	int numNodes; //�����ܽڵ���
	vector<vector<int>> link_to_left; //���ڽӱ� link_to_left[a]��ʾ��a�����ܵ�����ھ�
	vector<vector<int>> link_to_right; //���ڽӱ� link_to_right[i]��ʾ�ܵ���i�������ھ�
	vector<int>mark;// ������飬��ǽڵ�ƥ�������Ϣ,ֵֻ��Ϊ0����1
	vector<int>match; //match[u] = v����ʾv-uƥ����

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

int GetMaxIdOfNetwork(string filename) //��ȡ��������ڵ���
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

void read_edgelist(string filename) //��ȡ����ı���Ϣ
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
	cout << "number of nodes��" << Gra.numNodes << endl;
	cout << "number of links��" << numEdges << endl;
}

bool DFS(int u)   //�Ӷ���ͼ��ߵĽڵ�u����Ѱ������·
{
	for (auto i = 0; i < Gra.link_to_left[u].size(); ++i)  //�ڵ�u�������ܵ�����ھ�
	{
		int v = Gra.link_to_left[u][i]; //u��һ���ھ�v
		if (!Gra.mark[v])
		{
			Gra.mark[v] = true;
			if (Gra.match[v] == -1 || DFS(Gra.match[v]))
			{
				Gra.match[v] = u; //�γ�ƥ��u - v
				return true;
			}
		}
	}
	return false;
}



pair<set<int>, vector<int>>maxMatching() //���ƥ�䣬���������ڵ㼯��
{
	std::fill(Gra.match.begin(), Gra.match.end(), -1); //��Ĭ����ÿ���ڵ��-1ƥ��
	for (int i = 0; i < Gra.numNodes; ++i)
	{
		std::fill(Gra.mark.begin(), Gra.mark.end(), 0); //��ÿ���ڵ㶼�ܱ�����
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

//Ѱ�Ҷ���ͼ�ұ�ʼ��ƥ��Ľڵ㼯�ϣ���Щ�ڵ�϶��Ǵ������ұ�ƥ��ڵ㼯���е�
//��������Ҫ֪����ǰ��ƥ���������ģ�match_right[i]��ʾ�Ͷ���ͼ�ұ߽ڵ�iƥ��Ľڵ㣨��ߣ�
set<int> finding_always_matched_nodes_on_right(const set<int>& right_match, const vector<int>& match_right)
{
	set<int>always_match;
	for (const auto& u : right_match)
	{
		Gra.match = match_right; //����ǰ��ƥ�䱣������
		std::fill(Gra.mark.begin(), Gra.mark.end(), 0);//�����нڵ㶼�ܱ�����
		Gra.mark[u] = 1; //��u�ڵ㲻�ܱ�����,����ɾ���ڵ�ͱߵĲ���

		if (DFS(match_right[u]) == false) //v-->u����v�����Ҳ�������·
		{
			always_match.insert(u);
		}
	}
	return always_match;
}

////�����Ƴ�ʼ��ƥ��Ľڵ�
//set<int> remove_always_matched_from_right_match_set(const set<int>&right_match,const set<int>&always_match)
//{
//	set<int>right_remain;
//	std::set_difference(right_match.begin(), right_match.end(), always_match.begin(), always_match.end(), inserter(right_remain, right_remain.begin()));
//	return right_remain;
//}

vector<set<int>> construct_Set_for_each_left_node(const set<int>& always_match)
{
	//��Ϊ��߽ڵ㽨������
	vector<set<int>> S;
	set<int>temp; //����ͼ��߽ڵ�v��Ӧ�ļ���S�������ܴﵽ��Ԫ�ؼ��루����ʼ��ƥ��Ľڵ㣩
	for (int i = 0; i < Gra.numNodes; ++i)
	{
		temp.clear();
		for (const auto& x : Gra.link_to_left[i])
		{
			if (!always_match.count(x)) //ֻ����always_match֮��Ľڵ�
			{
				temp.insert(x);
			}
		}
		if (!temp.empty()) //ֻ��Ҫ�ǿռ���
		{
			S.push_back(temp);
		}
	}
	return S;
}


//����õ��ļ����л���Ҫȥ��
void backTrace(set<set<int>>& res, const vector<set<int>>& S, vector<int>temp, const int num, int idx)
{
	if (idx == static_cast<int>(S.size())) //�˴α�������
	{
		set<int>ret(temp.begin(), temp.end()); //ȥ��
		if (static_cast<int>(ret.size()) == num)
		{
			res.insert(ret);
		}
		return;
	}
	for (const auto& x : S[idx])
	{
		temp.push_back(x);  //���ɽ�temp����Ϊset,���temp���Ѿ�����x,�ټ���x��tempҲֻ��һ��x
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
	string fin = "net.txt"; // �����ļ�
	string fout = "res.txt";  //����ļ�,�������ڵ㼯��д����ļ�
	int GraphSize = GetMaxIdOfNetwork(fin) + 1; //��Ϊ�� ����� + 1 == ����ڵ���
	Gra.numNodes = GraphSize;
	Gra.link_to_left.resize(GraphSize);
	Gra.link_to_right.resize(GraphSize);
	Gra.mark.resize(GraphSize);
	Gra.match.resize(GraphSize);

	read_edgelist(fin);

	//�ֱ��ȡ����ͼ�ұ�ƥ��ģ��Լ�ƥ�����飨��¼˭��˭ƥ�䣩
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