
#include <vector>
#include <sstream>
#include <unistd.h>

#include "GameThread.hpp"

using namespace std;

#define MinimumBet 5

// Thread constructor
GameThread::GameThread()
{
}

// Thread destructor
GameThread::~GameThread()
{
	qDebug() << "arghhh you've destroyed me!";
}

// Custom sleep function which can 
// be interupted by a New Game request
void GameThread::MySleep(int Seconds)
{
	for(int i = 0; i < (Seconds * 100); i++)
	{
		// Check for a new game request every 10ms
		if(CheckChoice() == 8)
		{
			break;
		}
		else
		{
			usleep(10000);
		}
	}
}

// Create a new box and assign it its connections
Box* GameThread::NewBox(Player* pPlayer, bool Split)
{
	Box *pBox;

	pBox = new Box(pPlayer, Split);
	connect(pBox, SIGNAL(updatePlayersHand(QString,int)), this, SIGNAL(UpdatePlayersHand(QString,int)));
	connect(pBox, SIGNAL(updatePlayersHandValue(QString)), this, SIGNAL(UpdatePlayersHandValue(QString)));
	connect(pBox, SIGNAL(ClearPlayersHand()), this, SIGNAL(ClearPlayersHand()));
	connect(pBox, SIGNAL(updateBet(QString)), this, SIGNAL(UpdateBetValue(QString)));
	connect(this, SIGNAL(IncreaseBet(float)), pBox, SLOT(IncreaseBet(float)));
	connect(this, SIGNAL(DecreaseBet(float)), pBox, SLOT(DecreaseBet(float)));

	return pBox;
}

// Create a box for every player at the table
// Structure from multiplayer version
void GameThread::SeatPlayers(Table& BlackJackTable)
{
	Box *pBox;
	Player *pPlayer;

	// Create a player object to link to each box
	pPlayer = new Player();
	connect(pPlayer, SIGNAL(updateStack(QString)), this, SIGNAL(UpdateStackValue(QString)));
	pBox = NewBox(pPlayer, false);
	BlackJackTable.AddBox(pBox, 0);
}

// Collect bets from every player
void GameThread::CollectBets()
{
	QEventLoop eventLoop;
	connect(this, SIGNAL(BetDone()), &eventLoop, SLOT(quit()));
	// Only checks that any of the buttons will exit loop
	// but ok as only Done button visible
	connect(this, SIGNAL(Choice()), &eventLoop, SLOT(quit()));

	emit ButtonVisibility(false, false, false, false, false, false, false, false);
	emit UpdateGameStatus("Place your bet");
	// Only allow chip movement during betting period
	emit EnableChips(true);	
	eventLoop.exec();
	emit EnableChips(false);
}

// Deal everyone their first two cards (including the dealer)
void GameThread::InitialDeal(Table& BlackJackTable, Croupier& Dealer)
{
	int Deal;
	Card DealtCard;

	Box* CurrentBox = BlackJackTable.GetBox(0);

	for(Deal = 0; Deal < 2; Deal++)
	{
    	DealtCard = Dealer.Deal();
		CurrentBox->TakeCard(DealtCard);
	    DealtCard = Dealer.Deal();
		Dealer.TakeCard(DealtCard);
	}
}

