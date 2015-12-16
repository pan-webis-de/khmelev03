#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include <unistd.h>

#include "Jzon.h"
#include "Jzon.cpp"

using namespace std;

vector<char> empty; //empty vector for pushback
char letter; 

#include "utext.cpp"
#include "author.cpp"
#include "corpus.cpp"
	
//Other Variables:

int mintraininglength=50000000;

struct answerList
{
	string unknown_text;
	string author;
	float score;
	answerList* nextAnswer;
};

struct candidateAuthors
{
	string author_name;
	candidateAuthors* next;
};

struct unknownTexts
{
	string unknown_text;
	unknownTexts* next;
};

struct inputMetadata
{
	string unknown_folder;
	string language;
	string encoding;

	candidateAuthors* candidates;
	unknownTexts* unknown;
};

inputMetadata* readMetadata(string path);
void processAnswers(answerList* llist, string output_path);

int main(int argn, char* argv[]){	

	//Strings:
	string input_path;
	string output_path;
	
	for(int i=0;i<argn;i++){
		string str(argv[i]);
		cout << str << endl;
		if(str.compare("-i")==0){
			string str2(argv[i+1]);
			input_path=str2;
		}	
		if(str.compare("-o")==0){
			string str2(argv[i+1]);
			output_path=str2;
		}
		
	}
	
	cout << "Input path succesfully delivered: " << input_path << endl;
	cout << "Output path succesfully delivered: " << output_path << endl;
	
	//Open corpus:
	corpus co(input_path);
	
	int numberofauthors=0;
	int numberofunknowntexts=0;
	
	//Reading the names from the metafile:
	
	//Open the file:
	string metafile = co.getname() + "meta-file.json";
	
	inputMetadata* result = readMetadata(metafile);
	
	while(result->candidates)
	{
		numberofauthors++;
		co.makeauthor(result->candidates->author_name);
		result->candidates = result->candidates->next;
	}
	
	co.nofauthors=numberofauthors;

	//Streams for input:
	ifstream input;

	//Reading the training files for all the authors:
	for(int au=0;au<numberofauthors;au++){
		for(int tn=1;tn<999;tn++){
			string filename;
			string tnstring;
			stringstream tnstream;
			tnstream << tn;
			tnstring=tnstream.str();
			if(tn<10){ //Add zeros in respect to the length of the number:
				filename="known0000" + tnstring;
			}
			else{
				if(tn<100){
					filename="known000" + tnstring;
				}
				else{
					filename="known00" + tnstring;
				}
			}
			string filedir=co.getname()+co.getauthorname(au)+"/";
			string file=filedir+filename+".txt";
			
			const char* filechar;
			filechar=file.c_str();
			input.close();
			input.open(filechar,ifstream::in);
			if(input.good()==false){
				cout << "Author " << co.getauthorname(au) << " succesfully initialized. " << tn-1 << " documents identified." << endl; 
				break;
			}
			cout << "File: " << file << " succesfully read." << endl;
			co.allocauthors[au].training.push_back(empty);
				letter=1;
				while(letter!=-1){
					letter=input.get();
					if (letter!=-1){
						co.allocauthors[au].training[tn-1].push_back(letter);
					}
				}
		}
	}
	
	//Minimal length of the unknown texts: (later I will "cut" them all to this length)
	
	for(int au=0; au<numberofauthors; au++){
		int acttraininglenght=0;
		for(int doc=0; doc<co.allocauthors[au].training.size();doc++){
			acttraininglenght+=co.allocauthors[au].training[doc].size();
		}
		if (acttraininglenght<mintraininglength){
			mintraininglength=acttraininglenght;
		}
	}
	
	cout << "Minimal lenght of the texts: " << mintraininglength << endl;
	
	cout << endl;
	
	//Reading the unknown texts:
	
	//Getting names from the metafile:
	
	while(result->unknown)
	{
		numberofunknowntexts++;
		co.makeutext(result->unknown->unknown_text);
		result->unknown = result->unknown->next;
	}
	
	co.nofunknown=numberofunknowntexts;
	
	cout << endl;
	
	
	for(int ut=0;ut<numberofunknowntexts;ut++){
		string file=co.getname() + "unknown/"  + co.getutextname(ut);
		const char* filechar;
		filechar=file.c_str();
		input.close();
		input.open(filechar,ifstream::in);
		cout << "File: " << file << " succesfully read." << endl;
				letter=1;
				while(letter!=-1){
					letter=input.get();
					if (letter!=-1){
						co.allocutexts[ut].memory.push_back(letter);
					}
				}
	}
	
	cout << endl;
	
	//Memory for the different r-values:
	double rmem [numberofauthors];
	double smem [numberofauthors];	
	
	answerList* llist = 0;
	answerList* flist = 0;
	
	//Calculating the different authors:
	
	for(int ut=0; ut<numberofunknowntexts; ut++){
		//Delete memory:
		for(int i=0; i<numberofauthors; i++){
			smem[i]=0;
			rmem[i]=0;
		}
		for(int au=0; au<numberofauthors; au++){
			//CALCULATION:
			
			//variables
			double sum=0;
			double rvalue=-1;
	
			int maxmatch=0;
			int charactersintrainingdoc=0;
				
			//going through all the suffixes
			for(int start=0; start<co.allocutexts[ut].memory.size()&&charactersintrainingdoc<mintraininglength; start++){
				maxmatch=0;
				charactersintrainingdoc++;
				//going through prefixes
				for (int compdoc=0; compdoc<co.allocauthors[au].training.size(); compdoc++){
					for(int complet=0;complet<co.allocauthors[au].training[compdoc].size();complet++){
						int actmax=0;
							for(int act=0; (act+start<co.allocutexts[ut].memory.size())&&(act+complet<co.allocauthors[au].training[compdoc].size()); act++){
								if(co.allocutexts[ut].memory[start+act]==co.allocauthors[au].training[compdoc][complet+act]){
									actmax=act+1;
								}
								else{
									break;
								}
								if(actmax>maxmatch){
									maxmatch=actmax;			
								}
								actmax=0;
						}
					}		
				}
				sum=sum+maxmatch;	
			}	
	
			rvalue=sqrt(2*sum/((co.allocutexts[ut].memory.size()+1)*co.allocutexts[ut].memory.size()));
			
			cout << "Compared " << co.allocutexts[ut].getname() << " with " << co.allocauthors[au].getname() << ". R-value: " << rvalue << endl;
			
			rmem[au]=rvalue;			
			
		}
		
		int maxr=0;
		double maxv=0;
		
		for(int p=0;p<numberofauthors;p++){
			if(rmem[p]>maxv){
				maxv=rmem[p];
				maxr=p;
			}
		}
		
		//Calculating score:
		
		double scmin=0.02870035; //Best value has been calculated with a test run on the data  - determines whether there will be given an answer or not.
		double scsum=0.0;
		
		double scmem[numberofauthors];
		
		for(int sca=0;sca<numberofauthors;sca++){
			scmem[sca]=rmem[sca]/rmem[maxr];
		}
		
		for(int j=0;j<numberofauthors;j++){
			if(j!=maxr){
				if(scmem[j]==scmem[maxr]){
					scsum=0;
					break;
				}
				scsum=scsum+((rmem[maxr]-rmem[j])/(rmem[maxr]+rmem[j]));
			}
		}
	
		double sc=scsum/numberofauthors;
	
		
		//Output on console:
		
		cout  << endl <<  "The unknown text " << co.allocutexts[ut].getname() << " is associated with "; 
		
		if (sc>scmin){
			cout << co.allocauthors[maxr].getname(); 
		}
		else{
			cout << "(" << co.allocauthors[maxr].getname() << ")"; 
		}
		
		cout << ". Score: " << sc << endl << endl; 
		
		//json - output:
		
		answerList* newlist = new answerList;
		newlist->unknown_text = co.allocutexts[ut].getname();
		if(sc>scmin){ 
			newlist->author = co.allocauthors[maxr].getname();	
		}
		else{
			newlist->author = "candidate00000";
		}
		newlist->score = sc;
		
		if(llist){
			llist->nextAnswer = newlist;
		}
		else{
			flist=newlist;
		}
		llist=newlist;
		
		co.allocresults.push_back(maxr);
	}
	llist->nextAnswer = 0;
	processAnswers(flist,output_path);
	return 0;	
}

