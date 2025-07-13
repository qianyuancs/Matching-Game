#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<windows.h>

#define SIZE 4
#define PAIRS 8                //牌的对数
#define TIME  60              //游戏时间限制（60s)
#define MAX_PLAYERS 100       //最多的游戏排行榜数 
#define RANK_FILE "rank.txt"   //排行榜文件（存储玩家信息） 


//排行榜结构体 
typedef struct {
    char name[50];
    int score;
    int rank;
    double time;  // 单位：秒
} Player;

Player players[MAX_PLAYERS];
int player_count = 0;


// 比较函数：先按 score 降序，score 相同按 time 升序
int cmp_player(const void *a, const void *b) {			//const void 接受任意类型数据的地址，不能修改（qsort函数的要求） 
    Player *pa = (Player *)a;		//这里写了一个排序规则，方便下面的qsort函数调用 
    Player *pb = (Player *)b;
    if (pa->score != pb->score)
        return pb->score - pa->score;       // 高分在前 
    else
        return (pa->time > pb->time) ? 1 : -1;  // 用时少的在前
} 													


//把排行榜信息存储在文件里 
void save_ranking_to_file() {
    FILE *fp = fopen(RANK_FILE, "w");
    if (!fp) {
        printf("无法写入排行榜文件！\n");
        return;
    }

    for (int i = 0; i < player_count; i++) {
        fprintf(fp, "%s %d %.2f\n",
                players[i].name,
                players[i].score,
                players[i].time);
    }

    fclose(fp);
}

// 更新所有玩家的 rank 字段
void update_ranks() {
    qsort(players, player_count, sizeof(Player), cmp_player);    //用于按照cmp_player的排序方法快速排序 
    for (int i = 0; i < player_count; i++) {						//直接修改了原数组 
        players[i].rank = i + 1;   					//从数组0变成排行1 
    }
    save_ranking_to_file();     //保存到文件里面去 
}


//每次输入游戏id进行比较 
void add_or_update_player_by_data(const char *name_input, int score_input, double time_input) {
    int found = 0;
    for (int i = 0; i < player_count; i++) {
        if (strcmp(players[i].name, name_input) == 0) {
            // 是同一个玩家
            found = 1;
            // 记录新旧数据来比较 
            int old_score = players[i].score;
            double old_time = players[i].time;

            int is_new_better = 0;

            if (score_input > old_score) {
                is_new_better = 1;
            } else if (score_input == old_score && time_input < old_time) {
                is_new_better = 1;
            }

            if (is_new_better) {               //如果同一个玩家的成绩更好 就更新他的成绩 
                players[i].score = score_input;
                players[i].time = time_input;
                printf("玩家 %s 的记录已更新为更好的成绩。\n", name_input);
            } else {
                printf("玩家 %s 的原成绩更好，未进行更新。\n", name_input);   
            }

            break;
        }
    }
		if (!found) {       //如果没找到就是新玩家，添加新玩家 
        if (player_count >= MAX_PLAYERS) {
            printf("已达到最大玩家数 %d，无法添加新玩家。\n", MAX_PLAYERS);
            return;
        }
        // 添加新玩家
        strcpy(players[player_count].name, name_input);
        players[player_count].score = score_input;
        players[player_count].time = time_input;
        player_count++;
        printf("已添加新玩家 %s。\n", name_input);
    }

    update_ranks();  // 不论是否更新都需要重新排序以保持排名准确/逻辑更简单 
}


// 查询指定名字的玩家
void query_player() {
    char target[50];
    printf("请输入要查询的游戏id：");
    scanf("%49s", target);		//安全的输入方式 

    int found = 0;
    for (int i = 0; i < player_count; i++) {
        if (strcmp(players[i].name, target) == 0) {
            printf("找到玩家：%s  分数：%d  用时：%.2f  名次：%d\n",
                   players[i].name,
                   players[i].score,
                   players[i].time,
                   players[i].rank);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("未找到名为 \"%s\" 的玩家。\n", target);
    }
}


// 显示所有玩家
void display_all() {
    if (player_count == 0) {
        printf("当前没有任何记录。\n");
        return;
    }
    update_ranks();
	printf("%-6s %-10s %-5s %-10s\n", "名次", "名字", "分数", "用时(秒)");
    printf("-------------------------------------------------\n");
    for (int i = 0; i < player_count; i++) {
        printf("%-6d %-10s %-5d %-10.2f\n",
               players[i].rank,
               players[i].name,
               players[i].score,
               players[i].time);
    }
}

//排行榜的管理，与玩家的交互 
int Leaderboard() {
    int choice;
    do {
        printf("\n===== 排行榜管理 =====\n");
        printf("0. 退出\n");
        printf("1. 查询玩家\n");
        printf("2. 展示全部排行榜\n");
        printf("请选择（0-2）：");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                query_player();
                break;
            case 2:
                display_all();
                break;
            case 0:
                printf("退出程序。\n");
                break;
            default:
                printf("无效选项，请重新输入（0-2）。\n");
        }
    } while (choice != 0);

    return 0;
}


