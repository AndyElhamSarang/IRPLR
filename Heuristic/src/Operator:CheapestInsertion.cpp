#include "lib.h"
int solution_improvement::OperatorCheapestInsertion(input &IRPLR, vector<int> &route, int &day, int &vehicle, int &CustomerToReinsert,  double &PenaltyForStockOut, double &CurrentTransportationCost, preprocessing &memory)
{
	//Given a customer (CustomerToReinsert) in route, find the cheapest insertion position for this customer
	
	int whether_improve_move_been_found = 0;

	vector<int> move;
	for (int move_temp = 0; move_temp < 3; move_temp++)
	{
		move.push_back(0);
	}

	for (int j = 0; j <= route.size(); j++)
	{

		if ((j != CustomerToReinsert + 1) && (j != CustomerToReinsert))
		{
			double TempTransportationCost = memory.ConcatenateSingleRoute(day, vehicle, j, CustomerToReinsert, 1, route, IRPLR, memory);
			//  cout<<routeobj<<endl;

				//cout << "CurrentTransportationCost:" << CurrentTransportationCost << ", TempTransportationCost:" << TempTransportationCost << endl;
			if (CurrentTransportationCost - TempTransportationCost > 0.00001)
			{
				//cout << "Improved Transportation Cost"<<endl;
				whether_improve_move_been_found = 1;
				CurrentTransportationCost = TempTransportationCost;
				move[0] = j;
				move[1] = CustomerToReinsert;
				move[2] = 1;
			}
			/*vector<int> CopyOfRoute(route);
			CopyOfRoute.insert(CopyOfRoute.begin() + j,
							   CopyOfRoute.begin() + CustomerToReinsert,
							   CopyOfRoute.begin() + CustomerToReinsert + 1);

			if (j < CustomerToReinsert)
			{
				CopyOfRoute.erase(CopyOfRoute.begin() + CustomerToReinsert + 1,
								  CopyOfRoute.begin() + CustomerToReinsert + 1 + 1);
			}
			else
			{
				CopyOfRoute.erase(CopyOfRoute.begin() + CustomerToReinsert,
								  CopyOfRoute.begin() + CustomerToReinsert + 1);
			}

			for (int i = 0; i < CopyOfRoute.size(); i++)
			{
				cout << CopyOfRoute[i] << ",";
			}
			cout << endl;

			double TransportationCostForTheDay = 0;
			if (CopyOfRoute.size() != 0)
			{
				TransportationCostForTheDay += IRPLR.Distance[0][CopyOfRoute[0] + 1];

				for (int k = 0; k < CopyOfRoute.size() - 1; k++)
				{
					TransportationCostForTheDay += IRPLR.Distance[CopyOfRoute[k] + 1][CopyOfRoute[k + 1] + 1];
				}
				TransportationCostForTheDay += IRPLR.Distance[CopyOfRoute[CopyOfRoute.size() - 1] + 1][0];
			}
			cout << "TransportationCost:" << TempTransportationCost << ", " << TransportationCostForTheDay << endl;
			assert(abs(TempTransportationCost - TransportationCostForTheDay) <= 0.00001);*/
		}
	}

	if (whether_improve_move_been_found != 0)
	{
		vector<int> CopyOfRoute(route);
		route.insert(route.begin() + move[0], CopyOfRoute.begin() + move[1], CopyOfRoute.begin() + move[1] + move[2]); 

		if (move[0] < move[1])
		{
			route.erase(route.begin() + move[1] + move[2], route.begin() + move[1] + move[2] + move[2]);
		}
		else
		{
			route.erase(route.begin() + move[1], route.begin() + move[1] + move[2]);
		}
	}

	return whether_improve_move_been_found;
}
