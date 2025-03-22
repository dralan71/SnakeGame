#include <iostream>
#include <ncurses.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// Function to detect if a key was hit (obtained online)
bool kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Get terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Set non-blocking read
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Check for key press
    ch = getchar();

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin); // Put character back in buffer
        return true;
    }

    return false;
}


using namespace std;
bool gameOver;
const int width = 40;
const int height = 20;
const int maxTailLength=50;
int x, y, fruitX, fruitY, score;
int tailX[maxTailLength], tailY[maxTailLength];
int tailSize;
enum direction {STOP=0, LEFT, RIGHT, UP, DOWN};
direction snakeDirection;


void Setup()
{
    gameOver=false;
    snakeDirection=STOP;
    x=width/2;
    y=height/2;
    srand(time(0));
    fruitX=rand()%width;
    fruitY= rand()%height;
    score=0;
    tailSize=0;
}
void Draw()
{
    system("clear");
    cout<<"score: "<<score<<endl;
    for (int i=0; i<width+2; i++){                          //prints top wall
        cout<<"#";
    }
    cout<<endl;

    for(int i=height-1; i>=0; i--){                         //prints map
        for(int j=0; j<width; j++){
            if(j==0) cout<<"#";
            if(i==y && j==x) cout<<"O";
            else if(i==fruitY && j==fruitX) cout<<"F";
            else{
                bool print=false;
                for(int k=0; k<tailSize; k++){              //prints tail
                    if(tailX[k]==j&& tailY[k]==i){
                        cout<<"o";
                        print=true;
                    }
                }
                if(!print) cout<<" ";
            } 
            if(j==width-1) cout<<"#";
        }
        cout<<endl;
    }

    for (int i=0; i<width+2; i++){                          //prints bottom wall
        cout<<"#";
    }
    cout<<endl;
}
void Input()
{
    if(kbhit())
    {
        switch (getchar())                                  //detects input
        {
            case 'a':
                snakeDirection=LEFT;
                break;
            case 'd':
                snakeDirection=RIGHT;
                break;
            case 's':
                snakeDirection=DOWN;
                break;
            case 'w':
                snakeDirection=UP;
                break;
            case 'x':
                gameOver=true;
        }
    }
}
void Logic()
{
    int prevX= x;
    int prevY=y;
    int prev2X, prev2Y;
    for(int i =0; i<tailSize;i++){                          //tail logic
        prev2X=tailX[i];
        prev2Y=tailY[i];
        tailX[i]=prevX;
        tailY[i]=prevY;
        prevX=prev2X;
        prevY=prev2Y;
    }
    switch (snakeDirection)                                 //snake movement
    {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y++;
            break;
        case DOWN:
            y--;
            break;
        default:
            break;
    }
    if(x>=width || x<0 || y>=height || y<0)                 //wall collision detection
    {
        gameOver=true;
    }
    for(int i=1; i<tailSize; i++){                          //tail collision detection
        if(x==tailX[i]&&y==tailY[i]){
            gameOver=true;
        }
    }
    if(x==fruitX&&y==fruitY)                                //Fruit eating and respawning logic
    {
        score+=10;
        tailSize++;
        fruitX=rand()%width;
        fruitY=rand()%height;
    }   

}

int main(){
    Setup();
    while(!gameOver){
        Draw();
        Input();
        Logic();
        //cout<<"Fruit position: ("<<fruitX<<", "<<fruitY<<")\n"; //Debuging output
        usleep(100000);                                     //100 ms pause
    }
    cout<<"\nGAME OVER\n";

    return 0;
}