// Check for player blackjack and state any insurance options
void GameThread::InsuranceOffers(Table& BlackJackTable, Croupier& Dealer)
{
	QEventLoop eventLoop;
	connect(this, SIGNAL(Choice()), &eventLoop, SLOT(quit()));

	Box* CurrentBox = BlackJackTable.GetBox(0);
		
	// Player has blackjack - display message
	if(CurrentBox->CheckHand() == 21)
	{
		// Display dealers cards - one face up, one face down
		Dealer.InitialStatus();
		emit UpdateGameStatus("Blackjack!");
		emit ResultTextVisibility(false, false, true, false);
		MySleep(2);
		if(CheckChoice() == 8)
		{
			return; 
		}
		CurrentBox->Status();
		emit ResultTextVisibility(false, false, false, false);
	}
				
	// If dealer has an ace as their upcard, and they have enough money, offer insurance
	if (Dealer.CheckCard(0) == "Ace" and (CurrentBox->CountStack() >= (CurrentBox->CountBet() / 2)))
	{
		// If the player has blackjack their status will already be displayed
		if(CurrentBox->CheckHand() != 21)
		{
			Dealer.InitialStatus();
			CurrentBox->Status();
		}

		// Hands of blackjack receive different message for same offer
		if (CurrentBox->CheckHand() == 21)
		{
			emit UpdateGameStatus("The dealer is showing an ace\nEven money?");
		}
		else
		{
			emit UpdateGameStatus("The dealer is showing an ace\nWould you like insurance?");
		}
		emit ButtonVisibility(false, false, false, false, false, true, true, false);
		eventLoop.exec();
		emit ButtonVisibility(false, false, false, false, false, false, false, false);

		if(CheckChoice() == 6)
		{
			CurrentBox->PlaceInsurance();
		}
		else if(CheckChoice() == 7)
		{
		}
	}
}

/* Check for any blackjacks and handle insurance claims accordingly */
bool GameThread::CheckForBlackJack(Table& BlackJackTable, Croupier& Dealer)
{
	bool DealerBlackjack = false;
	bool PauseForMessage = false;

	/* If the dealer has blackjack, show it! */
	if (Dealer.CheckHand() == 21)
	{
		emit UpdateGameStatus("Dealer has blackjack!");
		Dealer.Status();
		DealerBlackjack = true;
		emit ResultTextVisibility(false, false, false, true);

/********************** added ********************************/
		Box* CurrentBox = BlackJackTable.GetBox(0);
		CurrentBox->Status();
/*************************************************************/
		MySleep(2);
		if (CheckChoice() == 8)
		{
			return false;
		}
		emit ResultTextVisibility(false, false, false, false);
	}
	else
	{
		DealerBlackjack = false;
	}

	Box* CurrentBox = BlackJackTable.GetBox(0);
	/* Player has blackjack */
	if (CurrentBox->CheckHand() == 21)
	{
		/* Player has taken out insurance */
		if (CurrentBox->CheckInsurance() == true)
		{
			emit UpdateGameStatus("Even money");
			PauseForMessage = true;
		}
		/* Player hasn't taken out insurance and Dealer has blackjack */
		else if(CurrentBox->CheckInsurance() == false and DealerBlackjack == true)
		{
			emit UpdateGameStatus("Dealer and player both have blackjack!\nPush!");
			PauseForMessage = true;
		}
	}

	/* Keep any messages on the screen for a bit */
	// This is required as player blackjack is paused in a previous
	// function and we don't want to repeat it here
	if (PauseForMessage == true)
	{
		MySleep(2);
		if (CheckChoice() == 8)
		{
			return false;
		}
	}

	/* Player has taken out insurance and doesn't have blackjack */
	if (CurrentBox->CheckInsurance() == true and CurrentBox->CheckHand() != 21)
	{
		/* Dealer doesn't have blackjack */
		if (DealerBlackjack == false)
		{
			emit UpdateGameStatus("Dealer doesn't have blackjack\nInsurance bet lost");
		}
		/* Dealer has blackjack */
		else
		{
			emit UpdateGameStatus("Dealer has blackjack\nInsurance bet pays out");
		}
		MySleep(2);
		if (CheckChoice() == 8)
		{
			return false;
		}
	}
	return DealerBlackjack;
}

