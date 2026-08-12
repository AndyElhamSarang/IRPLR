class file
{
 private:
  
 public:
  
  vector<vector<string>> instances;
  string InstanceType;
  
  string InstanceFile;
  vector<string> InstanceDirectories;
  string JSONDirectory;
  //vector< is > isfile;
  void ReadDirectory();
  void ReadIRPInstanceName();

  void ReadGlobalSettings();
  void ReadGlobalParameter();
};
