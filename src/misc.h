//
//  misc.h
//  
//
//  Created by kannolab1 on 2016/08/02.
//
//

#ifndef misc_h
#define misc_h

int32_t atoi(char *n) {
    int32_t intID = 0;
    int digit = strlen(n);
    
    for(int i=0;i<digit;i++) {
        int mul = 1;
        for(int j=0; j<i;j++) {
            mul *= 10;
        }
        intID += (n[digit-1-i] - '0')*mul;
    }
    return intID;
}

#endif /* misc_h */
