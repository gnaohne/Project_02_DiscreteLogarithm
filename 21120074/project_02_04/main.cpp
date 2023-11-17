#include<iostream>
#include<bitset>
#include<string>
#include<fstream>
#include<algorithm>
#include<chrono>

#define MAX_SIZE 514 // 1 bit sign and 1 bit to overflow

using namespace std;

//string input_file_name = "test_19.inp";
//string output_file_name = "test_00.out";

bitset<4> hex_char_to_bin(char c);
bitset<MAX_SIZE> hex_str_to_bin(const string& hex);
bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
string binary_to_hex_little_endian(const bitset<MAX_SIZE>& binary);
bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> mul_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& y, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> modulo(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> inverse_mod(const bitset<MAX_SIZE>& s, const bitset<MAX_SIZE>& p);// cal s^-1 in s.s^-1 = 1 (mod p)

int main(int argc, char* argv[])
{
    //auto start = std::chrono::high_resolution_clock::now();

    if (argc != 3)
    {
        cerr << "Usage: $ ./main test_XX.inp test_XX.out" << endl;
        return 1;
    }

    ifstream input_file(argv[1]);

    //ifstream input_file(input_file_name);

    if (!input_file.is_open())
    {
        cout << "Can't open file " << argv[1] << endl;
        return 1;
    }

    string p_hex, g_hex, y_hex, m_hex, r_hex, h_hex;

    getline(input_file, p_hex, '\n');
    getline(input_file, g_hex, '\n');
    getline(input_file, y_hex, '\n');
    getline(input_file, m_hex, '\n');
    getline(input_file, r_hex, '\n');
    getline(input_file, h_hex, '\n');

    reverse(p_hex.begin(), p_hex.end());
    reverse(g_hex.begin(), g_hex.end());
    reverse(y_hex.begin(), y_hex.end());
    reverse(m_hex.begin(), m_hex.end());
    reverse(r_hex.begin(), r_hex.end());
    reverse(h_hex.begin(), h_hex.end());

    bitset<MAX_SIZE> p = hex_str_to_bin(p_hex);
    bitset<MAX_SIZE> g = hex_str_to_bin(g_hex);
    bitset<MAX_SIZE> y = hex_str_to_bin(y_hex);
    bitset<MAX_SIZE> m = hex_str_to_bin(m_hex);
    bitset<MAX_SIZE> r = hex_str_to_bin(r_hex);
    bitset<MAX_SIZE> h = hex_str_to_bin(h_hex);

    ofstream output_file(argv[2]);
    //ofstream output_file(output_file_name);

    if (!output_file.is_open())
    {
        cout << "Can't open file " << argv[2] << endl;
        return 1;
    }

    int sign = 1;
    bitset<MAX_SIZE> p_1 = subtract_binary(p, bitset<MAX_SIZE>(1));

    if (compare_binary(r, p_1) == 1 || r[MAX_SIZE - 1] == 1) // r > p-1 || r < 0
        sign = 0;
    if (compare_binary(h, subtract_binary(p_1, bitset<MAX_SIZE>(1))) == 1 || h[MAX_SIZE - 1] == 1) // h > p-2 || h < 0 
        sign = 0;

    bitset<MAX_SIZE> g_m_p = power_mod(g, m, p); // g^m mod p
    
    bitset<MAX_SIZE> y_r_p = power_mod(y, r, p); // y^r mod p

    bitset<MAX_SIZE> r_h_p = power_mod(r, h, p); // r^h mod p

    bitset<MAX_SIZE> y_r_r_h_p = mul_mod(y_r_p, r_h_p, p); // y^r. r^h mod p

    if (g_m_p != y_r_r_h_p)
        sign = 0;

    cout << sign << endl;
    output_file << sign << endl;

    output_file.close();

    /*auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    cout << "Time: " << duration << " s" << endl;*/

    return 0;
}

bitset<4> hex_char_to_bin(char c)
{
    switch (toupper(c))
    {
    case '0': return bitset<4>("0000");
    case '1': return bitset<4>("0001");
    case '2': return bitset<4>("0010");
    case '3': return bitset<4>("0011");
    case '4': return bitset<4>("0100");
    case '5': return bitset<4>("0101");
    case '6': return bitset<4>("0110");
    case '7': return bitset<4>("0111");
    case '8': return bitset<4>("1000");
    case '9': return bitset<4>("1001");
    case 'A': return bitset<4>("1010");
    case 'B': return bitset<4>("1011");
    case 'C': return bitset<4>("1100");
    case 'D': return bitset<4>("1101");
    case 'E': return bitset<4>("1110");
    }
    return bitset<4>("1111");
}

bitset<MAX_SIZE> hex_str_to_bin(const string& hex)
{
    bitset<MAX_SIZE> bin;
    int length = hex.length() * 4;
    int bitset_size = min(length, MAX_SIZE);

    for (int i = 0; i < bitset_size; ++i)
    {
        bin <<= 1;
        char hex_char = hex[i / 4];
        int hex_value = hex_char_to_bin(hex_char).to_ulong();
        int shift = 3 - (i % 4);
        int bit = (hex_value >> shift) & 1;
        bin |= bit;
    }

    return bin;
}

bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{
    bitset<MAX_SIZE> result = a;
    result ^= b;
    bitset<MAX_SIZE> carry = (a & b) << 1;
    while (carry != 0)
    {
        bitset<MAX_SIZE> temp_result = result;
        result ^= carry;
        carry = (temp_result & carry) << 1;
    }
    return result;
}

int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{
    if (a[MAX_SIZE - 1] > b[MAX_SIZE - 1]) // sign a = 1 => a < 0 ; sign b = 0 => b >= 0 => a < b
        return -1;
    if (a[MAX_SIZE - 1] < b[MAX_SIZE - 1])
        return 1;

    for (int i = MAX_SIZE - 2; i >= 0; --i)
    {
        if (a[i] < b[i])
        {
            return -1; // a < b
        }
        else if (a[i] > b[i])
        {
            return 1; // a > b
        }
    }
    return 0; // a = b
}


bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{

    bitset<MAX_SIZE> result = a;
    result ^= b;
    bitset<MAX_SIZE> borrow = ((~a) & b) << 1;
    while (borrow != 0)
    {
        bitset<MAX_SIZE> temp_result = result;
        result ^= borrow;
        borrow = ((~temp_result) & borrow) << 1;
    }
    return result;
}

string binary_to_hex_little_endian(const bitset<MAX_SIZE>& binary)
{
    string bin;
    for (int i = 0; i < MAX_SIZE - 1; ++i)
        bin += (binary[i] + 48);

    int last_one_index = -1;
    for (int i = bin.size() - 1; i >= 0; --i)
    {
        if (bin[i] == '1')
        {
            last_one_index = i;
            break;
        }
    }

    if (last_one_index == -1) return "0";

    string result = bin.substr(0, last_one_index + 1);

    while (result.size() % 4 != 0)
    {
        result = result + "0";
    }

    reverse(result.begin(), result.end());

    string rest, tmp;

    for (int i = 0; i < result.length(); i += 4)
    {
        tmp = result.substr(i, 4);
        if (!tmp.compare("0000"))
        {
            rest = "0" + rest;
        }
        else if (!tmp.compare("0001"))
        {
            rest = "1" + rest;
        }
        else if (!tmp.compare("0010"))
        {
            rest = "2" + rest;
        }
        else if (!tmp.compare("0011"))
        {
            rest = "3" + rest;
        }
        else if (!tmp.compare("0100"))
        {
            rest = "4" + rest;
        }
        else if (!tmp.compare("0101"))
        {
            rest = "5" + rest;
        }
        else if (!tmp.compare("0110"))
        {
            rest = "6" + rest;
        }
        else if (!tmp.compare("0111"))
        {
            rest = "7" + rest;
        }
        else if (!tmp.compare("1000"))
        {
            rest = "8" + rest;
        }
        else if (!tmp.compare("1001"))
        {
            rest = "9" + rest;
        }
        else if (!tmp.compare("1010"))
        {
            rest = "A" + rest;
        }
        else if (!tmp.compare("1011"))
        {
            rest = "B" + rest;
        }
        else if (!tmp.compare("1100"))
        {
            rest = "C" + rest;
        }
        else if (!tmp.compare("1101"))
        {
            rest = "D" + rest;
        }
        else if (!tmp.compare("1110"))
        {
            rest = "E" + rest;
        }
        else if (!tmp.compare("1111"))
        {
            rest = "F" + rest;
        }
        else
        {
            continue;
        }
    }
    return rest;
}

bitset<MAX_SIZE> modulo(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& n)
{

    bool compare = compare_binary(x, n);
    if (compare == 0) return bitset<MAX_SIZE>(0);
    if (compare == -1) return x;

    bitset<MAX_SIZE> adjusted_x = x;
    bitset<MAX_SIZE> adjusted_n = n;

    while (compare_binary(adjusted_x, adjusted_n) == 1 || compare_binary(adjusted_x, adjusted_n) == 0)
    {
        adjusted_x = subtract_binary(adjusted_x, adjusted_n);
    }

    return adjusted_x;
}

bitset<MAX_SIZE> mul_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& y, const bitset<MAX_SIZE>& n)
{
    bitset<MAX_SIZE> temp_x = modulo(x, n);
    bitset<MAX_SIZE> result(0);

    int count = 0;
    for (int i = MAX_SIZE - 1; i >= 0; --i)
    {
        if (y[i] == 1) break;
        else
        {
            count++;
        }
    }

    if (y[0] == 1)
    {
        result = temp_x;
    }

    for (int i = 1; i < MAX_SIZE - count; ++i)
    {
        temp_x <<= 1; // temp_x = temp_x * 2
        temp_x = modulo(temp_x, n);

        if (y[i] == 1) {
            result = add_binary(result, temp_x);
            result = modulo(result, n);
        }
    }
    return result;
}

bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n)
{

    bitset<MAX_SIZE> result(1);

    bool start = false;

    for (int i = a.size() - 1; i >= 0; --i)
    {
        if (a[i] == 1)start = true;

        if (start)
        {
            // y = y^2 (mod n)
            result = mul_mod(result, result, n);

            if (a[i] == 1)
            {
                result = mul_mod(result, x, n); // y = y*x (mod n)
            }
        }
        else continue;
    }

    return result;
}

bitset<MAX_SIZE> inverse_mod(const bitset<MAX_SIZE>& s, const bitset<MAX_SIZE>& p)
{
    // cal s^-1 in s.s^-1 = 1 (mod p)
    // this algo base on bezout algo: ax + by = gcd(x,y) => s = x, p = y

    bitset<MAX_SIZE> g(1);
    bitset<MAX_SIZE> temp_p = p;
    bitset<MAX_SIZE> temp_s = s;


    while (temp_s[0] == 0 && temp_p[0] == 0)
    {
        temp_s >>= 1;
        if (temp_s[MAX_SIZE - 2] == 1)
            temp_s[MAX_SIZE - 1] = 1;

        temp_p >>= 1;
        if (temp_p[MAX_SIZE - 2] == 1)
            temp_p[MAX_SIZE - 1] = 1;

        g <<= 1;
    }

    bitset<MAX_SIZE> u = s; // u = s = x
    bitset<MAX_SIZE> v = p; // v = p = y
    bitset<MAX_SIZE> A(1);
    bitset<MAX_SIZE> B(0);
    bitset<MAX_SIZE> C(0);
    bitset<MAX_SIZE> D(1);

    while (u != 0 && u[MAX_SIZE - 1] != 1)
    {
        while (u[0] == 0)
        {
            u >>= 1;
            if (u[MAX_SIZE - 2] == 1)
                u[MAX_SIZE - 1] = 1;

            if (A[0] == 0 && B[0] == 0)
            {
                A >>= 1; // A=A/2
                if (A[MAX_SIZE - 2] == 1)
                    A[MAX_SIZE - 1] = 1;
                B >>= 1; // B=B/2
                if (B[MAX_SIZE - 2] == 1)
                    B[MAX_SIZE - 1] = 1;
            }
            else
            {
                A = add_binary(A, temp_p);
                A >>= 1;
                if (A[MAX_SIZE - 2] == 1)
                    A[MAX_SIZE - 1] = 1;
                B = subtract_binary(B, temp_s);
                B >>= 1;
                if (B[MAX_SIZE - 2] == 1)
                    B[MAX_SIZE - 1] = 1;
            }
        }

        while (v[0] == 0)
        {
            v >>= 1;
            if (v[MAX_SIZE - 2] == 1)
                v[MAX_SIZE - 1] = 1;
            if (C[0] == 0 && D[0] == 0)
            {
                C >>= 1;
                if (C[MAX_SIZE - 2] == 1)
                    C[MAX_SIZE - 1] = 1;
                D >>= 1;
                if (D[MAX_SIZE - 2] == 1)
                    D[MAX_SIZE - 1] = 1;
            }
            else
            {
                C = add_binary(C, temp_p);
                C >>= 1;
                if (C[MAX_SIZE - 2] == 1)
                    C[MAX_SIZE - 1] = 1;
                D = subtract_binary(D, temp_s);
                D >>= 1;
                if (D[MAX_SIZE - 2] == 1)
                    D[MAX_SIZE - 1] = 1;
            }
        }

        if (compare_binary(u, v) == -1)
        {
            v = subtract_binary(v, u);
            C = subtract_binary(C, A);
            D = subtract_binary(D, B);
        }
        else
        {
            u = subtract_binary(u, v);
            A = subtract_binary(A, C);
            B = subtract_binary(B, D);
        }
    }
    if (C[MAX_SIZE - 1] == 1)
        C = add_binary(C, p);
    return C;
}