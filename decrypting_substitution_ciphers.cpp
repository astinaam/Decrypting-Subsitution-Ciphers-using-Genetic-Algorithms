#include<bits/stdc++.h>

using namespace std;

#define sz(x) (int)x.size()
#define dbg2(a,b) cerr<<#a<<"="<<a<<","<<#b<<"="<<b<<endl

#define POPULATION_SIZE 100
#define CHROMOSOME_SIZE 26
#define TOURNAMENT_SIZE 20
#define ELITISM 30
#define NUMBER_OF_CROSSOVER_POINTS 5
#define NGRAM_SIZE 3
#define MUTATION_PROBABILITY 0.20

const string CHROMOSOME = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

string Population[POPULATION_SIZE];
string test_file;

map<string,long long>ngrams_train,ngrams_test;

void genarate_population()
{
    // to check if its taken earlier
    map<string,int>isChoosen;
    // genrates population
    for(int p=0;p<POPULATION_SIZE;++p)
    {
        string chromosome = CHROMOSOME;
        random_shuffle(chromosome.begin(),chromosome.end());
        while(isChoosen.count(chromosome))
        {
            random_shuffle(chromosome.begin(),chromosome.end());
        }
        isChoosen[chromosome] = 1;
        Population[p] = chromosome;
    }
}

void load_ngram_frq(string file_name)
{
    ifstream in(file_name);
    string cs,ctr="";
    string current;
    while(getline(in,cs))
    {
        ctr += cs;
    }
    in.close();
    for(int i=0;i<sz(ctr);++i)
    {
        char c = ctr[i];
        current.push_back(c);
        if(sz(current) == NGRAM_SIZE)
        {
            transform(current.begin(), current.end(), current.begin(), ::toupper);
            ngrams_train[current]++;
            current = current.substr(1,NGRAM_SIZE);
        }
    }
    //cerr<<"log:: loaded train ngram"<<endl;
}

// calculate ngrams of the test string for a key
void calculate_ngrams_of_test_string(string decrypted_test_string)
{
    ngrams_test.clear();
    string current;
    for(int i=0;i<sz(decrypted_test_string);++i)
    {
        char c = decrypted_test_string[i];
        if(c == ' ' || ispunct(c)) continue;
        if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') )
        {
            current.push_back(c);
            if(sz(current) == NGRAM_SIZE)
            {
                ngrams_test[current]++;
                current = current.substr(1,NGRAM_SIZE);
            }
        }
    }
}
// decrypt the test string with a key and capitalize
string decrypt_test_string(string key)
{
    // mapping the letters
    map<char,char>character_map;
    for(int i=0;i<sz(key);++i)
    {
        character_map[key[i]] = char('A' + i);
    }
    ifstream test_input(test_file);
    string ts,test_string = "";
    while(getline(test_input,ts))
    {
        transform(ts.begin(),ts.end(),ts.begin(), ::toupper);
        test_string += ts;
    }
    test_input.close();
    // decrypt the test string
    for(int i=0;i<sz(test_string);++i)
    {
        if((test_string[i] >= '0' && test_string[i] <= '9') || (test_string[i] >= 'A' && test_string[i] <= 'Z') || (test_string[i] >= 'a' && test_string[i] <= 'z') )
        {
            test_string[i] = character_map[test_string[i]];
        }

    }
    return test_string;
}

// n-gram frequency based fitness function
// Equation : SUM 1 to N(Fp(x,y)*log2(Ft(y))
double fitness(string key)
{
    string decrypt = decrypt_test_string(key);
    calculate_ngrams_of_test_string(decrypt);
    double fitness_value = 0;
    for(auto x : ngrams_test)
    {
        long long train_value = 0;
        if(ngrams_train.count(x.first))
        {
            train_value = ngrams_train[x.first];
            fitness_value += x.second * log2(train_value);
        }
    }
    return fitness_value;
}

