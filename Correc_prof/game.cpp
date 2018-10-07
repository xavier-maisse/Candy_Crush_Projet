#include <iostream>
#include <fstream>

#include "game.h"
#include "Correc_prof/params.h"
#include "Correc_prof/gridmanagement.h"
#include <algorithm>
#include <map>
using namespace std;
template <class T, class U>
void ShowMap (const map<T,U> & AMap){
    for (const pair <T,U> & Val : AMap)                cout << "Cle : " << Val.first << "   "             << "Valeur : " << Val.second << endl;
    cout << endl;
}


using namespace std;

void Menu (CPosition & Pos, char & Direction,CMyParam & params, bool & erreurlecture)
{
    erreurlecture=false;
    cout << "Saisir un indice de ligne, un indice de column,\n"
           << " et un sens de Deplacements : \n"<<
                                         params.MapParamChar["KeyLeft"] <<" a gauche,\n "<<
                                         params.MapParamChar["KeyDown"]<<" en bas,\n"<<
                                         params.MapParamChar["KeyRight"]<<" a droite,\n "<<
                                         params.MapParamChar["KeyUp"]<<" en haut\n"
         << endl;
    cout <<"cin Pos.first\n";
    cin >> Pos.first;
    if (cin.fail ())
    {
        erreurlecture=true;
        cout << "Problème dans l'indice de la ligne, Indice non valide"<< endl;
        cin.clear ();
        cin.get();
    }

    if (Pos.first > 9 || Pos.first < 0)cout << "Déplacement impossible, débordement de tableau \n";


    if (erreurlecture==false)
    {
        cout <<"cin Pos.second\n";
        cin>> Pos.second;
        if (cin.fail ())
        {
            erreurlecture=true;
            cout << "Problème dans l'indice de la colonne, Indice non valide" << endl;
            cin.clear ();
            cin.get();

        }
    }
    if (erreurlecture==false)
    {
       cout <<"cin Direction"<<endl;
       cin >> Direction;
       if (cin.fail () || Direction != params.MapParamChar["KeyUp"]
               && Direction != params.MapParamChar["KeyDown"]
               && Direction != params.MapParamChar["KeyRight"]
               && Direction != params.MapParamChar["KeyLeft"]  )
       {
           erreurlecture=true;
           cout << "Problème dans le choix de la direction, direction non valide" << endl;
           cin.clear ();
           cin.get();

       }
    }
} // Menu()


void LeaderBoard()
{
    ifstream ifs ("../M1103-ProjetNumberCrush/Nos_fichiers/leaderboard.txt");
    if (!ifs.is_open())
    {
        cerr << "pas le bon fichier de configuration";
        exit (2);
    }
    else
        cout << "Fichier configuration ok"<< endl;
    string Key;
    map <string,uint> LeaderBoardScore;

    while (ifs >> Key)
    {
        char tmp;
        ifs >> tmp;
        uint Val;
        ifs >> Val;
        cout << "Key = " <<Key << " Val = " << Val<< endl;
        LeaderBoardScore["Key"]=Val;
    }
}

void MakeAMove (CMat & Grid, const CPosition & Pos, const char & Direction,  CMyParam & Params, bool & erreurdirection)
{
    unsigned NewLineNumber (Pos.first), NewColumnNumber (Pos.second);
    if (Direction == Params.MapParamChar.find("KeyLeft")->second )
        if (Pos.second!=0) --NewColumnNumber;
        else erreurdirection=true;
    else if (Direction == Params.MapParamChar.find("KeyDown")->second)
        if (Pos.first < Params.MapParamUnsigned["GridSize"]-1) ++NewLineNumber;
        else erreurdirection=true;
    else if (Direction == Params.MapParamChar.find("KeyRight")->second)
        if (Pos.second < Params.MapParamUnsigned["GridSize"]-1) ++NewColumnNumber;
        else erreurdirection=true;
    else if (Direction == Params.MapParamChar.find("KeyUp")->second)
        if (Pos.first != 0) --NewLineNumber;
        else erreurdirection=true;
    else
        exit (EXIT_FAILURE);
    if (Grid [NewLineNumber][NewColumnNumber] == KImpossible || Grid [Pos.first][Pos.second] == KImpossible) erreurdirection=true;

    else swap (Grid [Pos.first][Pos.second], Grid [NewLineNumber][NewColumnNumber]);
} // MakeAMove ()

