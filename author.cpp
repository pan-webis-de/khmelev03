class author{
	public:
		//Empty constructor
		author(){
			name="new";
		}
		//Constructor with given name
		author(string nm){
			name=nm;
		}
		//Functions for the name
		string getname(){
			return name;
		}	
		string givename(string newname){
			name=newname;
		}
		//Memory variables
		string name;
		vector <vector  <char> > training; //build vector
};
