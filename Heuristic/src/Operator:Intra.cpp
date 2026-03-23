#include "lib.h"
int solution_improvement::OperatorIntra(input &IRPLR, vector<int> &route, int &day, int &vehicle, int &MinToReinsert, int &MaxToReinsert, preprocessing &memory, double &CurrentTransportationCost)
{
	// Given a number of customers (NumberOfCustomersToReinsert) in route
	// cout << "Start Intra-route optimization" << endl;
	int whether_improve_move_been_found = 0;

	vector<int> move;
	for (int move_temp = 0; move_temp < 3; move_temp++)
	{
		move.push_back(0);
	}

	vector<int> ImprovingRoute;
	// for (int i = 0; i < route.size(); i++)
	// {
	// 	cout << route[i] << ",";
	// }
	// cout << endl;

	for (int InsertFrom = 0; InsertFrom <= route.size(); InsertFrom++)
	{
		for (int InsertTo = 0; InsertTo <= route.size(); InsertTo++)
		{
			for (int NumberOfCustomersToReinsert = MinToReinsert; NumberOfCustomersToReinsert <= MaxToReinsert; NumberOfCustomersToReinsert++)
			{
				if (InsertFrom + NumberOfCustomersToReinsert <= route.size())
				{

					// Skip moves that would produce the same route
					bool same_route =
						(InsertTo == InsertFrom) ||
						(InsertTo == InsertFrom + NumberOfCustomersToReinsert);

					// Skip moves where insertion point lies inside the block being moved
					bool overlap =
						(InsertTo > InsertFrom && InsertTo < InsertFrom + NumberOfCustomersToReinsert);

					if (!same_route && !overlap)
					{
						if (NumberOfCustomersToReinsert <= 3)
						{
							////////////////////////////////////////////////////////////////////////////////////////////
							//                                                                                        //
							//           efficient evaluation only works for 2 or less customers for now              //
							//                                                                                        //
							////////////////////////////////////////////////////////////////////////////////////////////
							double TempTransportationCost = memory.ConcatenateSingleRoute(day, vehicle, InsertTo, InsertFrom, NumberOfCustomersToReinsert, route, IRPLR, memory);

							// cout << "CurrentTransportationCost:" << CurrentTransportationCost << ", TempTransportationCost:" << TempTransportationCost << endl;
							if (CurrentTransportationCost - TempTransportationCost > 0.00001)
							{
								// cout << "Improved Transportation Cost"<<endl;
								whether_improve_move_been_found = 1;
								CurrentTransportationCost = TempTransportationCost;
								move[0] = InsertTo;
								move[1] = InsertFrom;
								move[2] = NumberOfCustomersToReinsert;
							}
							// vector<int> CopyOfRoute(route);
							// CopyOfRoute.insert(CopyOfRoute.begin() + InsertTo,
							// 				   CopyOfRoute.begin() + InsertFrom,
							// 				   CopyOfRoute.begin() + InsertFrom + NumberOfCustomersToReinsert);

							// if (InsertTo < InsertFrom)
							// {
							// 	CopyOfRoute.erase(CopyOfRoute.begin() + InsertFrom + NumberOfCustomersToReinsert,
							// 					  CopyOfRoute.begin() + InsertFrom + NumberOfCustomersToReinsert + NumberOfCustomersToReinsert);
							// }
							// else
							// {
							// 	CopyOfRoute.erase(CopyOfRoute.begin() + InsertFrom,
							// 					  CopyOfRoute.begin() + InsertFrom + NumberOfCustomersToReinsert);
							// }

							// for (int i = 0; i < CopyOfRoute.size(); i++)
							// {
							// 	cout << CopyOfRoute[i] << ",";
							// }
							// cout << endl;

							// double TransportationCostForTheDay = 0;
							// if (CopyOfRoute.size() != 0)
							// {
							// 	TransportationCostForTheDay += IRPLR.Distance[0][CopyOfRoute[0] + 1];

							// 	for (int k = 0; k < CopyOfRoute.size() - 1; k++)
							// 	{
							// 		TransportationCostForTheDay += IRPLR.Distance[CopyOfRoute[k] + 1][CopyOfRoute[k + 1] + 1];
							// 	}
							// 	TransportationCostForTheDay += IRPLR.Distance[CopyOfRoute[CopyOfRoute.size() - 1] + 1][0];
							// }

							// for (int i = 0; i < CopyOfRoute.size(); i++)
							// {
							// 	cout << CopyOfRoute[i] << ",";
							// }
							// cout << endl;
							// cout << "TransportationCost:" << TempTransportationCost << ", " << TransportationCostForTheDay << endl;
							// assert(abs(TempTransportationCost - TransportationCostForTheDay) <= 0.00001);
						}
						else
						{
							vector<int> CopyOfRoute(route);
							CopyOfRoute.insert(CopyOfRoute.begin() + InsertTo,
											   CopyOfRoute.begin() + InsertFrom,
											   CopyOfRoute.begin() + InsertFrom + NumberOfCustomersToReinsert);

							if (InsertTo < InsertFrom)
							{
								CopyOfRoute.erase(CopyOfRoute.begin() + InsertFrom + NumberOfCustomersToReinsert,
												  CopyOfRoute.begin() + InsertFrom + NumberOfCustomersToReinsert + NumberOfCustomersToReinsert);
							}
							else
							{
								CopyOfRoute.erase(CopyOfRoute.begin() + InsertFrom,
												  CopyOfRoute.begin() + InsertFrom + NumberOfCustomersToReinsert);
							}

							// for (int i = 0; i < CopyOfRoute.size(); i++)
							// {
							// 	cout << CopyOfRoute[i] << ",";
							// }
							// cout << endl;

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
							if (CurrentTransportationCost - TransportationCostForTheDay > 0.00001)
							{
								// cout << "Improved Transportation Cost" << endl;
								whether_improve_move_been_found = 1;
								CurrentTransportationCost = TransportationCostForTheDay;
								ImprovingRoute = CopyOfRoute;
							}
						}
					}
				}
			}
		}
	}

	if (whether_improve_move_been_found != 0)
	{
		memory.TrackSolutionStatus[day][vehicle] = 1;
		if (move[2] <= 3)
		{
			// cout << "whether_improve_move_been_found:" << whether_improve_move_been_found << endl;
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

			// double CheckTransportationCost = 0;
			// if (route.size() != 0)
			// {
			// 	CheckTransportationCost += IRPLR.Distance[0][route[0] + 1];

			// 	for (int k = 0; k < route.size() - 1; k++)
			// 	{
			// 		CheckTransportationCost += IRPLR.Distance[route[k] + 1][route[k + 1] + 1];
			// 	}
			// 	CheckTransportationCost += IRPLR.Distance[route[route.size() - 1] + 1][0];
			// }

			// for (int i = 0; i < route.size(); i++)
			// {
			// 	cout << route[i] << ",";
			// }
			// cout << endl;
			// cout<<"CheckTransportationCost:"<<CheckTransportationCost<<", CurrentTransportationCost:"<<CurrentTransportationCost<<endl;
			// assert(abs(CheckTransportationCost - CurrentTransportationCost) <= 0.00001);
		}
		else
		{
			assert(ImprovingRoute.size() != 0);
			route = ImprovingRoute;
		}
	}
	// cout << "End Intra-route optimization" << endl;
	return whether_improve_move_been_found;
}
