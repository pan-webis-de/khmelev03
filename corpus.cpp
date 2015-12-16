class corpus {
	public:
		//Constructor
		corpus(string indname){
			name=indname;
			cout << "Opened corpus " << name << "." << endl;
		}
		//Functions for the name
		string getname(){
			return name;
		}
		//Get the numbers
		int getnofauthors(){
			return nofauthors;
		}
		int getnofunknown(){
			return nofunknown;
		}
		//Adding an author
		void makeauthor(string nm){
			author newone(nm);
			allocauthors.push_back(newone);
			cout << "Added author " << nm << "." << endl;
		}
		//Adding a text
		void makeutext(string nm){
			utext newone(nm);
			allocutexts.push_back(newone);
			cout << "Added unknown text " << nm << "." << endl;
 		}
		//Get the name of an author
		string getauthorname(int pos){
			return (allocauthors[pos].getname());
		}
		//Change the name of an author
		void giveauthorname (int pos, string argument){
			allocauthors[pos].givename(argument);	
		}
		//Get the name of a text
		string getutextname(int pos){
			return (allocutexts[pos].getname());
		}
		//Change the name of a text
		void giveutextname (int pos, string argument){
			allocutexts[pos].givename(argument);	
		}
		//Memory variables
		string name;
		int nofauthors;
		int nofunknown;
		vector <author> allocauthors; //Memory: number -> author
		vector <utext> allocutexts; //Memory: number -> unknown text
		vector <int> allocresults; //Memory: unknowntextnumber -> authornumber 
};
