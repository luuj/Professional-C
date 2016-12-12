#include "DecryptPasswords.h"

DecryptPasswords::DecryptPasswords(char* pF, std::unordered_map<std::string, std::string> inMap)
{
    DecryptPasswords::passwordFile = pF;
    DecryptPasswords::dictionaryMap = inMap;
}

DecryptPasswords::~DecryptPasswords(){}

void DecryptPasswords::decrypt()
{
    std::ifstream infile(passwordFile);
    char passwordInput[41];
    std::unordered_map<std::string, std::string>::iterator it;
    int index = 0;
    
    //Decrypt passwords and store them in solved/unsolved buckets
    while (infile >> passwordInput)
    {
        it = dictionaryMap.find(passwordInput);
        
        //Entry not found -- place in unsolved bucket
        if (it == dictionaryMap.end())
        {
            Password newPass(passwordInput, "??", index);
            unsolvedPasswords.push_back(newPass);
        }
        //Entry found -- place in solved bucket
        else
        {
            Password newPass(passwordInput, it->second, index);
            std::pair<int, Password> foundPass (index, newPass);

            solvedPasswords.insert(foundPass);
        }
        
        index++;
    }

    infile.close();
}

void DecryptPasswords::bruteForceDecrypt()
{
    //Start timer
    Timer timer;
    timer.start();

    tbb::parallel_invoke(
        [this] { bruteForceHelper(0, 208240); },
        [this] { bruteForceHelper(208241, 416481);},
        [this] { bruteForceHelper(416482, 624722);},
        [this] { bruteForceHelper(624723, 832963);},
        [this] { bruteForceHelper(832964, 1041204);},
        [this] { bruteForceHelper(1041205, 1249445);},
        [this] { bruteForceHelper(1249446, 1457686);},
        [this] { bruteForceHelper(1457687, 1665927);},
        [this] { bruteForceHelper(1665928, 1874161);}
    );
    
    //Stop timer
    double elapsed = timer.getElapsed();
    std::cout << "Brute-force time: " << elapsed << " sec" << std::endl;
}

void DecryptPasswords::bruteForceHelper(int start, int end)
{
    int countingMachine = start;
    char* testString;
    char testStringHash[41];
    unsigned char hash[20];
    Password currentPass;

    while (countingMachine < end)
    {
        //Create permutations of passwords
        testString = const_cast<char*>(convertBase36(countingMachine).c_str());
        countingMachine++;
        sha1::calc(testString, strlen(testString), hash);
        sha1::toHexString(hash, testStringHash);
        
        for (auto it = unsolvedPasswords.begin(); it != unsolvedPasswords.end(); it++)
        {
            currentPass = *it;
            
            //Hash permuted passwords and compare to hash of unsolved password
            if (currentPass.hash.compare(testStringHash) == 0)
            {
                Password newPass(currentPass.hash, testString, currentPass.index);
                *it = newPass;
            }
        }
    }
}

bool DecryptPasswords::checkFileExists()
{
    struct stat buffer;
    return (stat (passwordFile, &buffer) == 0);
}

std::string DecryptPasswords::convertBase36(int val)
{
    std::string legend = " abcdefghijklmnopqrstuvwxyz0123456789";
    std::string convertedNum;
    convertedNum.reserve(4);
    
    do
    {
        convertedNum = legend[val%37] + convertedNum;
    }
    while(val /= 37);
    
    return convertedNum;
}

void DecryptPasswords::writeFile()
{
    //Add unsolved passwords to final list
    for (int i=0; i<unsolvedPasswords.size(); i++)
    {
        std::pair<int, Password> foundPass (unsolvedPasswords.at(i).index, unsolvedPasswords.at(i));
        solvedPasswords.insert(foundPass);
    }
    
    std::ofstream outfile("password_results.txt");
    typedef std::map<int, Password>::iterator it_type;
    Password currPassword;
    
    for (it_type iterator = solvedPasswords.begin(); iterator != solvedPasswords.end(); iterator++)
    {
        currPassword = iterator->second;
        outfile << currPassword.hash;
        outfile << "," << currPassword.original << "\n";
    }
    
    outfile.close();

}