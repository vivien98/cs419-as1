#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <string>

using namespace std;

int ct = 0;
int numNodes = 0;

///
float avg(vector<float> a, int start,int end){
	float sum = 0;
	for (int i = start; i < end; i++)
	{
		sum += a[i];	
	}
	sum /= (end - start);
	return sum;
}

float errorDueToSplit(int n, int i, vector<float> lab, float &lhs,float &rhs){
	 lhs = avg(lab,0,i);
	 rhs = avg(lab,i,n);
	 float sum = 0;
	 for (int j = 0; j < i; j++)
	 {
	 	sum += (lab[j] - lhs)*(lab[j] - lhs);
	 }
	 for (int j = i; j < n; j++)
	 {
	 	sum += (lab[j] - rhs)*(lab[j] - rhs);
	 }

	 return sum;

}
float bestSplitOnAtt(int n, vector<vector<float> > lab,int &split,float &lhs, float &rhs){
	float minErr = HUGE_VAL;
	vector<float> label;
	for (int i = 0; i < n; i++)
	{
		label.push_back(lab[i][2]);
	}
	for (int i = 0; i < n; i++)
	{	float tlhs,trhs;
		float err = errorDueToSplit(n,i,label,tlhs,trhs);
		if(err < minErr){
			minErr = err;
			split = i;
			lhs = tlhs;
			rhs = trhs;
		}
	}
 	return minErr;
}

void bestAtt(int numAttr,int numInstances, float &split,int &attrInd,float &merr, vector<vector<vector<float> > > data,vector<vector<vector<float> > > &rc,vector<vector<vector<float> > > &lc,float &rhs,float &lhs){
	
	int tsplit = 0;
	float tlhs,trhs;
	merr = HUGE_VAL;
	
	for (int i = 0; i < numAttr; i++)
	{
		float err;
		err = bestSplitOnAtt(numInstances,data[i],tsplit,tlhs,trhs);
		if(err < merr){
			merr = err;
			attrInd = i;
			split = data[i][tsplit][1];
			lhs = tlhs;
			rhs = trhs;
		}
	}
	map<float, bool> where;
	for (int i = 0; i < numInstances; i++)
	{
		if (data[attrInd][i][1] < split)
		{
			where[data[attrInd][i][0]] = true;
		}else{
			where[data[attrInd][i][0]] = false;
		}
	}
	for (int i = 0; i < numAttr; i++)
	{
		vector<vector<float> > ltemp,rtemp;
		for (int j = 0; j < numInstances; j++)
			{
				if(where[data[i][j][0]]){
					ltemp.push_back(data[i][j]);
					
				}else{
					rtemp.push_back(data[i][j]);
					
				}
			}
		lc.push_back(ltemp);
		rc.push_back(rtemp);	
	}
	return;
}
///
bool compare(vector<float> v1,vector<float> v2){
	return (v1[0] < v2[0]);
}
void sortAccToAttr(int ind, int numInstances, vector<vector<float> > &data){
	
	for (int i = 0; i < numInstances; i++)
	{
		int temp = data[i][ind];
		data[i][ind] = data[i][0];
		data[i][0] = temp; 
	}

	sort(data.begin(), data.end(),compare);
	for (int i = 0; i < numInstances; i++)
	{
		int temp = data[i][ind];
		data[i][ind] = data[i][0];
		data[i][0] = temp; 
	}
	return;
}
void sortData(int numInstances, int numAttr, vector<vector<float> > data, vector<vector<vector<float> > > &sorted){
	
	for (int i = 1; i < numAttr; i++)
	{	
		
		sortAccToAttr(i,numInstances, data);
		vector<vector<float> > ttemp;
		for (int j = 0; j < numInstances; j++)
		{
			vector<float> temp ;
			temp.push_back(data[j][0]);
			temp.push_back(data[j][i]);
			temp.push_back(data[j][numAttr]);
			ttemp.push_back(temp);
		}
		sorted.push_back(ttemp);
	}

	return;
}
///
void printTable(int numAttr,vector<vector<vector<float> > > lchild){
	for (int i = 0; i < numAttr; i++)
	{
		for (int j = 0; j < lchild[0].size(); j++)
		{
			
				cout<< lchild[i][j][0] << " " << lchild[i][j][1] << " " << lchild[i][j][2] << endl;
			
		}
		cout<<endl;
	}
	cout<<endl;

}
///

class Node{
public:
	float split = 0;
	int attInd = 0;
	float err;
	float ERR = -1;
	int numTestsPassed = 0;
	int numLeft = 0;
	int numRight=0;
	Node* lchild;
	Node* rchild;
	bool isLeaf = false;
	float av;
	void create(int numAttr,int numInstances, vector<vector<vector<float> > > data,int prevNum,float &avg);
	float decide(vector<float> instance, float expected, float &error);
	void assignErr(vector<vector<float> > valiData,vector<float> expected);
	void exploreAndCut();
	void prune(vector<vector<float> > valiData,vector<float> expected);

};
void Node::create(int numAttr,int numInstances, vector<vector<vector<float> > > data,int prevNum,float &avg){
	float lhs,rhs;
	numNodes++;
		if (numInstances==1)
		{
			av = data[0][0][numAttr];
			err = 0;
			lchild = NULL;
			rchild = NULL;
			isLeaf = true;
			//cout<<"leaf"<<endl;
			return;
		}
		if (prevNum == numInstances)
		{
			av = avg;
			err = 0;
			lchild = NULL;
			rchild = NULL;
			isLeaf = true;
			//cout<<"leaf"<<endl;
			return;
		}
		av = avg;
		vector<vector<vector<float> > > lc,rc;

		bestAtt(numAttr, numInstances, split, attInd, err, data, rc, lc, rhs,lhs);
		
		//cout<<ct<<" "<<split<<" "<<attInd<<" "<<lhs<<" "<<rhs<<" "<<err<<endl;
		ct++;
		lchild = new Node;
		rchild = new Node;
		lchild->create(numAttr,lc[0].size(),lc,numInstances,lhs);
		//cout<<"br"<<endl;
		rchild->create(numAttr,rc[0].size(),rc,numInstances,rhs);
		return;

	}
