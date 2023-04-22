#include <string>

using namespace std;

class Ts {
    public:
        Ts(string apiKey, string chatId);

    private:
        string _chatId;
        string _apiKey;
};