/* Give player options on how they can play their hand */
void GameThread::StateOptions(Box* CurrentBox, bool PlayersFirstGo)
{
	string FirstCard = CurrentBox->CheckCard(0);
	string SecondCard = CurrentBox->CheckCard(1);

	bool SurrenderVisible = false;
	bool DoubleVisible = false;
	bool SplitVisible = false;

	if (CurrentBox->CheckSplit() == true)
	{
		emit UpdateGameStatus("Split Hand\nWhat would you like to do?");			
	}
	else
	{
		emit UpdateGameStatus("What would you like to do?");
	}

	/* Initial hand stage */
	if (CurrentBox->CountCards() == 2)
	{	
		/* Player has enough money to double or split */
		if (CurrentBox->CountStack() >= CurrentBox->CountBet())
		{
			DoubleVisible = true;
			/* First two cards are the same */
			if (FirstCard.compare(SecondCard) == 0)
			{
				SplitVisible = true;
			}
		}

		/* Not after split */
		if (PlayersFirstGo == true and CurrentBox->CheckSplit() != true)
		{
			SurrenderVisible = true;
		}
	}
	emit ButtonVisibility(true, true, SurrenderVisible, DoubleVisible, SplitVisible, false, false, false);
}

int GameThread::CheckChoice()
{
	return ButtonChoice;
}

void GameThread::ChoiceMade(int ButtonIndex)
{
	qDebug() << "button " << ButtonIndex << " was pressed";
	ButtonChoice = ButtonIndex;
	emit Choice();
}

