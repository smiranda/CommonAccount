/**
* @file common_account.cpp
* @brief Manage a common account.
* @author Sebastiao
* @date 2015/20/06
* @copyright See LICENCE.txt
*/

#include <ctime>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
using namespace std;

namespace caccount {

   struct Record {
      string sys_timestamp;
      string rec_timestamp;
      uint16_t entity;
      double amount;
   };
   
   class Account : public vector <Record> {
   public:
      Account(): vector <Record>() {};
      
      bool LoadFromFile(const char * fname)
      {
         clear();
         ifstream file (fname);
         if (!file.good())
            return false;
         
         while(true){
            string line;
            auto& istr = getline(file, line);
            istringstream iss(line);       
            if (istr.eof())
               break;
               
            Record rec;
            iss   >> rec.sys_timestamp
                  >> rec.rec_timestamp
                  >> rec.entity
                  >> rec.amount;
            push_back (rec);
         }
         file.close();
      }
      
      bool DumpToFile(const char * fname)
      {
         ofstream file (fname, ios::trunc);
         bool result = DumpToStream(file);
         file.close();
         return result;
      }
      
      bool DumpToStream(ostream & file, bool show_total = false)
      {
         if (!file.good())
            return false;
            
         unordered_map<uint16_t, double> totals;
         for (const Record& rec : *this) {
            file  << rec.sys_timestamp << m_sep 
                  << rec.rec_timestamp << m_sep 
                  << rec.entity        << m_sep
                  << rec.amount        << endl;
                  
            if (totals.find(rec.entity) != totals.end()){
               totals[rec.entity] += rec.amount;
            } else {
               totals.insert({rec.entity, rec.amount});
            }                  
         }
         if (show_total){
            file  << "entity\t\tamount\t\tbalance" << endl;
            for (const auto& total : totals) {                                    
               double others = 0.0;
               for (const auto& total0 : totals) {
                  if (total0.first != total.first)
                     others += total0.second;
               }
               file  << total.first    << "\t\t"
                     << total.second   << "\t\t"
                     << (total.second/totals.size() - others/totals.size())
                     << endl;
            }
         }
      }
   private:
      const char m_sep = '\t'; // must be whitespace
   };
}

namespace {
   string SystemTimestamp(){
      time_t rawtime;
      tm * timeinfo;
      time (&rawtime);
      timeinfo = localtime (&rawtime);
      char buff [64];
      strftime(buff, 64, "%Y-%m-%d_%H:%M:%S", timeinfo);
      return string(buff);
   }
   const char * file_name = "account.dat";
   const char * program_header =
      "cma: Common Account Program\n"
      "Crash the program (Ctrl+C) to exit\n";
}

int main(int argc, char** argv)
{
   cout << program_header << endl;
   caccount::Account account;
   account.LoadFromFile(file_name);
   account.DumpToStream(cout, true);
   while (true){
      
      cout <<  "Insert record "
               "<entity:uint16_t> "
               "<amount:double> "
               "[timestamp:%d-%m]: " << endl;
      
      caccount::Record rec;
      cin >> rec.entity
          >> rec.amount
          >> rec.rec_timestamp;
      rec.sys_timestamp = SystemTimestamp();
      
      account.push_back(rec);
      account.DumpToStream(cout, true);
      account.DumpToFile(file_name);
   }
   
   return 0;
}