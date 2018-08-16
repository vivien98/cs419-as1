#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <string>

using namespace std;

int ct = 0;
int numNodes = 0;
int numLeaves = 0;

///
double avg(vector<double> a, int start,int end){
	double sum = 0;
	for (int i = start; i < end; i++)
	{
		sum += a[i];	
	}
	sum = sum/(end - start);
	return sum;
}

double errorDueToSplit(int n, int i, vector<double> lab, double &lhs,double &rhs){
	 lhs = avg(lab,0,i);
	 rhs = avg(lab,i,n);
	 double sum = 0;
	 for (int j = 0; j < i; j++)
	 {
	 	sum += (lab[j] - lhs)*(lab[j] - lhs);
	 }
	 for (int j = i; j < n; j++)
	 {
	 	sum += (lab[j] - rhs)*(lab[j] - rhs);
	 }
	 //sum /= n;
	 return sum;

}
double bestSplitOnAtt(int n, vector<vector<double> > lab,int &split,double &lhs, double &rhs){
	double minErr = HUGE_VAL;
	vector<double> label;
	for (int i = 0; i < n; i++)
	{
		label.push_back(lab[i][2]);
	}
	for (int i = 0; i < n; i++)
	{	double tlhs,trhs;
		double err = errorDueToSplit(n,i,label,tlhs,trhs);
		if(err <= minErr){
			minErr = err;
			split = i;
			lhs = tlhs;
			rhs = trhs;
		}
	}
 	return minErr;
}