//把排行榜信息加载到内存里面 
void load_rank(){
	FILE *fp=fopen(RANK_FILE,"r");
	if(!fp) return;
	player_count=0;
    while (fscanf(fp, "%s %d %lf", players[player_count].name,
                  &players[player_count].score,
                  &players[player_count].time) == 3) {
        player_count++;
    }

    fclose(fp);
    update_ranks();
} 

//卡牌结构体设计和全局化
typedef struct{
    int Isopen;              //记录牌面当前有没有被翻开
    int Ismatched;       //记录牌面当前有没有匹配
    int value;              //记录牌面值
}card;

card cards[SIZE][SIZE];     // 牌面数组地图
int score=0;              //玩家的得分（初始是0）
time_t startime;           //游戏开始的时间



//卡牌初始化+洗牌
void  shuffle(char *values, int size){       //洗牌函数
    for(int i=size-1;i>0;i--){
        srand(time(NULL));            //设置随机种子，让rand这个伪随机数变成真的随机数
        int j=rand()%(i+1);           //生成0-i的随机数
        char temp=values[i];
        values[i]=values[j];
        values[j]=temp;               //交换位置
    }
}

void init_cards(){          //初始化牌面（设置牌面为“A”-“H")
    char values[PAIRS*2];
    for(int i=0;i<PAIRS;i++){
        values[2*i]='A'+i;
        values[2*i+1]='A'+i;
    }

    shuffle(values,PAIRS*2);     //洗牌

    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            cards[i][j].Isopen=0;           //初始化为背面 
            cards[i][j].Ismatched=0;        //初始化为未匹配
            cards[i][j].value=values[i*SIZE+j];    //把洗好的牌面赋值给地图
        }
    }
}


void clear_board(){
    system("cls");       //清屏
}


//显示牌面
void showboard(){
    clear_board();          //清屏   防止牌面内容堆积
    for(int i=0;i<SIZE;i++){
        printf("   %d     ",i+1);    //行标
    }
    printf("\n");
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            printf("  ┏━━━┓ ");
        }
        printf("\n");
        printf("%d ",i+1);          //列标
        for(int j=0;j<SIZE;j++){
            char background='*';         //背面显示 *
            if(cards[i][j].Ismatched){
                background=' ';        //如果匹配了，就显示空格
            }else{
                if(cards[i][j].Isopen){
                    background=cards[i][j].value;        //如果翻开了就显示牌面值
                }
            }
            printf("┃ %c ┃   ",background);
        }
        printf("\n");
        for(int k=0;k<SIZE;k++){
            printf("  ┗━━━┛ ");
        }
        printf("\n");
    }

    printf("玩家当前得分为：%d\n",score);

}


//显示记忆时间的倒计时 
void countdown(int sec){
	while(sec--){
		printf("\r 记忆时间还剩%d秒",sec);
		fflush(stdout);
		Sleep(1000);
	}
	printf("\n");
} 



//显示游戏倒计时 
void game_time(int sec){
	while(sec--){
		printf("\r游戏时间还剩%d秒",sec);
		fflush(stdout);
		Sleep(1000);
	}
	printf("\n");
}




//游戏开始——15s记忆牌面
void memory_game(){
    clear_board();
    printf("游戏开始！你有15s的时间来记忆以下牌面！\n");
    init_cards();         //初始化牌面
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            cards[i][j].Isopen=1;     //将所有牌都翻开
        }
    }
    showboard();         //显示牌面
	countdown(15);      //显示倒计时 

    clear_board();
    printf("时间到！请开始匹配牌面！\n");
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            cards[i][j].Isopen=0;           //把所有的牌全部翻回去
        }
    }
    clear_board();
    showboard();           //再次显示牌面（背面）等待玩家输入坐标

}


