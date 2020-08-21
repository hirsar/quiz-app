#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iomanip>

namespace fs = std::filesystem;
using namespace std;

class Problem
{
	private:
		string name;
		string course;
		string topic;
		double correct;
		double wrong;
		double total;
		double rate;
	public:
		Problem(string Nm, string Crs, string Tpc):name(Nm), course(Crs), topic(Tpc) {correct = 0; wrong = 0; total = 0; rate = 0;}	
		~Problem() {}
		void loadStats()
		{
			fstream statFile;
			string path = course+"/"+topic+"/stat/"+name+".txt";
			statFile.open(path.data(), ios_base::in);
			if(statFile.is_open())
			{
				statFile >> correct;
				statFile >> wrong;
				statFile >> total;
				statFile >> rate;
				statFile.close();
			}
		}
		void updateStats()
		{
			fstream statFile;
			string path = course+"/"+topic+"/stat/"+name+".txt";
			statFile.open(path.data(), ios_base::out);
			if (statFile.is_open())
			{
				statFile << correct << endl;
				statFile << wrong << endl;
				statFile << total << endl;
				statFile << rate << endl;
				statFile.close();
			}
		}
		void openProblem()
		{
			string path = course+"/"+topic+"/prob/"+name+".pdf";
			string command = "evince " + path + " &";
			system(command.data());
		}
		void openSolution()
		{
			string path = course+"/"+topic+"/sol/"+name+".pdf";
			string command = "evince " + path + " &";
			system(command.data());
		}
		void answer()
		{
			cout << "Did you get it right? [y/n] ";
			string response;
			cin >> response;
			if(response=="y")
			{
				correct += 1.0;
				total += 1.0;
				rate = correct/total;
			}
			else
			{
				wrong += 1.0;
				total += 1.0;
				rate = correct/total;
			}
		}
};

class Topic
{
	private:
		string course;
		string name;
		double correct;
		double wrong;
		double total;
		double rate;
	public:
		Topic(string nm, string crs):name(nm), course(crs) {correct = 0.0; wrong = 0.0; total = 0.0; rate = 0.0;}	
		void updateStats()
		{
			fstream statFile;
			string path = "../Practice/"+course+"/"+name+"/stat";
			double tc;
			double tw;
			double tt;
			for (const auto & entry : fs::directory_iterator(path))
			{
				statFile.open(entry.path().string().data(), ios_base::in);
				statFile >> tc;
				statFile >> tw;
				statFile >> tt;
				statFile.close();
				correct += tc;
				wrong += tw;
				total += tt;
			}
			rate = correct/total;
			path = course+"/"+name+"/topstat.txt";
			statFile.open(path.data(), ios_base::out);
			statFile << correct << endl;
			statFile << wrong << endl;
			statFile << total << endl;
			statFile << rate << endl;
			statFile.close();
		}
};

int main()
{
	fstream statFile;	
	string response;
	string course;
	string topic;
	string path = "../Practice";
	string rootpath = path;
	string statpath;
	double correct, wrong, total, rate;
	cout << fixed << setprecision(4);
	cout << "Choose a course: " << endl;
	for (const auto & entry : fs::directory_iterator(path))
	{
		if(entry.is_directory())
		{
			cout << entry.path().stem().string() << endl;
		}
	}
	cout << "---------------------" << endl;
	cout << "Enter your response: ";
	cin >> response;
	course = response;
	while(response!="q")
	{
		path = rootpath+"/"+course;
		cout << "---------------------" << endl;
		cout << "Choose a topic: " << endl;
		for (const auto & entry : fs::directory_iterator(path))
		{
			if(entry.is_directory())
			{
				cout << entry.path().stem().string() << " ";
				Topic* top = new Topic(entry.path().filename().string(), course);
				top->updateStats();
				statFile.open((entry.path().string()+"\topstat.txt").data(), ios_base::in);
				statFile >> correct;
				statFile >> wrong;
				statFile >> total;
				statFile >> rate;
				cout << rate*100 << "% " << "\t" << correct << "/" << total << endl;
				statFile.close();
				delete top;
			}
		}
		cout << "---------------------" << endl;
		cout << "Enter your response: ";
		cin >> response;
		if(response=="q")
		{
			break;
		}
		else if(response=="c")
		{
			continue;
		}
		topic = response;
		Topic* top = new Topic(topic, course);	
		statpath = path;
		while(response!="q")
		{
			path = rootpath+"/"+course+"/"+topic+"/prob/";
			statpath = rootpath+"/"+course+"/"+topic+"/stat/";
			cout << "---------------------" << endl;
			cout << "Choose a problem: " << endl;
			for (const auto & entry : fs::directory_iterator(path))
			{
				cout << entry.path().stem().string() << "\t";
				statFile.open((statpath+entry.path().stem().string()+".txt").data(),ios_base::in);
				statFile >> correct;
				statFile >> wrong;
				statFile >> total;
				statFile >> rate;
				cout << rate*100 << "% " << "\t" << correct << "/" << total << endl;
				statFile.close();
			}
			cout << "---------------------" << endl;
			cout << "Enter your response: ";
			cin >> response;
			if(response=="q")
			{
				return 0;
			}
			else if(response=="c")
			{
				break;
			}
			Problem* prob = new Problem(response, course, topic);
			prob->loadStats();
			prob->openProblem();
			cout << "View Solution? [y/n]" << endl;
			cin >> response;
			if(response=="y")
			{
				prob->openSolution();
				prob->answer();
				prob->updateStats();
				top->updateStats();
			}
			else
			{
				response="q";
				cout << response << endl;
			}
			delete prob;
		}
		top->updateStats();
		delete top;
	}
}