bool AtLeastThreeInARow (const CMat & Mat, CPosition & Pos, unsigned & HowMany)
{
    for (unsigned LineNumber = 0; LineNumber < Mat.size (); ++LineNumber)
    {
        for (unsigned ColNmber = 0; ColNmber < Mat.size () -2; ++ColNmber)
        {
            if (KImpossible == Mat[LineNumber][ColNmber]) continue;
            for (HowMany = 1; ColNmber + HowMany < Mat.size() && Mat [LineNumber][ColNmber] == Mat[LineNumber][ColNmber + HowMany]; ++HowMany);
            if (HowMany > 2)
            {
                Pos = make_pair (LineNumber, ColNmber);
                return true;
            }
        }
    }
    return false;
} // AtLeastThreeInARow ()

bool AtLeastThreeInAColumn (const CMat & Mat, CPosition & Pos, unsigned & HowMany)
{
    for (unsigned ColNmber = 0; ColNmber < Mat.size (); ++ColNmber)
    {
        for (unsigned LineNumber = 0; LineNumber < Mat.size () -2; ++LineNumber)
        {
            if (KImpossible == Mat[LineNumber][ColNmber]) continue;
            for (HowMany = 1; LineNumber + HowMany < Mat.size() && Mat [LineNumber][ColNmber] == Mat[LineNumber + HowMany][ColNmber]; ++HowMany);

            if (HowMany > 2)
            {
                Pos = make_pair (LineNumber, ColNmber);
                return true;
            }
        }
    }
    return false;
} // AtLeastThreeInAColumn ()

void RemovalInColumn (CMat & Mat, const CPosition & Pos, const unsigned & HowMany)
{
    unsigned Nb (0);
    for (; Pos.first + Nb + HowMany < Mat.size (); ++Nb)
    {
        Mat [Pos.first + Nb][Pos.second] = Mat [Pos.first + Nb + HowMany][Pos.second];
        Mat [Pos.first + Nb + HowMany][Pos.second] = KImpossible;
    }
    for (; Pos.first + Nb < Mat.size (); ++Nb)
        Mat [Pos.first + Nb][Pos.second] = KImpossible;
} // ReplaceColumn()

void RemovalInRow (CMat & Mat, const CPosition & Pos, const unsigned & HowMany)
{
    CPosition PosToRemove (Pos);
    for (unsigned i (0); i < HowMany; ++i)
    {
        RemovalInColumn (Mat, PosToRemove, 1);
        ++PosToRemove.second;
    }
} // ReplaceRow ()

unsigned ComputeScore (const unsigned & HowMany)
{
    return HowMany * (HowMany + 1) / 2;
}

void AffichFich()
{
    ifstream ifs ("../M1103-ProjetNumberCrush/Nos_fichiers/menu.txt");
    string Ligne;
    while (true)
    {
        getline (ifs, Ligne);
        if (ifs.eof()) break;
        cout << Ligne << endl;
    }
}

int ppal ()
{
    LeaderBoard();


    srand (time (NULL));
    cout << "Welcome to number crush!" << endl;
    CMat Grid;
    CMyParam params;
    InitParams(params);
    LoadParams(params);

    AffichFich();

    InitGrid (Grid, params.MapParamUnsigned["GridSize"]);
    CPosition Pos;
    unsigned HowMany, Score (0);
    char Dirrection;
    bool erreurdirection=false;
    for (unsigned Count (0); Count < KMaxTimes; ++Count)
    {
        bool erreurlecture=false;

        DisplayGrid (Grid,params);
        if (erreurdirection==true)
        {
            cout << "==========================================================="<<endl
                 << "   Erreur de Déplacement, ce déplacement est interdit !    "<<endl
                 << "==========================================================="<<endl;
            erreurdirection=false;
        }
        Menu (Pos, Dirrection, params,erreurlecture);
        if (erreurlecture)
        {
            --Count;
            continue;
        }
//        if ('q' == Dirrection) break;

        MakeAMove (Grid, Pos, Dirrection, params, erreurdirection);

        while (AtLeastThreeInAColumn (Grid, Pos, HowMany))
        {
            RemovalInColumn (Grid, Pos, HowMany);
            Score += ComputeScore (HowMany);
        }
        while (AtLeastThreeInARow (Grid, Pos, HowMany))
        {
            RemovalInRow (Grid, Pos, HowMany);
            Score += ComputeScore (HowMany);
        }
    }
    cout << "Your score is : " << Score << endl << "That's not so bad for a dummy ;)" << endl;
    return 0;
}
