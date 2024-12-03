 if (k == 0)
                    {
                        //cout << "From depot" << endl;
                        if (TempRoute[i][j].size() <= 1)
                        {
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                                              }
                        else
                        {
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][TempRoute[i][j][k + 1] + 1] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][TempRoute[i][j][k + 1] + 1];
                                 }
                    }
                    else if (k == IRPSolution.Route[i][j].size() - 1)
                    {
                        if (TempRoute[i][j].size() <= 1)
                        {
                            ChangeInTransportationCost = IRPLR.Distance[0][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[0][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                                                }
                        else
                        {
                            ChangeInTransportationCost = IRPLR.Distance[TempRoute[i][j][k - 1] + 1][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][0] - IRPLR.Distance[TempRoute[i][j][k - 1] + 1][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][0];
                                                   }
                    }
                    else
                    {
                        ChangeInTransportationCost = IRPLR.Distance[TempRoute[i][j][k - 1] + 1][TempRoute[i][j][k] + 1] + IRPLR.Distance[TempRoute[i][j][k] + 1][TempRoute[i][j][k + 1] + 1] - IRPLR.Distance[TempRoute[i][j][k - 1] + 1][tempCustomer + 1] - IRPLR.Distance[tempCustomer + 1][TempRoute[i][j][k + 1] + 1];
                          }