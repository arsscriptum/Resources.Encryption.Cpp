#include "resource.h"
#include "nowarns.h"
#include <Windows.h>

#include <string>
#include <string_view>
#include <vector>
#include <strsafe.h>

#include <iostream>
#include <algorithm>
#include <iterator>
#include <memory>
#include "Stream.h"
#include "Crypt.h"
#include "Base64.h"
#include "Decoder.h"
#include <iostream>
#include <cstddef>
#include <algorithm>
#include <vector>

using namespace std;

const char gPassword[] = "TooManySecrets_";
#pragma  warning( disable: 4840 )
#pragma  warning( disable: 4477 )
int main(int argc, char* argv[]) {

    printf("IDR_STRING1 %s\n", DECRYPT_RES(IDR_STRING1));
    printf("IDR_STRING2 %s\n", DECRYPT_RES(IDR_STRING2));
    return 0;
}