//玩家输入坐标
void player_input(int *x1,int *y1,int *x2,int *y2){
    printf("请输入第一张牌的坐标（行 列）：");     //提示玩家输入坐标
    scanf("%d %d",x1,y1);
    (*x1)--;        //将输入的坐标转换为数组索引
    (*y1)--;

    printf("请输入第二张牌的坐标（行 列）：");
    scanf("%d %d",x2,y2);
    (*x2)--;        //将输入的坐标转换为数组索引
    (*y2)--;

    //检查输入是否合法
    if(*x1<0 || *x1>=SIZE || *y1<0 || *y1>=SIZE || *x2<0 || *x2>=SIZE || *y2<0 || *y2>=SIZE ||
         cards[*x1][*y1].Ismatched || cards[*x2][*y2].Ismatched || (*x1==*x2 && *y1==*y2) ||
        cards[*x1][*y1].Isopen || cards[*x2][*y2].Isopen){      //超过范围或者坐标相等或者已经翻开有匹配都是非法的
        printf("输入的坐标不合法，请重新输入！\n");
        player_input(x1,y1,x2,y2);      //递归调用，直到输入合法
    }

    cards[*x1][*y1].Isopen=1;
    cards[*x2][*y2].Isopen=1;        //翻开两张牌
    clear_board();
    showboard();                 //让玩家看看自己的牌
    Sleep(2000);     //看2s
}


//判断牌面是否匹配
void is_match(int x1,int y1,int x2,int y2){
    if(cards[x1][y1].value==cards[x2][y2].value){
        cards[x1][y1].Ismatched=1;
        cards[x2][y2].Ismatched=1;          //匹配成功就改变value的值
        score ++;
        printf("匹配成功！");
    }else{
        cards[x1][y1].Isopen=0;
        cards[x2][y2].Isopen=0;             //匹配失败就翻回去
        printf("匹配失败！");
    }
    clear_board();
    showboard();          //更新牌面
}

    //游戏前导词
void pre_game(){
    printf("欢迎来到记忆配对游戏！\n");
    printf("\n");
    printf("游戏规则：\n");
    printf("\n");
    printf("1. 游戏开始后，你有15秒的时间来记忆牌面\n");
    printf("\n");
    printf("2. 记忆时间结束后， 你需要在60秒的时间内匹配所有牌面\n");
    printf("\n");
    printf("3. 每次输入两张牌的坐标（行 列），如果匹配成功得1分，失败不计分\n");
    printf("\n");
    //printf("请输入你的游戏id(任意字符作为你的id计入排行榜)\n");
    //static char name[20];
    //scanf("%s",name);
}


    //游戏主循环
int main(int argc,char* argv[]){
	load_rank();         //把排行榜信息加载到内存里 
    pre_game();              //游戏前导词
    printf("请输入你的游戏id(任意字符作为你的id计入排行榜)\n");
    char name[20];
    scanf("%s",name);
    memory_game();          //开始游戏，让玩家15秒记忆牌面
    printf("计时开始！请在60秒内匹配所有牌面！\n");
    time_t start_time=time(NULL);          //记录游戏开始的时间
    
    while(1){  
    	time_t now=time(NULL);
		int elasped=(int)(now-start_time);             //计算已用的时间
		int remaining=TIME-elasped; 					//计算剩余时间 
        if(remaining<=0){
        	printf("时间到！游戏结束！你最后的得分是%c\n",score);
        	printf("感谢你的参与！\n");
        	add_or_update_player_by_data(name,score, 60);
        	Leaderboard();
        	break;
		} 		     		
		printf("\r剩余时间%d秒\n",remaining);
		
        int x1,y1,x2,y2;
        player_input(&x1,&y1,&x2,&y2);       //让玩家输入坐标
        is_match(x1,y1,x2,y2);            //判断是否匹配
        
	    if(score ==PAIRS){
    	time_t end_time=time(NULL);
		int sum_time=end_time-start_time;     //计算游戏使用的总时间 
        clear_board();
        printf("恭喜你！成功匹配了所有牌面！\n");
        printf("游戏结束！你的得分是：%d\n",score);
        if(sum_time>60){
        	sum_time=60;
		} 
        add_or_update_player_by_data(name,score, sum_time);
        Leaderboard();
        exit(0);       //如果匹配完了，就直接退出游戏
    	}
    }

    return 0;
}
