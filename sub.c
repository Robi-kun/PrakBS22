#include "sub.h"

int sonderzeichen(char *string){
    for(int i = 0; i < strlen(string); i++) {
        if (!((int) string[i] >= 48 && (int) string[i] <= 57 || (int) string[i] >= 65 && (int) string[i] <= 90 || (int) string[i] >= 97 && (int) string[i] <= 122 || (int) string[i] == 32) ) //ASCII: 48-57, 65-90, 97-122, 32
            return 0;
    }
    return 1;
}