float Node::decide(vector<float> instance, float expected, float &error){
	ERR = (((expected - av)*(expected - av)/(expected*expected)) + numTestsPassed*ERR)/(numTestsPassed + 1);
	numTestsPassed++;
		if(isLeaf){
			//cout<<av<<" "<<err<<endl;
			error = (expected - av)*(expected - av)/(expected*expected);
			return av;
		}
		if(instance[attInd] < split){
			//cout<<av<<" "<<err<<endl;
			numLeft++;
			return lchild->decide(instance,expected,error);
		}
		//cout<<av<<" "<<err<<endl;
		numRight++;
		return rchild->decide(instance,expected,error);
	}
void Node::assignErr(vector<vector<float> > valiData,vector<float> expected){
	float e;
	for (int i = 0; i < valiData.size(); i++)
	{
		decide(valiData[i],expected[i],e);
	}

return;

}
void Node::exploreAndCut(){
	if(isLeaf || (numLeft==0 && numRight==0)){
		return;
	}
	lchild->exploreAndCut();
	rchild->exploreAndCut();

	if (ERR <= (numLeft/(numRight + numLeft))*(lchild->ERR) + (numRight/(numRight + numLeft))*(rchild->ERR) && ERR != -1)
	{
		lchild->~Node();
		rchild->~Node()	;
		lchild = NULL;
		rchild = NULL;
		isLeaf = true;
		numNodes -= 2;
		//cout<<numNodes<<endl;

	}
	return;
}
void Node::prune(vector<vector<float> > valiData,vector<float> expected){
	assignErr(valiData,expected);
	exploreAndCut();
	return;

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void readintovec(string line, vector<float> &data,int lineNum,int numAttr,bool train=true){
	
	if(train){
		data.push_back(lineNum);
	}
	
	for (int i = 0; i < numAttr; i++)
	{
		size_t pos = line.find(",");
		float val;
		val = stof(line.substr(0,pos));
		//if(!train)cout<<val<<endl;
		line = line.substr(pos + 1);
		data.push_back(val);
	}


	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int main()
{
	
	int numInstances = 3919;
	int numAttr = 12;
	int numTestCases = 1306;
	vector<vector<vector<float> > > dataout;
	vector<vector<float> > datain;
	vector<vector<float> > datatest;
	vector<float> expected;
	
	string line;
	ifstream myfile ("trainf.csv");
	if (myfile.is_open())
	{
	int k = 1;
	while ( getline (myfile,line) && k<=numInstances)
	{
	vector<float> l;
	readintovec(line, l, k,numAttr);
	datain.push_back(l);
	k++;
	}
	myfile.close();
	}
  	else cout << "Unable to open training file"; 

  	string line1;
	ifstream myfile1 ("test1.csv");
	if (myfile1.is_open())
	{
	int k1 = 1;
	while ( getline (myfile1,line1) && k1<=numTestCases)
	{
	vector<float> l1;
	readintovec(line1, l1, k1,numAttr,false);
	datatest.push_back(l1);
	k1++;
	}
	myfile1.close();
	}
  	else cout << "Unable to open test file"; 

	//cout<<"here";
  	for (int i = 0; i < datatest.size(); i++)
  	 {
  		//cout<<i<<endl;
  	 	expected.push_back(datatest[i][numAttr-1]);
  	 	datatest[i].pop_back();
  	 	
  	 }
cout<<"Sorting......"<<endl;
	sortData(numInstances,numAttr,datain,dataout);
	numAttr--;
	int attrInd;
	float merr,split;
	int prevNum = 0;
	float av = 0;
	Node* root = new Node;
cout<<"Training......"<<endl;
	root->create(numAttr,numInstances,dataout,prevNum,av);
	float totError = 0;
	cout<<numNodes<<endl;
cout<<"Testing before pruning......"<<endl;
	for (int i = 0; i < numTestCases; ++i)
	{
		float error;
		root->decide(datatest[i],expected[i],error);
		totError = (totError*(i)+error)/(i+1);

	}
	float accuracy = (1-sqrt(totError))*100;
	cout<<accuracy<<endl;
	//root->decide(in);
cout<<"Pruning......"<<endl;
	root->prune(datatest,expected);
	cout<<numNodes<<endl;
	totError = 0;
cout<<"Testing after pruning......"<<endl;
	for (int i = 0; i < numTestCases; ++i)
	{
		float error;
		root->decide(datatest[i],expected[i],error);
		totError = (totError*(i)+error)/(i+1);

	}
	accuracy = (1-sqrt(totError))*100;
	cout<<accuracy<<endl;
	cout<<"Over"<<endl;
	return 0;
}



/*
1.0, -5.0, 0.3 
2.0, 5.0, 0.3 
3.0, -2.0, 0.5 
1.0, 2.0, 0.5 
2.0, 0.0, 0.7 
6.0, -5.0, 0.5 
7.0, 5.0, 0.5 
6.0, -2.0, 0.3 
7.0, 2.0, 0.3 
6.0, 0.0, 0.7 
8.0, -5.0, 0.5 
9.0, 5.0, 0.5 
10.0, -2.0, 0.3 
8.0, 2.0, 0.3 
9.0, 0.0, 0.5 




*/