/* Every player plays their hands */
bool GameThread::PlayersPlay(Table& BlackJackTable, Croupier& Dealer)
{
	int BoxIndex;
	string Choice;
	Card DealtCard;
	int SplitIndex;
	/* Keep track of if any players are left for dealer to play against */
	bool PlayersStanding = false;
	/* Certain plays can only take place upon a player's first go */
	bool PlayersFirstGo;
	
	QEventLoop eventLoop;
	connect(this, SIGNAL(Choice()), &eventLoop, SLOT(quit()));

	for(BoxIndex = 0; BoxIndex < BlackJackTable.CountBoxes(); BoxIndex++)
	{
		Box* CurrentBox = BlackJackTable.GetBox(BoxIndex);
		PlayersFirstGo = true;

		/* Check player doesn't already have blackjack */
		if (CurrentBox->CheckHand() != 21)
		{			
			do
			{
				Box* CurrentBox = BlackJackTable.GetBox(BoxIndex);
				/* Check first two cards in case a split is possible */
				string FirstCard = CurrentBox->CheckCard(0);
				string SecondCard = CurrentBox->CheckCard(1);

				Dealer.InitialStatus(); 
				CurrentBox->Status();
					
				/* Check for a non-blackjack 21 */
				if(CurrentBox->CheckHand() == 21)
				{
					/* Auto-stand on a 21 */
					emit UpdateGameStatus("Auto-standing on 21");
					emit ButtonVisibility(false, false, false, false, false, false, false, false);
					PlayersStanding = true;
					MySleep(2);
					break;
				}
				
				/* Tell the player their options of play */
				StateOptions(CurrentBox, PlayersFirstGo);
				eventLoop.exec();

				/* Player hits */
				if(CheckChoice() == 1)
				{
					PlayersFirstGo = false;		
					DealtCard = Dealer.Deal();
					CurrentBox->TakeCard(DealtCard);
					QString CardName = DealtCard.GetName();
					QString CardSuit = DealtCard.GetSuit();
					QString LoadCardName = ":/PlayerCards/" + CardName + CardSuit + ".png";

					/* If the hit causes them to go bust */
					if(CurrentBox->CheckHand() > 21)
					{
						emit UpdateGameStatus("Bust!");
						emit ButtonVisibility(false, false, false, false, false, false, false, false);
						CurrentBox->Status();
						emit ResultTextVisibility(true, false, false, false);
						MySleep(2);
						emit ResultTextVisibility(false, false, false, false);
						break;
					}	
				}
				/* Player stands */
				else if(CheckChoice() == 2)
				{
					PlayersStanding = true;
				}
				else if(CheckChoice() == 8)
				{
					PlayersStanding = false;
					qDebug() << "\nPlayer trying to start a new game....";
					return PlayersStanding;
				}
				/* Certain plays can only happen if player is holding two cards (regardless of whether first go or not) */
				if (CurrentBox->CountCards() == 2)
				{
					/* Players can only surrender on their first go */
					if (PlayersFirstGo == true and CheckChoice() == 5)
					{
						emit UpdateGameStatus("You surrendered\nHalf of bet returned");
						emit ButtonVisibility(false, false, false, false, false, false, false, false);
						CurrentBox->Surrender();
						MySleep(2);
						break;
					}
					/* Some plays are only possible if a player has enough money in their stack */
					if (CurrentBox->CountStack() >= CurrentBox->CountBet())
					{
						/* Player doubles */
						if (CheckChoice() == 3)
						{		
							/* Place bet again to double initial bet and take one last card */
							CurrentBox->IncreaseBet(CurrentBox->CountBet());
							emit UpdateGameStatus("Initial bet doubled\nDealing one more card....");
							emit ButtonVisibility(false, false, false, false, false, false, false, false);
							MySleep(2);
							if (CheckChoice() == 8)							
							{
								break;
							}
							// Hide the new bet value after the 2 second pause
							emit UpdateBetValue("0");
							DealtCard = Dealer.Deal();
							CurrentBox->TakeCard(DealtCard);
							
							PlayersFirstGo = false;	
						//	PlayersStanding = true;

	/*						CurrentBox->Status();							
							sleep(2);
							break;*/
							/***************************************/
							if(CurrentBox->CheckHand() > 21)
							{
								PlayersStanding = false;
								emit UpdateGameStatus("Bust!");
								emit ButtonVisibility(false, false, false, false, false, false, false, false);
								CurrentBox->Status();
								emit ResultTextVisibility(true, false, false, false);
								MySleep(2);
								emit ResultTextVisibility(false, false, false, false);
								break;
							}	
							else
							{
								PlayersStanding = true;
								CurrentBox->Status();							
								MySleep(2);
								break;
							}
							/***************************************/
						}
						/* Players can split if they have two of the same card */
						if (FirstCard.compare(SecondCard) == 0 and CheckChoice() == 4)
						{
							PlayersFirstGo = false;	
							/* Add a new box to the table in the next position with same owner as the current box */
							Box *pBox;
							pBox = NewBox(CurrentBox->GetOwnerObj(), true);
							BlackJackTable.AddBox(pBox, BoxIndex + 1);

							/* Have to redefine CurrentBox after the insert */	
							Box* CurrentBox = BlackJackTable.GetBox(BoxIndex);
							Box* SplitBox = BlackJackTable.GetBox(BoxIndex+1);

							/* New box places same bet as current box */
							SplitBox->IncreaseBet(CurrentBox->CountBet());
							// The above call to IncreaseBet automatically updates the GUI
							// with the new bet value.  Send an update of zero to override this
							// Otherwise the new bet value stays on the screen until a decision
							// is made which doesn't match the normal time delay behaviour	
							emit UpdateBetValue("0");
							
							SplitBox->TakeCard(CurrentBox->MoveSplitCard());
							/* Deal a new card for both boxes */
							for(SplitIndex = BoxIndex; SplitIndex < BoxIndex + 2; SplitIndex++)
							{
								Box* CurrentBox = BlackJackTable.GetBox(SplitIndex);										
								DealtCard = Dealer.Deal();
								CurrentBox->TakeCard(DealtCard);	
							}	
						}
					}
				}
			/* Keep looping until everyone stands (or goes bust) */
			}while((CheckChoice() != 2) && (CheckChoice() != 8));
		}
		/* Split hands may have 21 but can't have blackjack */
		else if (CurrentBox->CheckHand() == 21 and CurrentBox->CheckSplit() == true)
		{
			CurrentBox->Status();
			MySleep(2);
		}
		if (CheckChoice() == 8)							
		{
			return false;
		}
	}
	emit ButtonVisibility(false, false, false, false, false, false, false, false);
	return PlayersStanding;
}

