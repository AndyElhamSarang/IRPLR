class file
{
 private:
  
 public:
  
  vector<vector<string>> instances;
  string InstanceType;
  
  string InstanceFile;
  vector<string> InstanceDirectories;
  //vector< is > isfile;
  void ReadDirectory();
  void ReadIRPInstanceName();

  void ReadGlobalParameter();
};
