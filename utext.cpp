class utext{
	public:
		utext(string indname){
			name=indname;
		}
		string getname(){
			return name;
		}
		void givename(string nm){
			name=nm;
		}
		string name;
		vector <char> memory;
};