/* Dealer plays until hand value is greater than 17 */
void GameThread::DealerPlays(Croupier& Dealer)
{
	Card DealtCard;

	emit UpdateGameStatus("Dealer plays....");
	/* Show dealer's current status before continuing */
	Dealer.Status();
	MySleep(1);

	/* Keep taking cards until over 17 or bust */
	while ((Dealer.CheckHand() < 17) && (CheckChoice() != 8))
	{
		DealtCard = Dealer.Deal();
		Dealer.TakeCard(DealtCard);
		Dealer.Status();
		if(Dealer.CheckHand() > 21)
		{
			emit UpdateGameStatus("Dealer bust!");
			emit ResultTextVisibility(false, true, false, false);
			MySleep(2);
			emit ResultTextVisibility(false, false, false, false);
		}
		else
		{
			MySleep(1);
		}
	}
	
}

/* Remove broke players from the table and report on remaining players */
void GameThread::RefreshPlayerList(Table& BlackJackTable)
{
	Box* CurrentBox = BlackJackTable.GetBox(0);

	if (CurrentBox->CountStack() < MinimumBet)
	{
		BlackJackTable.RemoveBox(0);
	}
}

string GameThread::NumberToString(float Number)
{
	stringstream ss;
	ss << Number;
	return ss.str();
}