void bestAtt(int numAttr,int numInstances, double &split,int &attrInd,double &merr, vector<vector<vector<double> > > data,vector<vector<vector<double> > > &rc,vector<vector<vector<double> > > &lc,double &rhs,double &lhs){
	
	int tsplit = 0;
	double tlhs,trhs;
	merr = HUGE_VAL;
	
	for (int i = 0; i < numAttr; i++)
	{
		double err;
		err = bestSplitOnAtt(numInstances,data[i],tsplit,tlhs,trhs);
		if(err <= merr){
			merr = err;
			attrInd = i;
			split = data[i][tsplit][1];
			lhs = tlhs;
			rhs = trhs;
		}
	}
	map<double, bool> where;
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
		vector<vector<double> > ltemp,rtemp;
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
bool compare(vector<double> v1,vector<double> v2){
	return (v1[0] < v2[0]);
}
void sortAccToAttr(int ind, int numInstances, vector<vector<double> > &data){
	
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
void sortData(int numInstances, int numAttr, vector<vector<double> > data, vector<vector<vector<double> > > &sorted){
	
	for (int i = 1; i < numAttr; i++)
	{	
		
		sortAccToAttr(i,numInstances, data);
		vector<vector<double> > ttemp;
		for (int j = 0; j < numInstances; j++)
		{
			vector<double> temp ;
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
void printTable(int numAttr,vector<vector<vector<double> > > lchild){
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
	double split = 0;
	int attInd = 0;
	double err;
	double ERR = 0;
	int numTestsPassed = 0;
	int numLeft = 0;
	int numRight=0;
	Node* lchild;
	Node* rchild;
	bool isLeaf = false;
	bool passed = false;
	double av;
	void create(int numAttr,int numInstances, vector<vector<vector<double> > > data,int prevNum,int maxLeafNum);
	double decide(vector<double> instance);
	double decide(vector<double> instance, double expected, double &error);
	void assignErr(vector<vector<double> > valiData,vector<double> expected);
	void exploreAndCut();
	void prune(vector<vector<double> > valiData,vector<double> expected);

};
void Node::create(int numAttr,int numInstances, vector<vector<vector<double> > > data,int prevNum,int maxLeafNum){
	double lhs,rhs;
	numNodes++;
	for (int i = 0; i < numInstances; i++)
	{
		av += data[0][i][2];
	}
	av = av/numInstances;
		if (numInstances <= maxLeafNum)
		{
			//av = avg;
			err = 0;
			lchild = NULL;
			rchild = NULL;
			isLeaf = true;
			numLeaves++;
			return;
		}
		if (numInstances==1)
		{
			//av = data[0][0][numAttr];
			err = 0;
			lchild = NULL;
			rchild = NULL;
			isLeaf = true;
			numLeaves++;
			//cout<<"leaf"<<endl;
			return;
		}
		if (prevNum == numInstances)
		{
			//av = avg;
			err = 0;
			lchild = NULL;
			rchild = NULL;
			isLeaf = true;
			numLeaves++;
			//cout<<"leaf"<<endl;
			return;
		}
		//av = avg;
		vector<vector<vector<double> > > lc,rc;

		bestAtt(numAttr, numInstances, split, attInd, err, data, rc, lc, rhs,lhs);
		
		//cout<<ct<<" "<<split<<" "<<attInd<<" "<<lhs<<" "<<rhs<<" "<<err<<endl;
		ct++;
		lchild = new Node;
		rchild = new Node;
		lchild->create(numAttr,lc[0].size(),lc,numInstances,maxLeafNum);
		//cout<<"br"<<endl;
		rchild->create(numAttr,rc[0].size(),rc,numInstances,maxLeafNum);
		return;

	}
double Node::decide(vector<double> instance){
		if(isLeaf){
			return av;
		}
		if(instance[attInd] < split){
			return lchild->decide(instance);
		}
			return rchild->decide(instance);
	}
double Node::decide(vector<double> instance, double expected, double &error){
	ERR = (((expected - av)*(expected - av)) + numTestsPassed*ERR)/(numTestsPassed + 1);
	passed = true;
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
void Node::assignErr(vector<vector<double> > valiData,vector<double> expected){
	double e;
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

	if (ERR*(numTestsPassed) <= numLeft*(lchild->ERR) + numRight*(rchild->ERR) && passed && ((lchild->isLeaf)&&(rchild->isLeaf)))
	{
		lchild->~Node();
		rchild->~Node()	;
		lchild = NULL;
		rchild = NULL;
		isLeaf = true;
		numNodes -= 2;
		numLeaves--;
		//cout<<numNodes<<endl;

	}
	return;
}
void Node::prune(vector<vector<double> > valiData,vector<double> expected){
	assignErr(valiData,expected);
	exploreAndCut();
	return;

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void readintovec(string line, vector<double> &data,int lineNum,int numAttr,bool train=true){
	
	if(train){
		data.push_back(lineNum);
	}
	
	for (int i = 0; i < numAttr; i++)
	{
		size_t pos = line.find(",");
		double val;
		val = stod(line.substr(0,pos));

		//if(!train)cout<<val<<endl;
		line = line.substr(pos + 1);
		data.push_back(val);
	}


	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int main()
{
	
	const int numInstances = 576;
	const int numPrune = 192;//192 
	int prunestart = 0;//0
	int numAttr = 9;
	const int numTestCases = 192;
	const int maxLeafNum = 30;//30
	int numTrees = 0;
	// vector<vector<vector<double> > > dataout;
	// vector<vector<double> > datain;
	// vector<vector<double> > dataprune;
	// vector<vector<double> > datatest;
	// vector<double> expected,expectedP;
	// vector<double> dataresult(numTestCases,0.0);
	vector<double> dataresult(numTestCases,0.0);
	
while(prunestart < 40){//40
	vector<vector<vector<double> > > dataout;
	vector<vector<double> > datain;
	vector<vector<double> > dataprune;
	vector<vector<double> > datatest;
	vector<double> expected,expectedP;
	
	numAttr = 9;
	numNodes = numLeaves = 0;
	string line;
	ifstream myfile ("train.csv");
	if (myfile.is_open())
	{
	int k = 1;
	string dump;
	getline(myfile,dump);
	while ( getline (myfile,line) && k<=numInstances)
	{
	vector<double> l;
	readintovec(line, l, k,numAttr);
	if (k <= (prunestart + numPrune) && k > prunestart)
	{
		dataprune.push_back(l);
	}
	else{
		datain.push_back(l);
	}
	k++;
	}
	myfile.close();
	}
  	else cout << "Unable to open training file"; 

  	string line1;
	ifstream myfile1 ("test.csv");
	if (myfile1.is_open())
	{
	int k1 = 1;
	string dump;
	getline(myfile1,dump);
	while ( getline (myfile1,line1) && k1<=numTestCases)
	{
	vector<double> l1;
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
  	 for (int i = 0; i < dataprune.size(); i++)
  	 {
  		//cout<<i<<endl;
  	 	expectedP.push_back(dataprune[i][numAttr-1]);
  	 	dataprune[i].pop_back();
  	 	
  	 }
cout<<"Sorting......"<<endl;
	sortData(numInstances-numPrune,numAttr,datain,dataout);
	numAttr--;
	int attrInd;
	double merr,split;
	int prevNum = 0;
	double av = 0;
	Node* root = new Node;
cout<<"Training......"<<endl;
	root->create(numAttr,numInstances-numPrune,dataout,prevNum,maxLeafNum);
	double totError = 0;
	cout<<numNodes<<" "<<root->isLeaf<<endl;
// cout<<"Testing before pruning......"<<endl;
// 	for (int i = 0; i < numTestCases; ++i)
// 	{
// 		double error;
// 		root->decide(datatest[i],expected[i],error);
// 		totError = (totError*(i)+error)/(i+1);

// 	}
// 	double accuracy = (1-sqrt(totError))*100;
// 	cout<<accuracy<<endl;
	//root->decide(in);
cout<<"Pruning......"<<endl;
	root->prune(dataprune,expectedP);
	cout<<numNodes<<endl;
	totError = 0;
// cout<<"Testing after pruning......"<<endl;
// 	for (int i = 0; i < numTestCases; ++i)
// 	{
// 		double error;
// 		root->decide(datatest[i],expected[i],error);
// 		totError = (totError*(i)+error)/(i+1);

// 	}
// 	accuracy = (1-sqrt(totError))*100;
// 	cout<<accuracy<<","<<maxLeafNum<<endl;
	cout<<"Testing on unseen file....."<<endl;
	for (int i = 0; i < numTestCases; i++)
	{
		//double result;
		if(numNodes>22)
		dataresult[i] += root->decide(datatest[i]);
	
		    //mfile << i+1<<","<<result <<endl;
		    
		

	}
	root->~Node();
prunestart += 5 ;
if(numNodes>22)numTrees++;
cout<<numTrees<<endl;
}


	ofstream mfile ("output.csv");
  	mfile<<"Id,output"<<endl;
	for (int i = 0; i < numTestCases; ++i)
	{
		//double result;
		//result = root->decide(datatest[i]);
		if (mfile.is_open())
		  {
		  	//string s = result.str();
		  	dataresult[i] = dataresult[i]/numTrees;
		    mfile << i+1<<","<<dataresult[i] <<endl;
		    
		    
		  }
		  else cout << "Unable to open write file";
		

	}
	mfile.close();
	cout<<"Over";//root->isLeaf<<endl;

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