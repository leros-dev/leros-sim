/*
4:268435455
*/

int main()
{
    int a = 0b10101111111111111111111111111111;

    a <<= 3; // MSB is 0
    a >>= 3; // right shift, expend positive (0b0) sign extension

    return a; // return in r4
}