string tournament_selection()
{
    string best;
    for(int i=0;i<TOURNAMENT_SIZE;++i)
    {
        int random = rand() % POPULATION_SIZE;
        string current = Population[random];
        if(sz(best)==0 || fitness(current) > fitness(best))
        {
            best = current;
        }
    }
    return best;
}
// Crossover using Position Based Crossover
// Given the two parents produced by the selection process
// Choose up to k points of crossover
// Copy the values at those k points from the first parent into the same k points in the first child
// Then iterate through the remaining values from the second parent filling in the missing
// - values in the first child
// To generate the second child follow the same process except reverse the parents
pair<string,string> crossover(string parent1, string parent2)
{
    // fill with '#'
    string child1(CHROMOSOME_SIZE,'#'), child2(CHROMOSOME_SIZE,'#');
    // select random crossover points of defined size
    vector<int>crossover_points;
    map<int,int>isThere; // alreday choosen or not
    while(sz(crossover_points) < NUMBER_OF_CROSSOVER_POINTS)
    {
        // any 0 - 25 string postions
        int random = rand() % 26;
        if(!isThere.count(random))
        {
            crossover_points.push_back(random);
            isThere[random]++;
        }
    }
    // child1
    map<char,int>isTaken;
    for(int i=0;i<sz(crossover_points);++i)
    {
        child1[crossover_points[i]] = parent1[crossover_points[i]];
        isTaken[child1[crossover_points[i]]]++;
    }
    int index = 0;
    for(int i=0;i<CHROMOSOME_SIZE;++i)
    {
        if(child1[i] == '#')
        {
            while(isTaken.count(parent2[index]))
            {
                index++;
            }
            child1[i] = parent2[index];
            index++;
        }
    }
    // child2
    isTaken.clear();
    for(int i=0;i<sz(crossover_points);++i)
    {
        child2[crossover_points[i]] = parent2[crossover_points[i]];
        isTaken[child2[crossover_points[i]]]++;
    }
    index = 0;
    for(int i=0;i<CHROMOSOME_SIZE;++i)
    {
        if(child2[i] == '#')
        {
            while(isTaken.count(parent1[index]))
            {
                index++;
            }
            child2[i] = parent1[index];
            index++;
        }
    }
    //cerr<<"log:: crossover done"<<endl;
    return make_pair(child1,child2);
}
// Mutaion using Swap Mutation Operator
// Randomly select 2 differnt positions and swaping them
string mutation(string child)
{
    double mutatble = rand()%100 + 1;
    mutatble /= 100.0;
    //dbg2(mutatble , MUTATION_PROBABILITY);
    if(mutatble < MUTATION_PROBABILITY) return child;
    int pos1 = rand() % 26;
    int pos2 = rand() % 26;
    // avoid duplication
    while(pos1 == pos2) pos2 = rand() % 26;
    // swap the positions
    char tmp = child[pos1];
    child[pos1] = child[pos2];
    child[pos2] = tmp;
    //cerr<<"log:: mutation done"<<endl;
    return child;
}

bool choose_best(string a, string b)
{
    return fitness(a) > fitness(b);
}

int main(int argc, char **argv)
{
    // cli
    string train;
    if(argc < 3 && argc > 1)
    {
        cerr<<"USAGE : train_file_name test_file_name"<<endl;
        return 0;
    }
    else if(argc == 3)
    {
        train = argv[1];
        test_file = argv[2];
    }
    else
    {
    cout<<"Train File name without spaces : ";
    cin>>train;
    cout<<"Test File Name without spaces : ";
    cin>>test_file;
    //train = "train.txt";
    //test_file = "test.txt";
    }
    // initialize random seed
    srand (time(NULL));

    cerr<<test_file<< " " << train<<endl;

    genarate_population();
    load_ngram_frq(train);

    bool found = false;

    //freopen("data.txt","w",stdout);

    string bkey,btext,prev_best;
    double best_sol = 0.0;
    long long gen = 1;
    int persistent = 0;
    while(found == false)
    {
        string Childs[POPULATION_SIZE];
        int cnt = 0;
        sort(Population,Population+POPULATION_SIZE,choose_best);
        for(cnt=0;cnt<ELITISM;++cnt)
        {
            Childs[cnt] = Population[cnt];
        }
        while(cnt < POPULATION_SIZE)
        {
            string parent1 = tournament_selection();
            string parent2 = tournament_selection();
            //cerr<<cnt<<" "<<parent1<<" "<<parent2<<endl;
            auto childs = crossover(parent1,parent2);
            Childs[cnt] = mutation(childs.first);
            cnt += 1;
        }
        for(int i=0;i<POPULATION_SIZE;++i)
        {
            //cerr<<i<<" "<<Childs[i]<<endl;
            Population[i] = Childs[i];
            double fit = fitness(Population[i]);
            if(fit > best_sol)
            {
                prev_best = bkey;
                best_sol = fit;
                bkey = Population[i];
                btext = decrypt_test_string(Population[i]);
            }
        }
        if(prev_best == bkey)
            persistent++;
        else
            persistent = 0;
        cout<<"Generation : "<<gen++<<" KEY : "<<bkey<<" FITNESS : "<<best_sol<<endl;
        cout<<decrypt_test_string(bkey)<<endl;
        if(persistent >= 50)
        {
            found = true;
        }
    }
}