//Function for reading the input:

inputMetadata* readMetadata(string path)
{
	Jzon::Object rootNode;
    Jzon::FileReader::ReadFile(path, rootNode);

	inputMetadata* result = new inputMetadata();
	
	result->unknown_folder = rootNode.Get("folder").ToString();
	result->language = rootNode.Get("language").ToString();
	result->encoding = rootNode.Get("encoding").ToString();
	result->candidates = 0;
	result->unknown = 0;

	candidateAuthors* tempA = 0;
	unknownTexts* tempU = 0;

	const Jzon::Array &candidates = rootNode.Get("candidate-authors").AsArray();
	for (Jzon::Array::const_iterator it = candidates.begin(); it != candidates.end(); ++it)
    {
		candidateAuthors* nextAuthor = new candidateAuthors();

		Jzon::Object obj = (*it).AsObject();

		nextAuthor->author_name = obj.Get("author-name").ToString();
		nextAuthor->next = 0;

		if(tempA)
		{
			tempA->next = nextAuthor;
		}

		tempA = nextAuthor;

		if(!result->candidates)
		{
			result->candidates = tempA;
		}
    }

	const Jzon::Array &unknown = rootNode.Get("unknown-texts").AsArray();
	for (Jzon::Array::const_iterator it = unknown.begin(); it != unknown.end(); ++it)
    {
		unknownTexts* nextText = new unknownTexts();

		Jzon::Object obj = (*it).AsObject();

		nextText->unknown_text = obj.Get("unknown-text").ToString();
		nextText->next = 0;

		if(tempU)
		{
			tempU->next = nextText;
		}

		tempU = nextText;

		if(!result->unknown)
		{
			result->unknown = tempU;
		}
    }

	return result;
}

//Function for writing the output:

void processAnswers(answerList* llist, string output_path)
{
	Jzon::Object root;
    Jzon::Array answers;

	while(llist)
	{
		Jzon::Object answerOne;
		answerOne.Add("unknown_text", llist->unknown_text);
		answerOne.Add("author", llist->author);
		answerOne.Add("score", llist->score);

		answers.Add(answerOne);
		llist = llist->nextAnswer;
	}

    root.Add("answers", answers);

    Jzon::FileWriter::WriteFile((output_path+"results.json"), root, Jzon::StandardFormat);
}