/* Work out who has won and who has lost and settle all bets */
void GameThread::SettleBets(Table& BlackJackTable, Croupier& Dealer)
{
	int BoxIndex;
//	QString ResultsSummary;
	std::string ResultsSummary;
	string StrCountBet;
	string StrCheckHand;
	string StrDealersCheckHand;
	string StrHalfBet;
	string StrBlackjack;
	string StrHandNum;
	string StrNumHands;

	bool PlayerWonBet;

	/* Give summary for every box at the table */
	for(BoxIndex = 0; BoxIndex < BlackJackTable.CountBoxes(); BoxIndex++)
	{
		Box* CurrentBox = BlackJackTable.GetBox(BoxIndex);

		StrCountBet = NumberToString(CurrentBox->CountBet());
		StrCheckHand = NumberToString(CurrentBox->CheckHand());
		StrDealersCheckHand = NumberToString(Dealer.CheckHand());
		StrHalfBet = NumberToString((CurrentBox->CountBet() / 2));
		StrBlackjack = NumberToString(CurrentBox->CountBet() + (CurrentBox->CountBet() / 2)) ;
		StrHandNum = NumberToString(BoxIndex + 1);
		StrNumHands = NumberToString(BlackJackTable.CountBoxes());

		emit UpdateGameStatus("Result for Hand " + QString::fromStdString(StrHandNum) + " of " + QString::fromStdString(StrNumHands));
	
		/* If player didn't surrender */
		if(CurrentBox->CheckSurrended() == false)
		{
			/* Start the summary with a different label if referring to a split hand */
			/* Note the equal sign here instead of += clears all text stored up to this point */
			if(CurrentBox->CheckSplit() == false)
			{
				ResultsSummary = "Hand of ";
			}
			else
			{
				ResultsSummary = "Split hand of ";
			}
			/* Player didn't go bust */
			if (CurrentBox->CheckHand() < 22)
			{
				/* Dealer had blackjack (checking for 2 cards to ensure they don't just have a 21 score) */
				if (Dealer.CheckHand() == 21 and Dealer.CountCards() == 2)
				{
					/* Player also had blackjack */
					if (CurrentBox->CheckHand() == 21 and CurrentBox->CountCards() == 2 and CurrentBox->CheckSplit() == false)
					{
						ResultsSummary += "blackjack in push with dealer's\nBet of " + StrCountBet + " returned\n";
						emit PlayLoseSound();
						CurrentBox->RetrieveBet();	
						/* Player took insurance */
						if (CurrentBox->CheckInsurance() == true)
						{
							/* Even money */
							CurrentBox->ReceiveInsurance();
							emit PlayWinSound();
							ResultsSummary += "Win insurance bet and collect " + StrCountBet + " from the dealer\n";
						}
					}
					/* Player didn't have blackjack as well */ 
					else
					{
						ResultsSummary += StrCheckHand + " beaten by dealer's blackjack\n";
						ResultsSummary += "Lose bet of " + StrCountBet + "\n";
						emit PlayLoseSound();
					}
				}
				/* Dealer doesn't have blackjack */
				else
				{
					/* Player had blackjack */
					if (CurrentBox->CheckHand() == 21 and CurrentBox->CountCards() == 2 and CurrentBox->CheckSplit() == false)
					{
						ResultsSummary += "blackjack beat's dealer's " + StrDealersCheckHand + "\n";
						ResultsSummary += "Win bet and collect " + StrBlackjack + " from the dealer\n";	
						emit PlayWinSound();
						PlayerWonBet = true;		
						CurrentBox->ReceiveWinnings();
					}
					/* Player didn't have blackjack */
					else
					{
						/* Player's hand is greater than dealer's hand or dealer is bust */
						if (CurrentBox->CheckHand() > Dealer.CheckHand() or Dealer.CheckHand() > 21)
						{
							ResultsSummary += StrCheckHand + " beats dealer's " + StrDealersCheckHand + "\n";	
							ResultsSummary += "Win bet and collect " + StrCountBet + " from the dealer\n";
							emit PlayWinSound();
							PlayerWonBet = true;
							CurrentBox->ReceiveWinnings();
						}
						/* Player's hand in push with dealer's hand */
						else if (CurrentBox->CheckHand() == Dealer.CheckHand())
						{
							ResultsSummary += StrCheckHand + " in push with dealer's " + StrDealersCheckHand + "\nBet of " + StrCountBet + " returned\n";	
							CurrentBox->RetrieveBet();
							emit PlayLoseSound();
						}
						/* Player's hand is less than dealer's hand */
						else
						{
							ResultsSummary += StrCheckHand + " beaten by dealer's " + StrDealersCheckHand + "\n";		
							ResultsSummary += "Lose bet of " + StrCountBet + "\n";
							emit PlayLoseSound();
						}
					}
				}
			}				
			/* Player went bust */
			else	
			{
				ResultsSummary += StrCheckHand + " went bust\n";
				ResultsSummary += "Lose bet of " + StrCountBet + "\n";
				emit PlayLoseSound();
			}
			
			/* Player took out insurance (need to check this even if player went bust) */
			if (CurrentBox->CheckInsurance() == true)
			{
				/* Player had blackjack */
				if (CurrentBox->CheckHand() == 21 and CurrentBox->CountCards() == 2 and CurrentBox->CheckSplit() == false)
				{
					/* Even money situation already dealt with above */
				}
				/* Player didn't have blackjack */
				else
				{
					/* Dealer had blackjack */
					if (Dealer.CheckHand() == 21 and Dealer.CountCards() == 2)
					{
						/* Message displayed with function */
						CurrentBox->ReceiveInsurance();
						emit PlayWinSound();
						ResultsSummary += "Win insurance bet and collect " + StrCountBet + " from the dealer\n";
					}
					/* Dealer didn't have blackjack */
					else
					{
						ResultsSummary += "Lose insurance bet of " + StrHalfBet + "\n";
						/* This is here to ensure that if the player wins their bet but lose their insurance
							only the win sound is played */
						if(PlayerWonBet != true)
						{
							emit PlayLoseSound();
						}
					}
				}
			}	
		}
		/* Player surrendered */
		else
		{
			ResultsSummary += "Surrendered\nHalf of bet of " + StrCountBet + " returned\n";
			emit PlayLoseSound();
		}
		/* Player returns cards to the dealer */
		CurrentBox->ReturnCards();
//		emit clearPlayersHand();

		// Test string
		//ResultsSummary = "Hand of blackjack in push with dealer's blackjack\nWin insurance bet and collect 1500 from the dealer\nWin bet and collect 1500 from the dealer\n";
		//ResultsSummary += "Win insurance bet and collect 1500 from the dealer";

		QString qstr = QString::fromStdString(ResultsSummary);
		UpdateResultsSummary(qstr);
		MySleep(3.0);
		if(CheckChoice() == 8)
		{
			return; 
		}
		// Send nothing to results summary to hide it
		UpdateResultsSummary("");
	}
}

