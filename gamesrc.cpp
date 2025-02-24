#include <cstdlib>
#include <iostream>
#include <time.h>
#include <string>
#include <windows.h>


using namespace std;

bool check_Ans1(string Ans1){               /* do u want to play? */ 
    for(int i = 0; i < Ans1.length(); i++){
        Ans1[i] = tolower(Ans1[i]);
    }
    if(Ans1 == "yes" || Ans1 == "y") return 1;
    // else if(Ans1 == "no" || Ans1 == "n") return 0;
    return 0;
}
bool check_Ans2(string Ans2){               // Big/Small?
    for(int i = 0; i < Ans2.length(); i++){
        Ans2[i] = tolower(Ans2[i]);
    }
    if(Ans2 == "big" || Ans2 == "b") return 1;
    // else if(Ans1 == "no" || Ans1 == "n") return 0;
    return 0;
}
void randomNum (int &D1, int &D2, int &D3, int &Sum, bool &result){ // tạo tình huống
    D1 = rand() % 6 + 1;
    D2 = rand() % 6 + 1;
    D3 = rand() % 6 + 1;
    Sum = D1 + D2 + D3;
    if(Sum > 10) result = true;
    else result = false;
}
void print (int x){             // in số/kết quả.
    cout << x << endl;
}
void print_Ques1 (bool const check){                  // này mới có yes/no
    if(check == true) {
        cout << "Game Rules: \n You have a certain number of guesses, guessing whether the sum of the dice is big or small. \n Big is when their sum is greater than 10, and small is when it is 10 or less. \n If you guess correctly, the reward will be paid to you." << endl;
        cout << "Do you want to play?" << endl;
    }
    else cout << "Continue?" << endl;
}
void print_Ques2 (){
    cout << "Please choose Big or Small?" << endl;
}
bool Result (bool BigSmall, bool Sum){
    if(BigSmall == Sum) return true;
    else return false;
}
void print_Result (bool const RE, int &times){
    if(RE == 1) {
        times ++;
        cout << "Congratulation! You win this turn ^.^" << endl;
        cout << "Now you have: " << times << " turns." << endl;
    }
    else {
        times --;
        cout << "Sorry, you lost that turn T-T" << endl;
        cout << "Now you have: " << times << " turns." << endl;
    }
}
void wait_Result(int D1, int D2, int D3, int Boxing){
    cout << "OK. Now check the Dicing Scores:" << endl;
    Sleep(2000);
    cout << "The first dice scores: " << D1 << endl;
    Sleep(2000);
    cout << "The second scores: " << D2 << endl;
    Sleep(2000); 
    cout << "And the last one scores: " << D3 << endl;
    cout << "Summary, we have the sum is: " << Boxing << endl;
}
    

int main(){
    srand(time(0));
    int Dice1 = 0, Dice2 = 0, Dice3 = 0;
    int Boxing = 0;
    bool Big_Small = true;
    int Your_times_playing = 10;
    
    string YN_Ans;
    string BS_Ans;

    bool tmp = 1;
    do{
        randomNum(Dice1, Dice2, Dice3, Boxing, Big_Small);
        print_Ques1(tmp);
        tmp = 0;
        cin >> YN_Ans;
        if(check_Ans1(YN_Ans)){
            print_Ques2();
            cin >> BS_Ans;
            bool Last = Result(check_Ans2(BS_Ans), Big_Small);
            wait_Result(Dice1, Dice2, Dice3, Boxing);
            print_Result(Last, Your_times_playing);
        }
        
    }
    while(check_Ans1(YN_Ans));

}