void GameThread::ReturnBet(int ReturnedChipValue)
{
	qDebug() << "You returned a chip of value = " << ReturnedChipValue;
	Box* CurrentBox = BlackJackTable.GetBox(0);
	CurrentBox->ReturnToStack(ReturnedChipValue);
}

void GameThread::run()
{
	/* Create table object */
	//Table BlackJackTable;

	/* Seat new players at the table */
	SeatPlayers(BlackJackTable);

	/* Create a dealer object which generates a shoe of cards which are then shuffled */
	Croupier Dealer;

	connect(&Dealer, SIGNAL(updateDealersHand(QString, int)), this, SIGNAL(UpdateDealersHand(QString, int)));
	connect(&Dealer, SIGNAL(updateDealersHandValue(QString)), this, SIGNAL(UpdateDealersHandValue(QString)));

	/* Game begins and loops until all players go bust! */
	do
	{
		/* Collect bets from all players at the table*/
		CollectBets();

		if(CheckChoice() != 8)
		{
			emit UpdateBetValue("0");
			/* Deal out the inital two cards to every player and the dealer */
			InitialDeal(BlackJackTable, Dealer);
		
			/* If the dealer is showing an ace, and a player has enough money, offer insurance */
			InsuranceOffers(BlackJackTable, Dealer);

			/* Dealer checks for blackjack before continuing with game */		
			if(CheckForBlackJack(BlackJackTable, Dealer) == false)
			{
				/* Each player takes their turn (returns if players still in game after plays) */
				if(PlayersPlay(BlackJackTable, Dealer) != false)
				{
					/* If players are still in the game the Dealer plays his hand */
					DealerPlays(Dealer);
				}
				/* If all players go bust, display dealer's status */
				else
				{
					/*if(CheckChoice() == 8)
					{
					}
					else
					{*/
						int DealersHandVal = Dealer.CheckHand();
						emit UpdateGameStatus("Dealer was holding " + QString::number(DealersHandVal));
						Dealer.Status();
						MySleep(2);
					//}
				}
			}
		}

/***********************  DO SOMETHING ABOUT RETURNING CARDS AS WELL TO CLEAR BETS ETC *****************************/


		if(CheckChoice() != 8)
		{
			/* Work out who has won and who has lost and settle all bets */
			SettleBets(BlackJackTable, Dealer);
		}

		/* The dealer returns his cards */
		Dealer.ReturnCards();
		emit ClearDealersHand();
		emit ClearPlayersHand();

		/* Clear all boxes containing splits */
		BlackJackTable.RemoveSplits();
	
		if(CheckChoice() != 8)
		{
			/* Remove broke players from the table */
			RefreshPlayerList(BlackJackTable);
		}
		else
		{	
			BlackJackTable.Clear();
		}
		emit ClearChips();
	}
	while((BlackJackTable.CountBoxes() > 0) && (CheckChoice() != 8));

	if(CheckChoice() != 8)
	{
		emit UpdateGameStatus("Game Over");
		QString qstr = QString::fromStdString("You've run out of playable money!\nWould you like to play again?");
		UpdateResultsSummary(qstr);
	}
	ButtonChoice = 0;
}

